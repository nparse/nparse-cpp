/*
 * @file $/include/anta/dsel/rt/equal_to.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

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
		return util::priority_cast(
				proto::eval(proto::left(a_expr), a_context),
				proto::eval(proto::right(a_expr), a_context),
				*this);
	}

public:
	template <typename U_, typename V_>
	bool operator() (U_, V_) const
	{
		throw std::logic_error("operator '==': incomparable operands");
	}

#if 1
	// NOTE: Despite the null value is incomparable, two null values are still
	//		 equivalent. This gives us the opportunity to compare values against
	//		 NULL.
	bool operator() (const typename aux::null<M_>::type&,
			const typename aux::null<M_>::type&) const
	{
		return true;
	}
#endif

	template <typename V_>
	bool operator() (const V_& a_left, const V_& a_right,
		typename boost::enable_if<comparable<M_, V_> >::type* a0 = NULL) const
	{
		return a_left == a_right;
	}

};

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_RT_EQUAL_TO_HPP_ */
