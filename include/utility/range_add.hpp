/*
 * @file $/include/utility/range_add.hpp
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
#ifndef UTILITY_RANGE_ADD_HPP_
#define UTILITY_RANGE_ADD_HPP_

#include <algorithm>

namespace utility {

template <typename Container_>
void range_add (Container_& a_container,
		const typename Container_::value_type& a_from,
		const typename Container_::value_type& a_to)
{
	if (a_to < a_from)
		return range_add(a_container, a_to, a_from);

	typename Container_::iterator left = std::lower_bound(
			a_container. begin(), a_container. end(), a_from);
	if (left == a_container. end())
	{
		a_container. insert(a_container. end(), a_from);
		a_container. insert(a_container. end(), a_to);
		return ;
	}

	typename Container_::iterator right = std::lower_bound(
			a_container. begin(), a_container. end(), a_to);
	if (right == a_container. begin())
	{
		a_container. insert(a_container. begin(), a_to);
		a_container. insert(a_container. begin(), a_from);
		return ;
	}

	if (left == right)
	{
		if ((left - a_container. begin()) & 1)
		{
			// no operation
		}
		else
		{
			a_container. insert(left, a_to);
			a_container. insert(left, a_from);
		}

		return ;
	}

	if ((left - a_container. begin()) & 1)
	{
		if ((right - a_container. begin()) & 1)
		{
			// no operation
		}
		else
		{
			*left = a_to;
			std::advance(left, 1);
		}
	}
	else
	{
		if ((right - a_container. begin()) & 1)
		{
			std::advance(right, -1);
			*right = a_from;
		}
		else
		{
			*left = a_from;
			std::advance(left, 1);
			*left = a_to;
			std::advance(left, 1);
		}
	}

	a_container. erase(left, right);
}

} // namespace utility

#endif /* UTILITY_RANGE_ADD_HPP_ */
