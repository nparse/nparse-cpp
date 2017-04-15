/*
The MIT License (MIT)
Copyright (c) 2010-2017 Alex Kudinov <alex.s.kudinov@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef PLUGIN_PLUGIN_HPP_
#define PLUGIN_PLUGIN_HPP_

#include <stdexcept>
#include <string>
#include <boost/noncopyable.hpp>
#include <util/callback.hpp>
#include "exceptions.hpp"

namespace plugin {

/**
 *	IPluggable is a generic interface for classes whose instance lifecycle
 *	management is intended to be delegated to the plugin management library.
 */
class IPluggable
{
public:
	/**
	 *	Polymorphic destructor.
	 */
	virtual ~IPluggable () {}

	/**
	 *	Execute post-construction initialization step.
	 */
	virtual void initialize () {}

};

/**
 *	IFactory is an interface for classes that are responsible for managing
 *	individual pluggable object lifecycles.
 */
class IFactory
{
public:
	/**
	 *	Polymorphic destructor.
	 */
	virtual ~IFactory () {}

	/**
	 *	Allocate memory for a new instance.
	 *	@return Allocated memory address
	 */
	virtual void* allocate () = 0;

	/**
	 *	Construct an instance at the given memory address.
	 *	@param a_address Allocated memory address
	 *	@return Constructed instance pointer
	 */
	virtual IPluggable* construct (void* a_address) = 0;

	/**
	 *	Perform post-construction initialization step on a constructed instance.
	 *	@param a_instance Constructed instance pointer
	 *	@return Initialized instance pointer
	 */
	virtual IPluggable* initialize (IPluggable* a_instance) = 0;

	/**
	 *	Destroy a fully initialized instance and release all its resources.
	 *	@param a_instance Initialized instance pointer
	 *	@return Previously allocated memory address
	 */
	virtual void* destruct (IPluggable* a_instance) = 0;

	/**
	 *	Release memory allocated for an instance.
	 *	@param a_address Allocated memory address
	 */
	virtual void deallocate (void* a_address) = 0;

};

/**
 *	IManager is an interface for a class responsible for managing multiple
 *	pluggable object lifecycles through their respective factories.
 */
class IManager
{
public:
	/**
	 *	Get the static (default) manager implementation.
	 */
	static IManager& instance ();

	/**
	 *	Register an instance factory within the manager.
	 */
	virtual bool register_factory (const std::string& a_interface_tag,
			IFactory* a_factory, const bool a_delegate_factory_ownership) = 0;

	/**
	 *	Unregister an instance factory and forcefully destroy all instances
	 *	produced by it.
	 */
	virtual bool unregister_factory (IFactory* a_factory) = 0;

	/**
	 *	Check if a factory producing instances of the given type exists.
	 */
	virtual bool factory_exists (const std::string& a_interface_tag) const = 0;

	/**
	 *	Create a new instance given an interface tag.
	 */
	virtual IPluggable* create (const std::string& a_interface_tag) = 0;

	/**
	 *	Validate an instance pointer.
	 */
	virtual IPluggable* validate (const IPluggable* a_instance) const = 0;

	/**
	 *	Dispose a previously created instance. It does not necessarily destroy
	 *	the the instance since it may be used elsewhere.
	 */
	virtual bool dispose (IPluggable* a_instance) = 0;

	/**
	 *	The callback argument type for the list(..) member function.
	 */
	typedef utility::callback<bool, const std::string&> callback_type;

	/**
	 *	List all registered interfaces.
	 */
	virtual void list (const callback_type& a_callback) const = 0;

	/**
	 *	Forcefully destroy all instances and owned factories.
	 */
	virtual void shutdown () = 0;

};

/**
 *	simple_factory<T_> is a template class for trivial factories that produce
 *	pluggable objects of type T_, where T_ must be derived from IPluggable.
 */
template <typename T_>
class simple_factory: public IFactory
{
public:
	// Overridden from IFactory:

