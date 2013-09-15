/*
 * @file $/source/libnparse_runtime/src/operators/definition.cpp
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
#include <nparse/nparse.hpp>
#include <nparse/util/linked_action.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>

#include "_priority.hpp"
#include "../static.hpp"

namespace {

using namespace nparse;
using anta::dsel::ct;

class Action: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const result_type flag = m_operand. evalVal(a_env);

		if (flag. is_null())
		{
			m_operand. evalRef(a_env) = true;
			return result_type(false);
		}
		else
		if (! flag. as_boolean())
		{
			throw flow_control(false);
		}
		else
		{
			return result_type(true);
		}
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		LinkedAction (a_range)
	{
		m_operand = a_staging. pop();
	}

private:
	action_pointer m_operand;

};

class Operator: public IOperator
{
public:
	// Overridden from IOperator:

	int priority () const
	{
		return PRIORITY_DEFINITION;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		a_current =
			a_previous
		|	'#' * M0 > space > a_current > pass * M1 * m_define;
	}

public:
	Operator ()
	{
		m_define = hnd_t(this, &Operator::push);
	}

private:
	bool push (const hnd_arg_t& arg) const
	{
		arg. staging. push(action_pointer(new Action(
				get_marked_range(arg), arg. staging
		)));
		return true;
	}

	anta::Label<SG> m_define;

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Operator, operator_definition, nparse.script.operators.Definition, 1 )
