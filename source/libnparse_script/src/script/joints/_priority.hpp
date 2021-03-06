/*
 * @file $/source/libnparse_script/src/script/joints/_priority.hpp
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
#ifndef SRC_PRIORITY_HPP_
#define SRC_PRIORITY_HPP_

enum operator_priority
{
	PRIORITY_OPERAND 		= 0,		// acceptor, $Node, $:Node, (X), _, #
	PRIORITY_FUNCTOR		= 100,		// <res:X {pre} post>, [ X ]
	PRIORITY_ALTERNATION_LC	= 200,		// X | Y
	PRIORITY_PREFIX			= 300,		// & X, ! X
	PRIORITY_POSTFIX		= 400,		// X +, X *, X ?, X :y, X *123
	PRIORITY_COMPOSITION	= 500,		// X ^ Y, X / Y, X % Y
	PRIORITY_SEQUENCING		= 600,		// X Y
	PRIORITY_CONDITIONAL	= 700,		// X <? Y, X ?> Y
	PRIORITY_ALTERNATION_GL	= 800,		// X || Y
	PRIORITY_PRIORITIZING	= 900		// X |? Y
};

#endif /* SRC_PRIORITY_HPP_ */
