/*
 * @file $/source/libnparse_runtime/src/operators/access.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@nparse.com>
 
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
#include <anta/sas/regex.hpp>
#include "_priority.hpp"
#include "../static.hpp"

namespace {

using namespace nparse;

class Action: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// Evaluate object value.
		result_type u = m_operand. evalVal(a_env);

		// If the object is an array then the field represents its key.
		if (u. is_array())
		{
			return u. as_array() -> val(m_field);
		}

		// Otherwise, access operator doesn't make much sence.
		else
		{
			throw ex::runtime_error()
				<< ex::function("dot/read")
				<< ex::message("cannot be used with this type");
		}
	}

	result_type& evalRef (IEnvironment& a_env) const
	{
		// Evaluate object reference.
		result_type& u = m_operand. evalRef(a_env);

		// If the object is an array then the field represents its key.
		if (u. is_array())
		{
			return u. as_array() -> ref(m_field);
		}

		// Otherwise, access operator doesn't make much sence.
		else
		{
			throw ex::runtime_error()
				<< ex::function("dot/write")
				<< ex::message("cannot be used with this type");
		}
	}

private:
	const IEnvironment::key_type m_field;
	action_pointer m_operand;

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging,
			const string_t a_field):
		LinkedAction (a_range), m_field (a_field)
	{
		m_operand = a_staging. pop();
	}

};

class Operator: public IOperator
{
	bool create_action (const hnd_arg_t& arg) const
	{
		arg. staging. push(new Action(get_marked_range(arg), arg. staging,
					get_accepted_str(arg)));
		return true;
	}

	anta::Label<SG> m_action;

public:
	Operator ()
	{
		m_action = hnd_t(this, &Operator::create_action);
	}

public:
	// Overridden from IOperator:

	int priority () const
	{
		return PRIORITY_ACCESS;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		a_current = a_previous > *(space > '.' * M0 > space
				> regex("\\A(?!\\d)[_\\w]+") * M1 * m_action);
	}

};

} // namespace

PLUGIN(Operator, operator_access, nparse.script.operators.Access)
