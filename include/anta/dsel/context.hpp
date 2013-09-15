/*
 * @file $/include/anta/dsel/context.hpp
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
#ifndef ANTA_DSEL_CONTEXT_HPP_
#define ANTA_DSEL_CONTEXT_HPP_

namespace anta { namespace dsel {

/******************************************************************************/

namespace proto = boost::proto;

/**
 *
 */
enum context_access_t
{
	caRead = 444,
	caWrite = 666

};

// (forward declarations)
template <typename M_, typename Expr_, typename Tag_, context_access_t Access_>
	class eval;
template <typename M_, typename Agent_> class ContextReader;
template <typename M_, typename Agent_> class ContextWriter;

/**
 *
 */
template <typename M_, context_access_t Access_>
class ContextBasic
{
public:
	template <typename Expr_, typename Tag_ =
		typename proto::tag_of<Expr_>::type>
	class eval: public dsel::eval<M_, Expr_, Tag_, Access_>
	{
	};

};

/**
 *
 */
template <typename M_, typename Agent_>
class ContextReader: public ContextBasic<M_, caRead>
{
public:
	typedef typename Agent_::key_type key_type;
	typedef typename Agent_::value_type value_type;

	ContextReader (Agent_& a_agent):
		m_agent (a_agent)
	{
	}

	const value_type& val (const key_type& a_key) const
	{
		return m_agent. val(a_key);
	}

//	const ContextReader<M_, Agent_>& getReader () const
//	{
//		return *this;
//	}

	ContextWriter<M_, Agent_> getWriter () const
	{
		return ContextWriter<M_, Agent_>(m_agent);
	}

	static void isReader () {}

	Agent_& getAgent () const
	{
		return m_agent;
	}

private:
	Agent_& m_agent;

};

/**
 *
 */
template <typename M_, typename Agent_>
class ContextWriter: public ContextBasic<M_, caWrite>
{
public:
	typedef typename Agent_::key_type key_type;
	typedef typename Agent_::value_type value_type;

	ContextWriter (Agent_& a_agent):
		m_agent (a_agent)
	{
	}

	value_type& ref (const key_type& a_key, const bool a_reset = false) const
	{
		return m_agent. ref(a_key, a_reset);
	}

	ContextReader<M_, Agent_> getReader () const
	{
		return ContextReader<M_, Agent_>(m_agent);
	}

//	const ContextWriter<M_, Agent_>& getWriter () const
//	{
//		return *this;
//	}

	static void isWriter () {}

	Agent_& getAgent () const
	{
		return m_agent;
	}

private:
	Agent_& m_agent;

};

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_CONTEXT_HPP_ */
