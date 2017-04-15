/*
 * @file $/source/libnparse_script/src/script/constructs/return.cpp
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
#include <nparse/nparse.hpp>
#include <anta/sas/regex.hpp>
#include "return.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

class Action: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		throw return_control(
				m_expression
			?	m_expression -> evalVal(a_env)
			:	result_type()
			);
	}

public:
	Action (action_pointer a_expression):
		m_expression (a_expression)
	{
	}

	Action ():
		m_expression ()
	{
	}

private:
	action_pointer m_expression;

};

class Construct: public IConstruct
{
	bool return_expression (const hnd_arg_t& arg)
	{
		action_pointer expression = arg. staging. pop();
		arg. staging. push(new Action(expression));
		return true;
	}

	bool return_nothing (const hnd_arg_t& arg)
	{
		arg. staging. push(new Action());
		return true;
	}

	plugin::instance<IConstruct> m_expression;
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
		m_expression ("nparse.script.Expression"),
// <DEBUG_NODE_NAMING>
		entry_		("Return.Entry")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;

		anta::Label<SG>
			doReturnExpression = hnd_t(this, &Construct::return_expression),
			doReturnNothing = hnd_t(this, &Construct::return_nothing);

		entry_ =
			re("return\\>\\s*")
		>	(	m_expression -> entry() > pass * doReturnExpression
			|	pass * doReturnNothing
			)
		>	re("\\s*;");
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, construct_return, nparse.script.constructs.Return)
