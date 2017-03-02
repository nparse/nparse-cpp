/*
 * @file $/contrib/include/utility/free.hpp
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
#ifndef UTILITY_FREE_HPP_
#define UTILITY_FREE_HPP_

#include <algorithm>
#include <functional>

namespace utility {

template <typename VictimPtr_>
class free: public std::unary_function<VictimPtr_, void>
{
public:
	void operator () (VictimPtr_ p) const
	{
		delete p;
	}

};

template <typename Pair_>
class free1st: public std::unary_function<Pair_, void>
{
public:
	void operator () (Pair_& p) const
	{
		delete p. first;
	}

};

template <typename Pair_>
class free2nd: public std::unary_function<Pair_, void>
{
public:
	void operator () (Pair_& p) const
	{
		delete p. second;
	}

};

template <typename Container_>
void free_all (Container_& c)
{
	if (c. empty())
		return;
	std::for_each(c. begin(), c. end(),
			free<typename Container_::value_type>());
	c. clear();
}

template <typename Container_>
void free1sts (Container_& c)
{
	if (c. empty())
		return;
	std::for_each(c. begin(), c. end(),
			free1st<typename Container_::value_type>());
	c. clear();
}

template <typename Container_>
void free2nds (Container_& c)
{
	if (c. empty())
		return;
	std::for_each(c. begin(), c. end(),
			free2nd<typename Container_::value_type>());
	c. clear();
}

} // namespace utility

#endif /* UTILITY_FREE_HPP_ */
