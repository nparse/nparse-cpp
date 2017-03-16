/*
 * @file $/include/anta/dsel/util/priority_cast.hpp
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
#ifndef ANTA_DSEL_UTIL_PRIORITY_CAST_HPP_
#define ANTA_DSEL_UTIL_PRIORITY_CAST_HPP_

namespace anta { namespace dsel { namespace util {

/******************************************************************************/

namespace mpl = boost::mpl;

/**
 *
 *	@{ */

template <typename T_, typename M_ = model<> >
struct priority: mpl::int_<9> {};

#define ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(TYPE, P) \
	template <typename M_> \
	struct priority<typename TYPE<M_>::type, M_>: mpl::int_<P> {};
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::null,		0)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::boolean,		1)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::integer,		2)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::real,		3)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(string,			4)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::array,		5)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::Variable,	6)
#undef ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY

/**	@} */

/**
 *
 *	@{ */

template <typename M_>
struct probe_functor
{
	typedef int result_type;

	template <typename V_>
	result_type operator() (const V_&) const
	{
		return static_cast<result_type>(priority<V_, M_>());
	}

};

template <typename M_>
inline typename probe_functor<M_>::result_type probe (
		const aux::Variable<M_>& a_variable)
{
	return a_variable. apply(probe_functor<M_>());
}

/**	@} */

/**
 *
 */
template <typename U_, typename V_>
struct choose
{
	typedef typename mpl::if_<mpl::greater<priority<U_>, priority<V_> >,
			U_, V_>::type type;

};

/******************************************************************************/

template <typename Left_, typename Right_, typename Greater_>
struct priority_cast_functor;

// (operands are of the same type)
template <typename Left_>
struct priority_cast_functor<Left_, Left_, Left_>
{
	template <typename F_>
	static typename F_::result_type apply (const Left_& a_left,
			const Left_& a_right, const F_& a_f)
	{
		return left_cast(a_left, a_right, a_f);
	}

};

// (left type has greater priority)
template <typename Left_, typename Right_>
struct priority_cast_functor<Left_, Right_, Left_>
{
	template <typename F_>
	static typename F_::result_type apply (const Left_& a_left,
			const Right_& a_right, const F_& a_f)
	{
		return left_cast(a_left, a_right, a_f);
	}

};

// (right type has greater priority)
template <typename Left_, typename Right_>
struct priority_cast_functor<Left_, Right_, Right_>
{
	template <typename F_>
	static typename F_::result_type apply (const Left_& a_left,
			const Right_& a_right, const F_& a_f)
	{
		return right_cast(a_left, a_right, a_f);
	}

};

// (the type priority of the right operand cannot be determined on compile time)
template <typename M_, typename Left_>
struct priority_cast_functor<Left_, aux::Variable<M_>, aux::Variable<M_> >
{
	template <typename F_>
	static typename F_::result_type apply (const Left_& a_left,
			const aux::Variable<M_>& a_right, const F_& a_f)
	{
		return (priority<Left_, M_>() < probe(a_right))
			? right_cast(a_left, a_right, a_f)
			: left_cast(a_left, a_right, a_f);
	}

};

// (the type priority of the left operand cannot be determined on compile time)
template <typename M_, typename Right_>
struct priority_cast_functor<aux::Variable<M_>, Right_, aux::Variable<M_> >
{
	template <typename F_>
	static typename F_::result_type apply (const aux::Variable<M_>& a_left,
			const Right_& a_right, const F_& a_f)
	{
		return (probe(a_left) < priority<Right_, M_>())
			? right_cast(a_left, a_right, a_f)
			: left_cast(a_left, a_right, a_f);
	}

};

// (none of the type priorities of any of the operands cannot be determined on
// compile time)
template <typename M_>
struct priority_cast_functor<aux::Variable<M_>, aux::Variable<M_>,
	  aux::Variable<M_> >
{
	template <typename F_>
	static typename F_::result_type apply (const aux::Variable<M_>& a_left,
			const aux::Variable<M_>& a_right, const F_& a_f)
	{
		return (probe(a_left) < probe(a_right))
			? right_cast(a_left, a_right, a_f)
			: left_cast(a_left, a_right, a_f);
	}

};

struct priority_cast_object
{
	template <typename Left_, typename Right_, typename F_>
	typename F_::result_type operator() (const Left_& a_left,
			const Right_& a_right, const F_& a_f) const
	{
		return priority_cast_functor<Left_, Right_,
			typename choose<Left_, Right_>::type>::apply(a_left, a_right, a_f);
	}

};

const priority_cast_object priority_cast = priority_cast_object();

/******************************************************************************/

}}} // namespace anta::dsel::util

#endif /* ANTA_DSEL_UTIL_PRIORITY_CAST_HPP_ */
