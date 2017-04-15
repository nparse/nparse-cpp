/*
 * @file $/include/nparse/util/linked_action.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef NPARSE_UTIL_LINKED_ACTION_HPP_
#define NPARSE_UTIL_LINKED_ACTION_HPP_

class LinkedAction: public nparse::IAction
{
public:
	// Overridden from IAction:

	bool getLocation (anta::range<nparse::SG>::type& a_range) const
	{
		a_range = m_range;
		return true;
	}

public:
	LinkedAction (const anta::range<nparse::SG>::type& a_range):
		m_range (a_range)
	{
	}

private:
	anta::range<nparse::SG>::type m_range;

};

#endif /* NPARSE_UTIL_LINKED_ACTION_HPP_ */
