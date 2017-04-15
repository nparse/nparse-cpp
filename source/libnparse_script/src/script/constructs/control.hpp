/*
 * @file $/source/libnparse_script/src/script/constructs/control.hpp
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
#ifndef SRC_LOOP_CONTROL_HPP_
#define SRC_LOOP_CONTROL_HPP_

#include <string>

class loop_control: public nparse::flow_control
{
public:
	loop_control (const bool a_exit = true, const std::string& a_target =
			std::string()):
		flow_control (! a_exit), m_target (a_target)
	{
	}

	const std::string& target () const
	{
		return m_target;
	}

private:
	std::string m_target;

};

#endif /* SRC_LOOP_CONTROL_HPP_ */
