/*
 * @file $/include/anta/dsel/util/make_bool.hpp
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
#ifndef ANTA_DSEL_UTIL_MAKE_BOOL_HPP_
#define ANTA_DSEL_UTIL_MAKE_BOOL_HPP_

namespace anta { namespace dsel { namespace util {

/******************************************************************************/

/**
 *
 *	@{ */

template <typename M_, bool Default_ = true, typename Interim_ = void>
class make_bool
{
public:
	static bool from (const Interim_&)
	{
		return Default_;
	}

};

template <typename M_, bool Default_>
class make_bool<M_, Default_, void>
{
public:
	template <typename T_>
	static bool from (const T_& a_value)
	{
		return make_bool<M_, Default_, T_>::from(a_value);
	}

};

template <typename M_, bool Default_>
class make_bool<M_, Default_, bool>
{
public:
	static bool from (const bool a_value)
	{
		return a_value;
	}

};

template <typename M_, bool Default_>
class make_bool<M_, Default_, aux::Variable<M_> >
{
public:
	static bool from (const aux::Variable<M_>& a_value)
	{
		try
		{
			return a_value. as_boolean();
		}
		catch (const std::bad_cast&)
		{
			return Default_;
		}
	}

};

/**	@} */

/******************************************************************************/

}}} // namespace anta::dsel::util

#endif /* ANTA_DSEL_UTIL_MAKE_BOOL_HPP_ */
