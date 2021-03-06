/*
 * @file $/include/anta/dsel/util/priority_cast.hpp
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
#ifndef ANTA_DSEL_UTIL_PRIORITY_CAST_HPP_
#define ANTA_DSEL_UTIL_PRIORITY_CAST_HPP_

namespace anta { namespace dsel { namespace util {

/******************************************************************************/

namespace mpl = boost::mpl;

/**
 *	The priority<Type_, M_> metafunction defines a list of type priorities for
 *	binary operators that require both arguments to be of the same type, but
 *	also allow implicit type casting. If argument types are different then the
 *	argument which's type has lower priority gets converted to the type of the
 *	other argument.
 *
 *	@{ */

template <typename Type_, typename M_ = model<> >
struct priority: mpl::int_<9> {};

#define ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(TYPE, P) \
	template <typename M_> \
	struct priority<typename TYPE<M_>::type, M_>: mpl::int_<P> {};
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::null,		0)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::array,		1)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::integer,		2)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::real,		3)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::boolean,		4)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(string,			5)
ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY(aux::Variable,	6)
#undef ANTA_DSEL_UTIL_PRIORITY_CAST_DECLARE_PRIORITY

/**	@} */

/**
 *	probe<M_>(var) is a helper tool used to extract type priority from dynamic
 *	values at runtime.
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
 *	choose<U_, V_>::type is a metafunction that, given two types, returns the
 *	one that has greater priority.
 */
template <typename U_, typename V_>
struct choose
{
	typedef typename mpl::if_<mpl::greater<priority<U_>, priority<V_> >,
			U_, V_>::type type;

};

/******************************************************************************/

/**
 *	priority_cast_functor<L, R, Greater(L, R)> is a functor that delegates call
 *	either to left_cast or right_cast depending on which of the two given types
 *	has higher priority according to priority<Type_, M_> metafunction.
 *	@{ */
template <typename Left_, typename Right_, typename Greater_>
struct priority_cast_functor;

// (both operands are of the same type)
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

// (type priority of the right operand cannot be determined at compile time)
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

// (type priority of the left operand cannot be determined at compile time)
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

// (type priority of neither one of operands can be determined at compile time)
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
/**	@} */

/**
 *	priority_cast_type and priority_cast are a convenience type and its instance
 *	respectively used to simplify priority_cast_functor invocation syntax.
 *	@{ */
struct priority_cast_type
{
	template <typename Left_, typename Right_, typename F_>
	typename F_::result_type operator() (const Left_& a_left,
			const Right_& a_right, const F_& a_f) const
	{
		return priority_cast_functor<Left_, Right_,
			typename choose<Left_, Right_>::type>::apply(a_left, a_right, a_f);
	}

};

const priority_cast_type priority_cast = priority_cast_type();
/**	@} */

/******************************************************************************/

}}} // namespace anta::dsel::util

#endif /* ANTA_DSEL_UTIL_PRIORITY_CAST_HPP_ */
