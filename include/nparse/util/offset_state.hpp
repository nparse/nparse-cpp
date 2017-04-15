/*
 * @file $/include/nparse/util/offset_state.hpp
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
#ifndef NPARSE_UTIL_OFFSET_STATE_HPP_
#define NPARSE_UTIL_OFFSET_STATE_HPP_

/**
 *	A helper template function that returns an actual state located at a
 *	specified relative offset from the given state.
 */
template <typename M_>
const anta::State<M_>* offset_state (const anta::State<M_>* s,
		const int a_offset = 0)
{
	for (int offset = a_offset + 1; offset; -- offset)
	{
		do
		{
			s = s -> get_ancestor();
			if (!s)
				return NULL;
		} while (! s -> get_arc(). get_label(). is_actual());
	}
	return s;
}

#endif /* NPARSE_UTIL_OFFSET_STATE_HPP_ */
