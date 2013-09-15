/*
The MIT License (MIT)
Copyright (c) 2010-2013 Alex S Kudinov <alex@nparse.com>
 
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

#include <assert.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <utility/callback.hpp>
#include "exceptions.hpp"

namespace plugin {

/**
 *	Generic puggable object interface.
 */
class IInterface
{
public:
	/**
	 *	The destructor.
	 */
	virtual ~IInterface ()
	{
	}

};

/**
 *	Pluggable object factory interface.
 */
class IFactory: public IInterface
{
public:
	/**
	 *	Create an instance.
	 */
	virtual bool create (IInterface*& a_instance,
			const std::string& a_interface, const int a_version) = 0;

	/**
	 *	Destroy an instance.
	 */
	virtual void destroy (IInterface* a_instance) = 0;

	/**
	 *	Release all resources owned by the factory.
	 */
	virtual void release () = 0;

};

/**
 *	Static pluggable object manager.
 */
class manager: public boost::noncopyable
{
public:
	/**
	 *	Get a reference to the static manager instance.
	 */
	static manager& instance ();

	/**
	 *	Install a pluggable object factory.
	 */
	virtual void install (IFactory* a_factory, const std::string& a_interface,
			const int a_version, const bool a_factory_owner) = 0;

	/**
	 *	Uninstall a pluggable object factory.
	 */
	virtual void uninstall (IFactory* a_factory) = 0;

	/**
	 *	Check whether the given interface is provided.
	 */
	virtual bool is_provided (const std::string& a_interface,
			const int a_version = -1) const = 0;

	/**
	 *	Create an instance, given the interface name and version number.
	 *	@{ */
	virtual void create (const std::string& a_interface,
			const int a_version, IInterface*& a_instance) = 0;

	template <typename Interface_>
	void create (const std::string& a_interface, const int a_version,
			Interface_*& a_instance)
	{
		IInterface* instance_ = NULL;
		create(a_interface, a_version, instance_);
		a_instance = dynamic_cast<Interface_*>(instance_);
		if (a_instance == NULL)
		{
			destroy(instance_);
			throw interface_type_conflict();
		}
	}
	/**	@} */

	/**
	 *	Destroy an instance.
	 */
	virtual void destroy (IInterface* a_instance) = 0;

	/**
	 *	Enumerate all installed interfaces.
	 */
	typedef utility::callback<bool, const std::string&> callback_type;
	virtual bool list (const callback_type& a_callback) const = 0;

	/**
	 *	Shut down plugin subsystem.
	 */
	virtual void shutdown () = 0;

};

/**
 *	Pluggable object instantiator.
 */
template <typename Interface_>
class instance: public boost::noncopyable
{
	Interface_* m_ptr;

public:
	/**
	 *	Default constructor.
	 */
	instance ():
		m_ptr (NULL)
	{
	}

	/**
	 *	Instantiating constructor.
	 */
	instance (const std::string& a_interface, const int a_version = -1):
		m_ptr (NULL)
	{
		instantiate(a_interface, a_version);
	}

	/**
	 *	Cleaning up destructor.
	 */
	~instance ()
	{
		if (m_ptr != NULL)
		{
			Interface_* ptr = m_ptr;
			m_ptr = NULL;
			manager::instance(). destroy(ptr);
		}
	}

public:
	/**
	 *	Check if the interface is instantiated.
	 */
	operator bool () const
	{
		return m_ptr != NULL;
	}

	/**
	 *	Instantiate the interface.
	 */
	void instantiate (const std::string& a_interface, const int a_version = -1)
	{
		manager::instance(). create(a_interface, a_version, m_ptr);
	}

	/**
	 *	Interface casting operator.
	 *	@{ */
	Interface_& operator* ()
	{
		return *m_ptr;
	}

	const Interface_& operator* () const
	{
		return *m_ptr;
	}
	/**	@} */

	/**
	 *	Interface access operator.
	 *	@{ */
	Interface_* operator-> ()
	{
		return m_ptr;
	}

	const Interface_* operator-> () const
	{
		return m_ptr;
	}
	/** @} */

};

namespace
{

/**
 *	Automatic pluggable object factory.
 */
template <typename Implementation_>
class auto_factory: public IFactory
{
public:
	// Overridden from IFactory:

	bool create (IInterface*& a_instance, const std::string&, const int)
	{
		a_instance = new Implementation_();
		return true;
	}

	void destroy (IInterface* a_instance)
	{
		delete a_instance;
	}

	void release ()
	{
	}

};

/**
 *	Automatic pluggable object singleton.
 */
template <typename Implementation_>
class auto_singleton: public IFactory
{
	Implementation_* m_instance;

public:
	auto_singleton ():
		m_instance (NULL)
	{
	}

public:
	// Overridden from IFactory:

	bool create (IInterface*& a_instance, const std::string&, const int)
	{
		if (m_instance == NULL)
		{
			// This makes singletones being able to be instantiated recursively.
			m_instance = reinterpret_cast<Implementation_*>(
					new char[sizeof(Implementation_)]);
			new(m_instance) Implementation_();
		}
		a_instance = m_instance;
		return false;
	}

	void destroy (IInterface*)
	{
	}

	void release ()
	{
		if (m_instance != NULL)
		{
			Implementation_* instance = m_instance;
			m_instance = NULL;
			instance -> ~Implementation_();
			delete[] reinterpret_cast<char*>(instance);
		}
	}

};

} // namespace

/**
 *	A quick way to install a pluggable object.
 */
template <typename Implementation_ = void>
class install
{
public:
	install (const std::string& a_interface, const int a_version = 0)
	{
		manager::instance(). install(new auto_factory<Implementation_>(),
				a_interface, a_version, true);
	}

};

/**
 *	A quick way to install a pluggable object as a singleton.
 */
template <typename Implementation_>
class install_singleton
{
public:
	install_singleton (const std::string& a_interface,
			const int a_version = 0)
	{
		manager::instance(). install(new auto_singleton<Implementation_>(),
				a_interface, a_version, true);
	}

};

/**
 *	A common way to install a custom pluggable object factory into the plug-in
 *	management subsystem.
 */
template <>
class install<void>
{
public:
	install (IFactory* a_factory, const std::string& a_interface,
		const int a_version = 0);

};

} // namespace plugin

#endif /* PLUGIN_PLUGIN_HPP_ */
