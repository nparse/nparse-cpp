/*
 * @file $/source/libnparse_runtime/src/operators/_priority.hpp
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
#ifndef SRC_PRIORITY_HPP_
#define SRC_PRIORITY_HPP_

enum operator_priority
{
	PRIORITY_OPERAND 		= 0,
	PRIORITY_ACCESS			= 100,	// X.Y
	PRIORITY_SUBSCRIPTING	= 200,	// X[Y]
	PRIORITY_DEFINITION		= 300,	// #X
	PRIORITY_PREFIX			= 600,	// ++ X
	PRIORITY_POSTFIX		= 700,	// X ++
	PRIORITY_INVERSION		= 400,	// -X
	PRIORITY_NEGATION		= 500,	// !X
	PRIORITY_ARITHMETIC2	= 800,	// X * Y, X / Y, X % Y
	PRIORITY_ARITHMETIC1	= 900,	// X + Y, X - Y
	PRIORITY_RELATIONAL2	= 1000,	// X < Y, X > Y, X <= Y, X >= Y
	PRIORITY_RELATIONAL1	= 1100,	// X == Y, X != Y
	PRIORITY_LOGICAL		= 1200,	// X & Y, X | Y, X ^ Y, X ~ Y
	PRIORITY_CONDITIONAL	= 1300,	// X ? Y : Z
	PRIORITY_ASSIGNMENT2	= 1400,	// X *= Y
	PRIORITY_ASSIGNMENT1	= 1500, // X = Y
	PRIORITY_ASSUMPTION		= 1600, // X ?= Y
	PRIORITY_COMMA			= 9999	// X, Y
};

#endif /* SRC_PRIORITY_HPP_ */
