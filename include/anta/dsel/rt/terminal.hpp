/*
 * @file $/include/anta/dsel/rt/terminal.hpp
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
#ifndef ANTA_DSEL_RT_TERMINAL_HPP_
#define ANTA_DSEL_RT_TERMINAL_HPP_

namespace anta { namespace dsel {

/******************************************************************************/

namespace proto = boost::proto;

/**
 *
 */
template <typename M_>
template <typename Void_>
struct GrammarCases<M_>::case_<proto::tag::terminal, Void_>:
	proto::or_<
		proto::terminal<Value<M_> >
	,	proto::terminal<Reference<M_> >
	,	proto::terminal<Placeholder<M_> >
	,	proto::terminal<const char*>
	,	proto::terminal<const wchar_t*>
	,	proto::or_<
			proto::terminal<typename aux::boolean<M_>::type>
		,	proto::terminal<typename aux::integer<M_>::type>
		,	proto::terminal<typename aux::real<M_>::type>
		,	proto::terminal<typename string<M_>::type>
		,	proto::terminal<typename aux::array<M_>::type>
		,	proto::terminal<typename ndl::context_value<M_>::type>
		>
	>
{};

/**
 *
 */
namespace {

// Terminal evaluation: the default behavior.
template <typename M_, typename Type_>
class implementation
{
public:
	typedef const Type_& result_type;

	template <typename Context_>
	static result_type f (const Type_& a_value, const Context_&)
	{
		return a_value;
	}

};

// Terminal evaluation: support for the address references.
template <typename M_>
class implementation<M_, Value<M_> >
{
public:
	typedef typename Value<M_>::val_type result_type;

	template <typename Context_>
	static result_type f (const Value<M_>& a_value, const Context_& a_context)
	{
		return a_value. val(a_context);
	}

};

// Terminal evaluation: support for the nominal references.
template <typename M_>
class implementation<M_, Reference<M_> >
{
public:
	typedef typename Reference<M_>::val_type result_type;

	template <typename Context_>
	static result_type f (const Reference<M_>& a_reference,
			const Context_& a_context)
	{
		return a_reference. val(a_context);
	}

};

// Terminal evaluation: support for the agent-dependent values (placeholders).
template <typename M_>
class implementation<M_, Placeholder<M_> >
{
public:
	typedef typename ndl::context_value<M_>::type result_type;

	template <typename Context_>
	static result_type f (const Placeholder<M_>& a_placeholder,
			const Context_& a_context)
	{
		return a_placeholder(a_context. getAgent());
	}

};

// Terminal evaluation: support for narrow strings.
template <typename M_>
class implementation<M_, const char*>
{
public:
	typedef typename string<M_>::type result_type;

	template <typename Context_>
	static result_type f (const char* a_str, const Context_&)
	{
		return encode::make<result_type>::from(a_str);
	}

};

// Terminal evaluation: support for wide strings.
template <typename M_>
class implementation<M_, const wchar_t*>
{
public:
	typedef typename string<M_>::type result_type;

	template <typename Context_>
	static result_type f (const wchar_t* a_str, const Context_&)
	{
		return encode::make<result_type>::from(a_str);
	}

};

// Terminal evaluation: a workaround for narrow string constants.
template <typename M_, int N_> class implementation<M_, char[N_]>:
	public implementation<M_, const char*> {};

// Terminal evaluation: a workaround for wide string constants.
template <typename M_, int N_> class implementation<M_, wchar_t[N_]>:
	public implementation<M_, const wchar_t*> {};

} // namespace

/**
 *
 */
template <typename M_, typename Expr_>
class eval<M_, Expr_, proto::tag::terminal, caRead>
{
public:
	typedef implementation<M_, typename proto::result_of::value<Expr_>::type>
		base_type;
	typedef typename base_type::result_type result_type;

	template <typename Context_>
	result_type operator() (const Expr_& a_expr, Context_& a_context) const
	{
		Context_::isReader();
		return base_type::f(proto::value(a_expr), a_context);
	}

};

/**
 *
 */
template <typename M_, typename Expr_>
class eval<M_, Expr_, proto::tag::terminal, caWrite>
{
public:
	typedef typename ndl::context_value<M_>::type& result_type;

	template <typename Context_>
	result_type operator() (const Expr_& a_expr, Context_& a_context) const
	{
		Context_::isWriter();
		return proto::value(a_expr). ref(a_context);
	}

};

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_RT_TERMINAL_HPP_ */
