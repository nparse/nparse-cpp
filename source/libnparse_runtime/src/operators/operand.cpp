/*
 * @file $/source/libnparse_runtime/src/operators/operand.cpp
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
#include <plugin/category.hpp>
#include <nparse/nparse.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include "../static.hpp"
#include "_priority.hpp"

namespace {

using namespace nparse;

class Operator: public IOperator
{
	plugin::category<IConstruct> m_terms;

public:
	Operator ():
		m_terms ("nparse.script.terms")
	{
	}

public:
	int priority () const
	{
		return PRIORITY_OPERAND;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;

		// Create parenthesis operator.
		a_current = '(' > space > a_top > space > ')';

		// Link terms (using supplement).
		for (plugin::category<IConstruct>::const_iterator i = m_terms. begin();
				i != m_terms. end(); ++ i)
		{
			a_current. cluster()(i -> second -> entry(). cluster());
		}
	}

};

} // namespace

PLUGIN(Operator, operator_operand, nparse.script.operators.Operand)
