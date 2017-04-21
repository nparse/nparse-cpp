/*
 * @file $/include/anta/dsel/rt/equal_to.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.8

The MIT License (MIT)
Copyright (c) 2007-2017 Alex Kudinov <alex.s.kudinov@gmail.com>

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
#ifndef ANTA_DSEL_RT_EQUAL_TO_HPP_
#define ANTA_DSEL_RT_EQUAL_TO_HPP_

namespace anta { namespace dsel {

/******************************************************************************/

namespace proto = boost::proto;

/**
 *
 */
template <typename M_>
template <typename Void_>
struct GrammarCases<M_>::case_<proto::tag::equal_to, Void_>:
	proto::equal_to<Grammar<M_>, Grammar<M_> > {};

/**
 *
 */
template <typename M_, typename Expr_>
class eval<M_, Expr_, proto::tag::equal_to, caRead>
{
public:
	typedef bool result_type;

	template <typename Context_>
	result_type operator() (const Expr_& a_expr, Context_& a_context) const
	{
		Context_::isReader();
		return (*this)(
				proto::eval(proto::left(a_expr), a_context),
				proto::eval(proto::right(a_expr), a_context),
				NULL);
	}

public:
	result_type operator() (const aux::Variable<M_>& u,
			const aux::Variable<M_>& v, void*) const
	{
		if (u. is_null()) return v. is_null();
		else if (v. is_null()) return false;
		else return util::priority_cast(u, v, *this);
	}

	template <typename U_>
	result_type operator() (const U_&, const typename aux::null<M_>::type&,
			void*) const
	{
		return false;
	}

	template <typename V_>
	result_type operator() (const typename aux::null<M_>::type&, const V_&,
			void*) const
	{
		return false;
	}

	result_type operator() (const typename aux::null<M_>::type&,
			const typename aux::null<M_>::type&, void*) const
	{
		return true;
	}

	template <typename U_, typename V_>
	result_type operator() (const U_& u, const V_& v, void*) const
	{
		return util::priority_cast(u, v, *this);
	}

public:
	result_type operator() (const typename aux::null<M_>::type&,
			const typename aux::null<M_>::type&) const
	{
		return true;
	}

	template <typename U_>
	result_type operator() (const U_& a_left, const U_& a_right,
		typename boost::enable_if<comparable<M_, U_> >::type* a0 = NULL) const
	{
		return a_left == a_right;
	}

	template <typename U_, typename V_>
	result_type operator() (U_, V_) const
	{
		throw std::logic_error("operator '==': incomparable operands");
	}

};

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_RT_EQUAL_TO_HPP_ */
