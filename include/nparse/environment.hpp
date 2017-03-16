/*
 * @file $/include/nparse/environment.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.6

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef NPARSE_ENVIRONMENT_HPP_
#define NPARSE_ENVIRONMENT_HPP_

namespace nparse {

namespace {

template <typename Key_>
struct is_global_functor;

template <typename Key_>
inline bool is_global (const Key_& a_key)
{
	return is_global_functor<Key_>::f(a_key);
}

template <>
struct is_global_functor<std::string>
{
	static bool f (const std::string& a_key)
	{
		return a_key. empty() || *a_key. begin() != '_';
	}

};

#if defined(NPARSE_UTIL_HASHED_STRING_HPP_)
template <>
struct is_global_functor<hashed_string>
{
	static bool f (const hashed_string& a_key)
	{
		return (a_key. hash() & 1) == 0;
	}

};
#endif

} // namespace

// The only constructor.
IEnvironment::IEnvironment (anta::Traveller<NLG>& a_traveller,
		const bool a_local_only):
	m_traveller (a_traveller), m_local_only (a_local_only), m_local_context ()
{
}

// The destructor.
IEnvironment::~IEnvironment ()
{
}

// Get the traveller.
anta::Traveller<NLG>& IEnvironment::get_traveller () const
{
	return m_traveller;
}

// Get the current context.
anta::ndl::Context<NLG>* IEnvironment::self (const bool a_create) const
{
	return m_traveller. get_state(). context(a_create ? &m_traveller : NULL);
}

// Get a mutable reference to a trace variable from the local context.
IEnvironment::value_type& IEnvironment::ref (const key_type& a_key,
		const bool a_reset)
{
	if (!m_local_only && is_global(a_key))
	{
		return m_traveller. ref(a_key, a_reset);
	}
	else
	{
		if (m_local_context. get() == NULL)
		{
			m_local_context. reset(new anta::ndl::Context<NLG>(self()));
		}
		return m_local_context -> ref(a_key, a_reset);
	}
}

// Get a constant reference to a trace variable from the global context.
const IEnvironment::value_type& IEnvironment::val (const key_type& a_key) const
{
	if (!m_local_only && is_global(a_key))
	{
		return m_traveller. val(a_key);
	}
	else
	if (m_local_context. get() != NULL)
	{
		return m_local_context -> val(a_key);
	}
	else
	{
		return anta::ndl::context<NLG>::def();
	}
}

// Create an additional stored context.
anta::ndl::context<NLG>::type IEnvironment::create (
		const anta::ndl::Context<NLG>* a_ancestor) const
{
	return anta::ndl::context<NLG>::type(m_traveller. create(a_ancestor));
}

} // namespace nparse

#endif /* NPARSE_ENVIRONMENT_HPP_ */
