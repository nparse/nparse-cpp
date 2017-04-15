/*
 * @file $/include/nparse/markers.hpp
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
#ifndef NPARSE_MARKERS_HPP_
#define NPARSE_MARKERS_HPP_

namespace nparse {

/**
 *	A specific arc label that is used to mark arbitrary elements of the script
 *	grammar as source script binding points.
 */
static const boost::proto::terminal<anta::Label<SG> >::type M0 =
	{ anta::Label<SG>(-100) };

/**
 *	A specific arc label that is used to mark arbitrary elements of the script
 *	grammar as points where corresponding source script bindings are
 *	being analyzed.
 */
static const boost::proto::terminal<anta::Label<SG> >::type M1 =
	{ anta::Label<SG>(+100) };

/**
 *	Find a previously marked state with respect to the nesting principle.
 */
const anta::State<SG>& get_marked_state (const hnd_arg_t& arg);

/**
 *	Get the accepted range of a previously marked state.
 */
inline const anta::range<SG>::type& get_marked_range (const hnd_arg_t& arg)
{
	return get_marked_state(arg). get_range();
}

} // namespace nparse

#endif /* NPARSE_MARKERS_HPP_ */
