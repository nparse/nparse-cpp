/*
 * @file $/source/libnparse_runtime/src/operators/assignment.cpp
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
#include "_binary_action.hpp"
#include <anta/sas/regex.hpp>
#include <anta/dsel/rt/equal_to.hpp>
#include <anta/dsel/rt/not_equal_to.hpp>
#include <anta/dsel/rt/plus.hpp>
#include <anta/dsel/rt/minus.hpp>
#include <anta/dsel/rt/multiplies.hpp>
#include <anta/dsel/rt/divides.hpp>
#include <anta/dsel/rt/modulus.hpp>
#include "_priority.hpp"
#include "../static.hpp"

namespace {

using namespace boost::proto::tag;
using namespace nparse;
using anta::dsel::ct;

struct just_assign {}; // tag for simple assignment operator `='
struct test_assign {}; // tag for conditional assignment operator `?='

template <typename Func_> class Action;

template <>
class Action<just_assign>: public BinaryAction
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
			result_type temp = a_env. create(&* right. as_array());
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

template <>
class Action<test_assign>: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		using anta::dsel::ct;

		const result_type left = m_left. evalVal(a_env);
		const result_type right = m_right. evalVal(a_env);

		if (left. is_null())
		{
			m_left. evalRef(a_env) = right;
			return result_type(false);
		}
		else
		if ((ct<NLG>(left) != right). fn(a_env). as_boolean())
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
		BinaryAction (a_range, a_staging)
	{
	}

};

template <>
class Action<plus>: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const result_type v = m_right. evalVal(a_env);
		result_type& u = m_left. evalRef(a_env);
		u = (ct<NLG>(u) + v). fn(a_env);
		return u;
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
		const result_type v = m_right. evalVal(a_env);
		result_type& u = m_left. evalRef(a_env);
		u = (ct<NLG>(u) - v). fn(a_env);
		return u;
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

template <>
class Action<multiplies>: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const result_type v = m_right. evalVal(a_env);
		result_type& u = m_left. evalRef(a_env);
		u = (ct<NLG>(u) * v). fn(a_env);
		return u;
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

template <>
class Action<divides>: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const result_type v = m_right. evalVal(a_env);
		result_type& u = m_left. evalRef(a_env);
		u = (ct<NLG>(u) / v). fn(a_env);
		return u;
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

template <>
class Action<modulus>: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const result_type v = m_right. evalVal(a_env);
		result_type& u = m_left. evalRef(a_env);
		u = (ct<NLG>(u) % v). fn(a_env);
		return u;
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

class Operator: public IOperator
{
	template <typename Func_>
	bool push (const hnd_arg_t& arg) const
	{
		arg. staging. push(new Action<Func_>(get_marked_range(arg),
					arg. staging));
		return true;
	}

	anta::Label<SG> m_just_assign, m_test_assign, m_plus, m_minus, m_multiplies,
		m_divides, m_modulus;

public:
	Operator ()
	{
		m_just_assign = hnd_t(this, &Operator::push<just_assign>);
		m_test_assign = hnd_t(this, &Operator::push<test_assign>);
		m_plus = hnd_t(this, &Operator::push<plus>);
		m_minus = hnd_t(this, &Operator::push<minus>);
		m_multiplies = hnd_t(this, &Operator::push<multiplies>);
		m_divides = hnd_t(this, &Operator::push<divides>);
		m_modulus = hnd_t(this, &Operator::push<modulus>);
	}

public:
	// Overridden from IOperator:

	int priority () const
	{
		return PRIORITY_ASSIGNMENT;
	}

	void deploy (level_t a_current, level_t a_previous) const
	{
		using namespace anta::ndl::terminals;
		a_current =
			a_previous
		>  ~(	re("\\s*=\\s*")    * M0 > a_current > pass * M1 * m_just_assign
			|	re("\\s*\\?=\\s*") * M0 > a_current > pass * M1 * m_test_assign
			|	re("\\s*\\+=\\s*") * M0 > a_current > pass * M1 * m_plus
			|	re("\\s*-=\\s*")   * M0 > a_current > pass * M1 * m_minus
			|	re("\\s*\\*=\\s*") * M0 > a_current > pass * M1 * m_multiplies
			|	re("\\s*/=\\s*")   * M0 > a_current > pass * M1 * m_divides
			|	re("\\s*%=\\s*")   * M0 > a_current > pass * M1 * m_modulus
			);
	}

};

} // namespace

PLUGIN(Operator, operator_assignment, nparse.script.operators.Assignment)