	void* allocate ()
	{
		return static_cast<void*>(new char[sizeof(T_)]);
	}

	IPluggable* construct (void* a_address)
	{
		return new(a_address) T_();
	}

	IPluggable* initialize (IPluggable* a_instance)
	{
		return a_instance;
	}

	void* destruct (IPluggable* a_instance)
	{
		T_* ptr = dynamic_cast<T_*>(a_instance);
		if (ptr)
		{
			ptr -> ~T_();
		}
		return ptr;
	}

	void deallocate (void* a_address)
	{
		delete[] reinterpret_cast<char*>(a_address);
	}

};

/**
 *	singleton_factory<T_> ...
 */
template <typename T_>
class singleton_factory: public IFactory
{
public:
	// Overridden from IFactory:

	void* allocate ()
	{
		return this;
	}

	IPluggable* construct (void* a_address)
	{
		return reinterpret_cast<IPluggable*>(a_address);
	}

	IPluggable* initialize (IPluggable* a_instance)
	{
		return a_instance;
	}

	void* destruct (IPluggable* a_instance)
	{
		return dynamic_cast<T_*>(a_instance);
	}

	void deallocate (void* a_address)
	{
	}

};

/**
 *	auto_factory<T_> is an extended version of simple_factory<T_> template class
 *	with the ability to register itself within the static manager.
 */
template <typename T_, template <typename T1_> class B_ = simple_factory>
class auto_factory: public B_<T_>
{
public:
	/**
	 *	The only constructor. Registers the factory within the static manager.
	 */
	auto_factory (const std::string& a_interface_tag)
	{
		if (! IManager::instance(). register_factory(a_interface_tag, this,
					false))
		{
			throw interface_name_conflict();
		}
	}

	/**
	 *	The destructor. Unregisters the factory and destroys all produced
	 *	instances of the pluggable object.
	 */
	~auto_factory ()
	{
		IManager::instance(). unregister_factory(this);
	}

};

/**
 *	install<T_> is an auxiliary template class that instantiates simple
 *	factories and delegates their ownership to the static manager.
 */
template <typename T_>
class install
{
public:
	install (const std::string& a_interface_tag)
	{
		IManager::instance(). register_factory(a_interface_tag,
				new simple_factory<T_>(), true);
	}

};

/**
 *	instance<T_> is a template class that represents an injection point for a
 *	pluggable object managed by the static manager.
 */
template <typename T_>
class instance: public boost::noncopyable
{
public:
	/**
	 *	The only constructor. Acquires a pointer to the pluggable object from
	 *	the static manager.
	 *	@param a_interface_tag Interface tag
	 */
	instance (const std::string& a_interface_tag):
		m_instance (NULL)
	{
		IPluggable* ptr = IManager::instance(). create(a_interface_tag);
		if (! ptr)
		{
			throw interface_not_found();
		}
		m_instance = dynamic_cast<T_*>(ptr);
		if (! m_instance)
		{
			IManager::instance(). dispose(ptr);
			throw interface_type_conflict();
		}
	}

	/**
	 *	Destructor. Disposes the underlying pluggable object.
	 */
	~instance ()
	{
		IManager::instance(). dispose(m_instance);
		m_instance = NULL;
	}

	/**
	 *	Get a mutable reference to the pluggable object.
	 */
	T_& operator* ()
	{
		return *dynamic_cast<T_*>(IManager::instance(). validate(m_instance));
	}

	/**
	 *	Get a constant reference to the pluggable object.
	 */
	const T_& operator* () const
	{
		return *dynamic_cast<T_*>(IManager::instance(). validate(m_instance));
	}

	/**
	 *	Provides read/write access to the pluggable object.
	 */
	T_* operator-> ()
	{
		return &**this;
	}

	/**
	 *	Provides read-only access to the pluggable object.
	 */
	const T_* operator-> () const
	{
		return &**this;
	}

private:
	T_* m_instance;

};

} // namespace plugin

#endif /* PLUGIN_PLUGIN_HPP_ */
