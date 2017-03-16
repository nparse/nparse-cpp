/*
 * @file $/source/libnparse_runtime/src/operators/postfix.cpp
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
#include <nparse/nparse.hpp>
#include <nparse/util/linked_action.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/dsel/rt/post_inc.hpp>
#include <anta/dsel/rt/post_dec.hpp>

#include "_priority.hpp"
#include "../static.hpp"

namespace {

using namespace nparse;
using namespace boost::proto::tag;
using anta::dsel::val;

template <typename Func_>
class Action;

template <>
class Action<post_inc>: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		result_type& u = m_operand. evalRef(a_env);
		return ( val<NLG>(u) ++ ). fn(a_env);
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

template <>
class Action<post_dec>: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		result_type& u = m_operand. evalRef(a_env);
		return ( val<NLG>(u) -- ). fn(a_env);
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
		return PRIORITY_POSTFIX;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		>  *(	space
			>	(	"++" * M0 > pass * M1 * m_post_inc
				|	"--" * M0 > pass * M1 * m_post_dec
				)
			);
	}

public:
	Operator ()
	{
		m_post_inc = hnd_t(this, &Operator::push<post_inc>);
		m_post_dec = hnd_t(this, &Operator::push<post_dec>);
	}

private:
	template <typename Func_>
	bool push (const hnd_arg_t& arg) const
	{
		arg. staging. push(new Action<Func_>(get_marked_range(arg),
					arg. staging));
		return true;
	}

	anta::Label<SG> m_post_inc, m_post_dec;

};

} // namespace

PLUGIN(Operator, operator_postfix, nparse.script.operators.Postfix)
