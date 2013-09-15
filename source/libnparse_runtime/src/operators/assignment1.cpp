/*
 * @file $/source/libnparse_runtime/src/operators/assignment1.cpp
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
#include "_binary_action.hpp"
#include "_binary_operator.hpp"
#include "_priority.hpp"
#include "../static.hpp"

namespace {

class Action: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// NOTE: this is value derivation, and not just assignment
		// return (m_left. evalRef(a_env) = m_right. evalVal(a_env));

		result_type right = m_right. evalVal(a_env);
		result_type& left = m_left. evalRef(a_env);
		if (right. is_array())
		{
			// All intermediate instances should be stored in the environment,
			// otherwise it will be impossible to store arrays in temporary
			// variables and return them as function results.
			result_type temp = a_env. create(& right. array());
			left. swap(temp);
		}
		else
		{
			left. swap(right);
		}
		return left;
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

class Operator: public BinaryOperator
{
	// Overridden from BinaryOperator:

	IAction* create_action (const anta::range<SG>::type& a_range,
			IStaging& a_staging) const
	{
		return new Action(a_range, a_staging);
	}

public:
	Operator ():
		BinaryOperator (PRIORITY_ASSIGNMENT1, "=")
	{
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Operator, operator_assignment1, nparse.script.operators.Assignment1, 1 )
