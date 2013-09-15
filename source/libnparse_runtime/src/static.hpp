/*
 * @file $/source/libnparse_runtime/src/static.hpp
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
#ifndef SRC_STATIC_HPP_
#define SRC_STATIC_HPP_

#include <plugin/static.hpp>

#define PLUGIN_NAMESPACE	\
	nparse_runtime

#define PLUGIN_LIST			\
	operator_operand,		\
	operator_subscripting,	\
	operator_access,		\
	operator_negation,		\
	operator_inversion,		\
	operator_definition,	\
	operator_prefix,		\
	operator_postfix,		\
	operator_arithmetic1,	\
	operator_arithmetic2,	\
	operator_relational1,	\
	operator_relational2,	\
	operator_logical,		\
	operator_conditional,	\
	operator_assignment1,	\
	operator_assignment2,	\
	operator_assumption,	\
	operator_comma,			\
	function_boolean,		\
	function_integer,		\
	function_real,			\
	function_string,		\
	function_array,			\
	function_self,			\
	function_ref,			\
	function_swap,			\
	function_pass,			\
	function_stop,			\
	function_nth,			\
	function_len

PLUGIN_STATIC_MODULE( PLUGIN_LIST )

#endif /* SRC_STATIC_HPP_ */
