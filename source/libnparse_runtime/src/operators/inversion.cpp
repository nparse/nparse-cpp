/*
 * @file $/source/libnparse_runtime/src/operators/inversion.cpp
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
#include <nparse/util/linked_action.hpp>
#include <anta/sas/regex.hpp>
#include <anta/dsel/rt/logical_not.hpp>
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
		const result_type u = m_operand. evalVal(a_env);
		return ( !ct<NLG>(u) ). fn(a_env);
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
	bool push (const hnd_arg_t& arg) const
	{
		arg. staging. push(new Action(get_marked_range(arg), arg. staging));
		return true;
	}

	anta::Label<SG> m_invert;

public:
	Operator ()
	{
		m_invert = hnd_t(this, &Operator::push);
	}

public:
	// Overridden from IOperator:

	int priority () const
	{
		return PRIORITY_INVERSION;
	}

	void deploy (level_t a_current, level_t a_previous) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		|	re("!\\s*") * M0 > a_current > pass * M1 * m_invert;
	}

};

} // namespace

PLUGIN(Operator, operator_inversion, nparse.script.operators.Inversion)
