/*
 * @file $/contrib/include/utility/callback.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex@nparse.com>
 
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
#ifndef UTILITY_CALLBACK_HPP_
#define UTILITY_CALLBACK_HPP_

#include <assert.h>
#include <stdexcept>
#include <boost/config.hpp>

namespace utility {

/**
 *	An auxiliary specializable functor for copying of temporary callable objects
 *	given as constant expressions (rvalue).
 */
template <typename T_>
class rcopy
{
public:
	typedef T_ type;
	typedef const T_& ref;
	static ref f (ref a_ref)
	{
		return a_ref;
	}

};

/**
 *	The generic single-argument callback object.
 *		The main purpose of this template class is to create arbitrary callback
 *		objects that are able to be passed between different program modules and
 *		fit some simple invocation protocol (the types of the argument and the
 *		return value are strictly defined).
 */
template <typename Result_ = void, typename Argument_ = void>
class callback
{
	// @todo: use shared_ptr<proxy_base> instead of counter

	/**
	 *	The inner sharable proxy interface.
	 */
	class proxy_base
	{
		int m_counter;

	public:
		proxy_base ():
			m_counter (1)
		{
		}

		virtual ~proxy_base ()
		{
			assert(m_counter == 0);
		}

		void lock ()
		{
			assert(m_counter > 0);
			++ m_counter;
		}

		bool release ()
		{
			assert(m_counter > 0);
			return -- m_counter == 0;
		}

	public:
		virtual Result_ operator() (Argument_ a_arg) = 0;
		virtual proxy_base* clone () const = 0;

	};

	/**
	 *	A template proxy for invoking functors, regular functions and static
	 *	functions.
	 */
	template <typename Self_, typename Cloned_ = Self_>
	class proxy_static: public proxy_base
	{
		Self_ m_ref;

	public:
		proxy_static (Self_ a_ref):
			proxy_base (), m_ref (a_ref)
		{
		}

	public:
		// Overridden from proxy_base:
		Result_ operator() (Argument_ a_arg)
		{
			return static_cast<Result_>(m_ref(a_arg));
		}

		proxy_base* clone () const
		{
			return new proxy_static<Cloned_>(m_ref);
		}

	};

	/**
	 *	A template proxy for invoking non-static member functions.
	 */
	template <typename ClassRef_, typename FuncPtr_>
	class proxy_member: public proxy_base
	{
		ClassRef_ m_ref;
		FuncPtr_ m_ptr;

	public:
		proxy_member (ClassRef_ a_ref, FuncPtr_ a_ptr):
			proxy_base (), m_ref (a_ref), m_ptr (a_ptr)
		{
		}

	public:
		// Overridden from proxy_base:
		Result_ operator() (Argument_ a_arg)
		{
			return (m_ref .* m_ptr)(a_arg);
		}

		proxy_base* clone () const
		{
			return new proxy_member(m_ref, m_ptr);
		}

	};

	proxy_base* m_proxy; /**< Pointer to an instance of the inner proxy class. */

public:
	/**
	 *	The default constructor.
	 */
	callback ():
		m_proxy (NULL)
	{
	}

	/**
	 *	A templatized constructor for invocation of constant functors.
	 */
	template <typename Action_>
	callback (const Action_& a_action):
		m_proxy (new proxy_static<
			typename rcopy<Action_>::ref, typename rcopy<Action_>::type>(
				rcopy<Action_>::f(a_action)))
	{
	}

	/**
	 *	A templatized constructor for invocation of non-constant functors.
	 */
	template <typename Action_>
	callback (Action_& a_action):
		m_proxy (new proxy_static<Action_&, Action_>(a_action))
	{
	}

	/**
	 *	A special constructor for invocation of regular and static functions.
	 */
	callback (Result_ (*a_action)(Argument_)):
		m_proxy (new proxy_static<Result_ (*)(Argument_)>(a_action))
	{
	}

	/**
	 *	A set of templatized constructors for invocation of non-static member
	 *	functions.
	 *	@{ */

	// (instance is given by a reference)
	template <typename Class_, typename FuncPtr_>
	callback (Class_& a_ref, FuncPtr_ a_ptr):
		m_proxy (new proxy_member<Class_&, FuncPtr_>(a_ref, a_ptr))
	{
	}

	// (instance is given by a pointer)
	template <typename Class_, typename FuncPtr_>
	callback (Class_* a_ref, FuncPtr_ a_ptr):
		m_proxy (new proxy_member<Class_&, FuncPtr_>(*a_ref, a_ptr))
	{
	}

	// (constant instance is given by a reference)
	template <typename Class_, typename FuncPtr_>
	callback (const Class_& a_ref, FuncPtr_ a_ptr):
		m_proxy (new proxy_member<const Class_&, FuncPtr_>(a_ref, a_ptr))
	{
	}

	// (constant instance is given by a pointer)
	template <typename Class_, typename FuncPtr_>
	callback (const Class_* a_ref, FuncPtr_ a_ptr):
		m_proxy (new proxy_member<const Class_&, FuncPtr_>(*a_ref, a_ptr))
	{
	}

	/**	@} */

	/**
	 *	The destructor.
	 */
	~callback ()
	{
		clear();
	}

public:
	/**
	 *	The copy constructor.
	 */
	callback (const callback& a_src):
		m_proxy ((a_src. m_proxy == NULL) ? NULL : a_src. m_proxy -> clone())
	{
	}

	/**
	 *	The sharing constructor.
	 */
#if !defined(BOOST_MSVC)
	callback (callback& a_src):
		m_proxy (a_src. m_proxy)
	{
		if (m_proxy != NULL)
		{
			m_proxy -> lock();
		}
	}
#endif

	/**
	 *	The copy assignment operator.
	 */
	const callback& operator= (const callback& a_src)
	{
		if (this != &a_src)
		{
			this -> ~callback();
			new(this) callback(a_src);
		}
		return *this;
	}

public:
	/**
	 *	Check whether the callback object is void.
	 */
	bool empty () const
	{
		return (m_proxy == NULL);
	}

	/**
	 *	Clear the callback object.
	 */
	void clear ()
	{
		if (m_proxy != NULL)
		{
			if (m_proxy -> release())
				delete m_proxy;
			m_proxy = NULL;
		}
	}

	/**
	 *	Invoke the callback.
	 */
	Result_ operator() (Argument_ a_arg) const
	{
		if (m_proxy == NULL)
			throw std::logic_error("void callback");
		return (*m_proxy)(a_arg);
	}

};

} // namespace utility

#endif /* UTILITY_CALLBACK_HPP_ */
