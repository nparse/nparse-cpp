/*
 * @file $/source/libnparse_script/src/script/joints/functor.cpp
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
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include "../_varname.hpp"
#include "../../tokenizer.hpp"
#include "../../static.hpp"
#include "_priority.hpp"
#include "../_punct.hpp"

namespace {

using namespace nparse;

// The external trace context is referred by a special zero length key which
// shouldn't be used explicitly in semantic actions.
static const IEnvironment::key_type zero = IEnvironment::key_type();

// ext = self();
// push ext;
class PreCond: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// Evaluate initializer expression.
		result_type init;
		if (m_init)
		{
			m_init. evalVal(a_env). swap(init);
		}
		else
		{
			result_type(a_env. create()). swap(init);
		}

		// Get traveller reference.
		anta::Traveller<NLG>& traveller = a_env. get_traveller();

		// Get current context without doing any instantiation.
		anta::ndl::Context<NLG>* self = traveller. get_state(). context(NULL);

		// If the initializer is an array then do context forwarding,
		// i.e. substitute initializer array for the current context.
		if (init. is_array())
		{
			traveller. get_state(). substitute(& init. array());
		}
		else

		// Otherwise, check if the initializer can be reduced to the positive
		// logical value.
		if (! init. as_boolean())
		{
			throw flow_control(false);
		}

		// Save previous context in a special variable representing the external
		// context.
		traveller. ref(zero, true) = a_env. create(self);

		// Mark the special variable as pushed.
		traveller. get_state(). context(&traveller) -> push(zero);

		// Return nothing.
		return result_type();
	}

public:
	PreCond (const action_pointer& a_init = action_pointer()):
		m_init (a_init)
	{
	}

private:
	action_pointer m_init;

};

// pop ext;
// res = self(ext);
class PostCond: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// First, evaluate and check the post-condition.
		if (m_cond && ! m_cond. evalVal(a_env). as_boolean())
		{
			throw flow_control(false);
		}

		// Get traveller reference.
		anta::Traveller<NLG>& traveller = a_env. get_traveller();

		// Get current context, instantiate if it's necessary.
		anta::ndl::Context<NLG>* self =
			traveller. get_state(). context(&traveller);

		// Pop the special variable that holds the external context.
		self -> pop(zero);

		// Get the value of the special variable (supposed to be array).
		result_type val = self -> val(zero);
		assert( val. is_array() );

		// Substitute restored external context for the current.
		traveller. get_state(). substitute(& val. array());

		// Save previous context as the result in another special variable.
		if (! m_alias. empty())
		{
			const IEnvironment::key_type key(m_alias);
			traveller. ref(key, true) =
				anta::aux::array<NLG>::type(a_env. create(self));
		}

		// Return nothing.
		return result_type();
	}

public:
	PostCond (const string_t& a_alias = string_t(),
			const action_pointer& a_cond = action_pointer()):
		m_alias (a_alias), m_cond (a_cond)
	{
	}

private:
	string_t m_alias;
	action_pointer m_cond;

};

class Operator: public IOperator
{
//	<LOCAL PARSER DATA>
	string_t m_alias, m_target;
	action_pointer m_pre_cond, m_post_cond;

//	</LOCAL PARSER DATA>

	// Defines alias.
	bool set_alias (const hnd_arg_t& arg)
	{
		m_alias = get_accepted_str(arg);
		return true;
	}

	// Defines no-alias mode.
	bool no_alias (const hnd_arg_t& arg)
	{
		m_alias. clear();
		return true;
	}

	// Defines auto-alias mode.
	bool auto_alias (const hnd_arg_t& arg)
	{
		m_alias = "*";
		return true;
	}

	// Defines target (the callee node).
	bool set_target (const hnd_arg_t& arg)
	{
		m_target = Tokenizer::decode(get_accepted_range(arg));

		if (m_alias. size() == 1 && static_cast<char>(m_alias[0]) == '*')
			m_alias = m_target;

		return true;
	}

	// Defines pre-condition that is evaluated on entry.
	bool set_pre_cond (const hnd_arg_t& arg)
	{
		m_pre_cond. reset(new PreCond(arg. staging. pop()));
		return true;
	}

	// Defines default (empty) pre-condition.
	bool no_pre_cond (const hnd_arg_t& arg)
	{
		m_pre_cond. reset(new PreCond());
		return true;
	}

	// Defines post-condition that is evaluated on exit.
	bool set_post_cond (const hnd_arg_t& arg)
	{
		m_post_cond. reset(new PostCond(m_alias, arg. staging. pop()));
		return true;
	}

	// Defines default (empty) post-condition.
	bool no_post_cond (const hnd_arg_t& arg)
	{
		m_post_cond. reset(new PostCond(m_alias));
		return true;
	}

	// Creates joint representing external syntactic functor.
	bool create_functor1 (const hnd_arg_t& arg)
	{
		// Create a new joint of appropriate compound type.
		IStaging::joint_pointer joint(
			anta::ndl::JointCall<NLG>(arg. staging. cluster(m_target))
				[ m_post_cond ] & m_pre_cond
		);
		// Push the new joint into the LL stack.
		joint -> set_flavor(0, 1);
		joint -> set_flavor(1, 1);
		arg. staging. pushJoint(joint);
		// Reset condition pointers.
		m_pre_cond. reset();
		m_post_cond. reset();
		return true;
	}

	// Creates joint representing local syntactic functor.
	bool create_functor2 (const hnd_arg_t& arg)
	{
		// Instantiate trivial pre and post contitional actions.
		auto_alias(arg);
		no_pre_cond(arg);
		no_post_cond(arg);
		// Create a new joint of appropriate compound type.
		IStaging::joint_pointer joint(
			(*arg. staging. popJoint()) [ m_post_cond ] & m_pre_cond
		);
		// Push the new joint into the LL stack.
		joint -> set_flavor(0, 1);
		joint -> set_flavor(1, 1);
		arg. staging. pushJoint(joint);
		// Reset condition pointers.
		m_pre_cond. reset();
		m_post_cond. reset();
		return true;
	}

	plugin::instance<IConstruct> m_expression;
	anta::Label<SG> m_set_alias, m_no_alias, m_auto_alias, m_set_target,
		m_set_pre_cond, m_no_pre_cond, m_set_post_cond, m_no_post_cond,
		m_create_functor1, m_create_functor2;

public:
	Operator ():
		m_expression ("nparse.script.Expression")
	{
		m_set_alias = hnd_t(this, &Operator::set_alias);
		m_no_alias = hnd_t(this, &Operator::no_alias);
		m_auto_alias = hnd_t(this, &Operator::auto_alias);
		m_set_target = hnd_t(this, &Operator::set_target);
		m_set_pre_cond = hnd_t(this, &Operator::set_pre_cond);
		m_no_pre_cond = hnd_t(this, &Operator::no_pre_cond);
		m_set_post_cond = hnd_t(this, &Operator::set_post_cond);
		m_no_post_cond = hnd_t(this, &Operator::no_post_cond);
		m_create_functor1 = hnd_t(this, &Operator::create_functor1);
		m_create_functor2 = hnd_t(this, &Operator::create_functor2);
	}

public:
	int priority () const
	{
		return PRIORITY_FUNCTOR;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		// External functor: < res : Target {pre-cond} post-cond >
		|	(	'<'
			// Optional variable name to store the result context.
			>	(	space > (varName * m_set_alias > space | pass * m_no_alias)
				>	':'
				|	pass * m_auto_alias
				)
			// Target node name.
			>	token<Separator>(PUNCT, true) * m_set_target
			// Optional pre and post-conditions.
			>	(	space > m_expression -> entry() > pass * m_set_pre_cond
				>	(	space > m_expression -> entry() > pass * m_set_post_cond
					|	pass * m_no_post_cond
					)
				|	pass * m_no_pre_cond > pass * m_no_post_cond
				)
			>	space * m_create_functor1 > '>'
			)
		// Inlined (screening) functor: [ a b c ]
		|	'[' > space > a_top > space * m_create_functor2 > ']';
	}

};

} // namespace

PLUGIN_STATIC_EXPORT(
		Operator, joint_functor, nparse.script.joints.Functor, 1 )
