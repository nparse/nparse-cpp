/*
 * @file $/source/libnparse_runtime/src/operators/arithmetic1.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#include "_priority.hpp"
#include "../static.hpp"

#include <anta/sas/test.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>

#include <anta/dsel/rt/plus.hpp>
#include <anta/dsel/rt/minus.hpp>

namespace {

using namespace boost::proto::tag;
using namespace nparse;
using anta::dsel::ct;

template <typename Func_>
class Action;

template <>
class Action<plus>: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const result_type u = m_left. evalVal(a_env);
		const result_type v = m_right. evalVal(a_env);
		return (ct<NLG>(u) + v). fn(a_env);
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

template <>
class Action<minus>: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const result_type u = m_left. evalVal(a_env);
		const result_type v = m_right. evalVal(a_env);
		return (ct<NLG>(u) - v). fn(a_env);
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

class Operator: public IOperator
{
public:
	// Overridden from IOperator:

	int priority () const
	{
		return PRIORITY_ARITHMETIC1;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		a_current =
			a_previous
		>  *(	space
			>	(	'+' * M0 > space > a_previous > pass * M1 * m_plus
				|	'-' * M0 > space > a_previous > pass * M1 * m_minus
				)
			);
	}

public:
	Operator ()
	{
		m_plus = hnd_t(this, &Operator::push<plus>);
		m_minus = hnd_t(this, &Operator::push<minus>);
	}

private:
	template <typename Func_>
	bool push (const hnd_arg_t& arg) const
	{
		arg. staging. push(new Action<Func_>(get_marked_range(arg),
					arg. staging));
		return true;
	}

	anta::Label<SG> m_plus, m_minus;

};

} // namespace

PLUGIN(Operator, operator_arithmetic1, nparse.script.operators.Arithmetic1)
