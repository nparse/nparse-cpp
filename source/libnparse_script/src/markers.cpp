/*
 * @file $/source/libnparse_script/src/markers.cpp
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
#include <nparse/nparse.hpp>

namespace nparse {

// @see include/nparse/markers.hpp for the definition

const anta::State<SG>& get_marked_state (const hnd_arg_t& arg)
{
	int level = 0;
	const anta::State<SG>* p;
	for (p =& arg. state; p != NULL; p = p -> get_ancestor())
	{
		switch (p -> get_arc(). get_label(). get_int())
		{
		case +100:
			++ level;
			break;

		case -100:
			-- level;
			break;
		}

		if (level == 0)
			break;
	}

	if (p == NULL)
		throw std::logic_error("unable to find the marked state");

	return *p;
}

} // namespace nparse
