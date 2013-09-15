/*
 * @file $/include/anta/dsel/util/make_key.hpp
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
#ifndef ANTA_DSEL_UTIL_MAKE_KEY_HPP_
#define ANTA_DSEL_UTIL_MAKE_KEY_HPP_

namespace anta { namespace dsel { namespace util {

/******************************************************************************/

template <typename U_, typename V_>
class transform
{
public:
	static V_ f (const U_&)
	{
		throw std::bad_cast();
	}

};

template <typename Char_>
class transform<int, std::basic_string<Char_> >
{
public:
	typedef std::basic_string<Char_> string;

	static string f (const int u)
	{
		return boost::lexical_cast<string>(u);
	}

};

template <typename Char_>
class transform<std::basic_string<Char_>, int>
{
public:
	typedef std::basic_string<Char_> string;

	static int f (const string& u)
	{
		return boost::lexical_cast<int>(u);
	}

};

template <typename Char1_, typename Char2_>
class transform_string
{
public:
	typedef std::basic_string<Char1_> string1;
	typedef std::basic_string<Char2_> string2;

	static string2 f (const string1& s)
	{
		return encode::make<string2>::from(s);
	}

};

template <typename Char_>
class transform_string<Char_, Char_>
{
public:
	typedef std::basic_string<Char_> string;

	static const string& f (const string& s)
	{
		return s;
	}

};

template <typename Char1_, typename Char2_>
class transform<std::basic_string<Char1_>, std::basic_string<Char2_> >:
	public transform_string<Char1_, Char2_> {};

/**
 *
 *	@{ */

template <typename M_>
class make_key
{
public:
	typedef typename ndl::context_key<M_>::type result_type;

	template <typename T_>
	static result_type from (const T_& a_t)
	{
		return transform<typename encode::unwrapper<T_>::type,
			   typename encode::unwrapper<result_type>::type>::f(a_t);
	}

	static const result_type& from (const result_type& a_key)
	{
		return a_key;
	}

};

/**	@} */

/******************************************************************************/

}}} // namespace anta::dsel::util

#endif /* ANTA_DSEL_UTIL_MAKE_KEY_HPP_ */
