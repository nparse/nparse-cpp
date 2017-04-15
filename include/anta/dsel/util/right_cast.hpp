/*
 * @file $/include/anta/dsel/util/right_cast.hpp
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
#ifndef ANTA_DSEL_UTIL_RIGHT_CAST_HPP_
#define ANTA_DSEL_UTIL_RIGHT_CAST_HPP_

namespace anta { namespace dsel { namespace util {

/******************************************************************************/

template <typename Left_, typename Right_>
struct right_cast_functor
{
	template <typename F_>
	static typename F_::result_type apply (const Left_& a_left,
			const Right_& a_right, const F_& a_f)
	{
		return aux::Variable<model<> >(a_right). applyInverseCast(a_f, a_left);
	}

};

template <typename Left_, typename M_>
struct right_cast_functor<Left_, aux::Variable<M_> >
{
	template <typename F_>
	static typename F_::result_type apply (const Left_& a_left,
			const aux::Variable<M_>& a_right, const F_& a_f)
	{
		return a_right. applyInverseCast(a_f, a_left);
	}

};

template <typename M_, typename Right_>
struct right_cast_functor<aux::Variable<M_>, Right_>
{
	template <typename F_>
	static typename F_::result_type apply (const aux::Variable<M_>& a_left,
			const Right_& a_right, const F_& a_f)
	{
		return aux::Variable<M_>(a_right). applyInverseCast(a_f, a_left);
	}

};

template <typename M_>
struct right_cast_functor<aux::Variable<M_>, aux::Variable<M_> >
{
	template <typename F_>
	static typename F_::result_type apply (const aux::Variable<M_>& a_left,
			const aux::Variable<M_>& a_right, const F_& a_f)
	{
		return a_right. applyInverseCast(a_f, a_left);
	}

};

struct right_cast_object
{
	template <typename Left_, typename Right_, typename F_>
	typename F_::result_type operator() (const Left_& a_left,
			const Right_& a_right, const F_& a_f) const
	{
		return right_cast_functor<Left_, Right_>::apply(a_left, a_right, a_f);
	}

};

const right_cast_object right_cast = right_cast_object();

/******************************************************************************/

}}} // namespace anta::dsel::util

#endif /* ANTA_DSEL_UTIL_RIGHT_CAST_HPP_ */
