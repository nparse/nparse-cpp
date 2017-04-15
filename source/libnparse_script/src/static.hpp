/*
 * @file $/source/libnparse_script/src/static.hpp
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
#ifndef SRC_STATIC_HPP_
#define SRC_STATIC_HPP_

#include <plugin/static.hpp>

#define PLUGIN_NAMESPACE	\
	nparse_script_grammar

#define PLUGIN_LIST			\
	script_staging,			\
	script_grammar,			\
	script_comment,			\
	script_expression,		\
	script_statement,		\
	script_footprint,		\
	decl_import,			\
	decl_namespace,			\
	decl_node,				\
	decl_rule,				\
	decl_function,			\
	term_operand,			\
	term_variable,			\
	term_placeholder,		\
	term_call,				\
	construct_block,		\
	construct_marking,		\
	construct_ifelse,		\
	construct_while,		\
	construct_dowhile,		\
	construct_for,			\
	construct_foreach,		\
	construct_switch,		\
	construct_control,		\
	construct_return,		\
	joint_operand,			\
	joint_functor,			\
	joint_prefix,			\
	joint_postfix,			\
	joint_alternation_lc,	\
	joint_composition,		\
	joint_conditional_left,	\
	joint_conditional_right,\
	joint_sequencing,		\
	joint_alternation_gl,	\
	joint_prioritizing

PLUGIN_STATIC_MODULE( PLUGIN_LIST )

#endif /* SRC_STATIC_HPP_ */
