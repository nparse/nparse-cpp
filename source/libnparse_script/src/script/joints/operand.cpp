/*
 * @file $/source/libnparse_script/src/script/joints/operand.cpp
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
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include "../_action_string.hpp"
#include "../_action_range.hpp"
#include "../../static.hpp"
#include "_priority.hpp"
#include "../_punct.hpp"

namespace {

using namespace nparse;

class MetaAcceptor: public IAcceptor, public anta::Acceptor<NLG>,
	private ActionString
{
public:
	MetaAcceptor (const anta::range<SG>::type& a_range, IStaging& a_staging):
		ActionString (a_range), m_staging (a_staging)
	{
	}

public:
	// Overridden from IAcceptor:
	const anta::Acceptor<NLG>& get () const
	{
		return *this;
	}

	// Overridden from Acceptor<NLG>:
	void accept (const anta::range<NLG>::type& C,
			const anta::range<NLG>::type& E, anta::spectrum<NLG>::type& S) const
	{
		m_staging. acceptor(eval(S)). accept(C, E, S);
	}

private:
	IStaging& m_staging;

};

class InlineArrayAction: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		anta::ndl::Context<NLG>* target0 = const_cast<anta::ndl::Context<NLG>*>(
				&* (*m_statement). evalVal(a_env). as_array());

		anta::ndl::Context<NLG>* self = a_env. self(false);
		if (self != NULL)
		{
			const anta::ndl::Context<NLG>* target = target0;
			while (target -> get_ancestor() != NULL)
				target = target -> get_ancestor();
			const_cast<anta::ndl::Context<NLG>*>(target) -> set_ancestor(self);
		}

		a_env. get_traveller(). get_state(). substitute(target0);

		return result_type(true);
	}

public:
	InlineArrayAction (action_pointer a_statement):
		m_statement (a_statement)
	{
	}

private:
	action_pointer m_statement;

};

class Operator: public IOperator
{
	bool jump (const hnd_arg_t& arg)
	{
		// Instantiate acceptor.
		const anta::Acceptor<NLG>* acceptor;
		if (*arg. state. get_range(). first == L'"')
		{
			acceptor =& arg. staging. acceptor(new MetaAcceptor(
				get_accepted_range(arg), arg. staging));
		}
		else
		{
			// Get the definition of the acceptor.
			const string_t def = Tokenizer::decode(get_accepted_range(arg));
			acceptor =& arg. staging. acceptor(def);
		}
		// Define transition label.
		static const anta::Label<NLG> label(true);
		// Create a new joint of appropriate type (jump).
		IStaging::joint_pointer joint( *acceptor, label );
		// Push the new joint into the LL stack.
		joint -> set_flavor(0, 1);
		joint -> set_flavor(1, 1);
		arg. staging. pushJoint(joint);
		return true;
	}

	bool call1 (const hnd_arg_t& arg)
	{
		// Get the name of the callee subnetwork entry point.
		const string_t name = Tokenizer::decode(get_accepted_range(arg));
		// Create a new joint of appropriate type (call).
		IStaging::joint_pointer joint( arg. staging. cluster(name) );
		// Push the new joint into the LL stack.
		joint -> set_flavor(0, 1);
		joint -> set_flavor(1, 1);
		arg. staging. pushJoint(joint);
		return true;
	}

	bool call2 (const hnd_arg_t& arg)
	{
		// Get the name of the callee subnetwork entry point.
		const string_t name = Tokenizer::decode(get_accepted_range(arg));
		// Create assignment semantic action.
		action_pointer action(new ActionRange(name));
		// Create a new joint of appropriate compound type.
		IStaging::joint_pointer joint(
			anta::ndl::JointCall<NLG>(arg. staging. cluster(name))
				[ action ]
		);
		// Push the new joint into the LL stack.
		joint -> set_flavor(0, 1);
		joint -> set_flavor(1, 1);
		arg. staging. pushJoint(joint);
		return true;
	}

	bool group (const hnd_arg_t& arg)
	{
		IStaging::joint_pointer joint0 = arg. staging. popJoint();
		// Create a new joint of appropriate type (grouping).
		IStaging::joint_pointer joint( - *joint0 );
		// Push the new joint into the LL stack.
		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	bool inline_block (const hnd_arg_t& arg)
	{
		// Create a new joint of appropriate type (unconditional repeat with
		// attached semantic action).
		IStaging::joint_pointer joint(
			anta::ndl::JointJump<NLG>(
				anta::unconditional<NLG>(), anta::Label<NLG>() )
			[ arg. staging. pop() ]
		);
		// Push the new joint into the LL stack.
	//	joint -> set_flavor(0, 0);
		joint -> set_flavor(1, 1);
		arg. staging. pushJoint(joint);
		return true;
	}

	bool inline_array (const hnd_arg_t& arg)
	{
		// Create a new joint of appropriate type (unconditional repeat with
		// attached semantic action).
		IStaging::joint_pointer joint(
			anta::ndl::JointJump<NLG>(
				anta::unconditional<NLG>(), anta::Label<NLG>() )
			[ new InlineArrayAction(arg. staging. pop()) ]
		);
		// Push the new joint into the LL stack.
	//	joint -> set_flavor(0, 0);
		joint -> set_flavor(1, 1);
		arg. staging. pushJoint(joint);
		return true;
	}

	bool separate (const hnd_arg_t& arg)
	{
		IStaging::joint_pointer joint(
			anta::unconditional<NLG>(), anta::Label<NLG>()
		);
		joint -> set_virtual(true);
		joint -> set_flavor(0, 2);
		joint -> set_flavor(1, 2);
		arg. staging. pushJoint(joint);
		return true;
	}

	bool concatenate (const hnd_arg_t& arg)
	{
		IStaging::joint_pointer joint(
			anta::unconditional<NLG>(), anta::Label<NLG>()
		);
		joint -> set_virtual(true);
	//	joint -> set_flavor(0, 0);
	//	joint -> set_flavor(1, 0);
		arg. staging. pushJoint(joint);
		return true;
	}

	plugin::instance<IConstruct> m_block, m_array;
	anta::Label<SG> m_jump, m_call1, m_call2, m_group, m_inline_block,
		m_inline_array, m_separate, m_concatenate;

public:
	Operator ():
		m_block ("nparse.script.constructs.Block"),
		m_array ("nparse.script.$.Array")
	{
		m_jump = hnd_t(this, &Operator::jump),
		m_call1 = hnd_t(this, &Operator::call1),
		m_call2 = hnd_t(this, &Operator::call2),
		m_group = hnd_t(this, &Operator::group);
		m_inline_block = hnd_t(this, &Operator::inline_block);
		m_inline_array = hnd_t(this, &Operator::inline_array);
		m_separate = hnd_t(this, &Operator::separate);
		m_concatenate = hnd_t(this, &Operator::concatenate);
	}

public:
	int priority () const
	{
		return PRIORITY_OPERAND;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		a_current =
			(	token<Separator>(PUNCT, false)
			|  -('^' > token<Separator>(";", false))
			) > pass * m_jump

		|	'$' >
			(	token<Separator>(PUNCT, true) * m_call1
			|	space > ':' > token<Separator>(PUNCT, true) * m_call2
			)

		|	'(' > space > a_top > space > lit(')') * m_group

		|	m_block -> entry() > pass * m_inline_block
		|	m_array -> entry() > pass * m_inline_array

		|	lit('_') * m_separate
		|	lit('#') * m_concatenate;
	}

};

} // namespace

PLUGIN(Operator, joint_operand, nparse.script.joints.Operand)
