/*
 * @file $/source/libnparse_script/src/script/constructs/switch.cpp
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
#include <stack>
#include <nparse/nparse.hpp>
#include <nparse/util/linked_action.hpp>
#include <anta/dsel/rt/assign.hpp>
#include <anta/dsel/rt/comma.hpp>
#include <anta/dsel/rt/equal_to.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/regex.hpp>
#include "../../static.hpp"
#include "control.hpp"

namespace {

using namespace nparse;

class Action: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		const anta::dsel::constant<NLG>::type condition =
			anta::dsel::ct<NLG>(m_condition. evalVal(a_env));

		cases_t::const_iterator c;
		for (c = m_cases. begin(); c != m_cases. end(); ++ c)
		{
			if	(	!c -> first
				||	(condition == c -> first. evalVal(a_env))
						. fn(a_env)
						. as_boolean()
				)
			{
				break;
			}
		}

		try
		{
			for ( ; c != m_cases. end(); ++ c)
			{
				for (actions_t::const_iterator a = c -> second. begin();
						a != c -> second. end(); ++ a)
				{
					(*a). evalVal(a_env);
				}
			}
		}
		catch (const loop_control_t& lc)
		{
			if (! lc. target(). empty() && lc. target() != m_loop)
				throw;

			if (! lc. exit())
			{
				anta::range<SG>::type where;
				getLocation(where);
				throw ex::runtime_error()
					<< ex::function("switch")
					<< ex::location(where)
					<< ex::message("continue operator is not appropriate");
			}
		}

		return result_type();
	}

public:
	Action (const anta::range<SG>::type& a_range, const std::string& a_loop,
			IStaging& a_staging):
		LinkedAction (a_range), m_loop (a_loop), m_has_default (false)
	{
		m_condition = a_staging. pop();
	}

	bool add_case (const action_pointer& a_value)
	{
		if (m_has_default) return false;
		m_cases. push_back(cases_t::value_type(a_value, actions_t()));
		return true;
	}

	bool add_default ()
	{
		if (m_has_default) return false;
		m_has_default = true;
		m_cases. push_back(cases_t::value_type());
		return true;
	}

	void add_action (const action_pointer& a_action)
	{
		m_cases. back(). second. push_back(a_action);
	}

private:
	std::string m_loop;
	bool m_has_default;

	action_pointer m_condition;

	typedef std::vector<action_pointer> actions_t;
	typedef std::vector<std::pair<action_pointer, actions_t> > cases_t;

	cases_t m_cases;

};

class Construct: public IConstruct
{
//	<LOCAL PARSER DATA>
	std::stack<Action*> m_switch_stack;

//	</LOCAL PARSER DATA>

	bool create_condition (const hnd_arg_t& arg)
	{
		// Determine the name of the loop.
		const anta::string<SG>::type loop =
			arg. state. val("loop"). as_string();
		m_switch_stack. push(new Action(get_marked_range(arg), loop,
					arg. staging));
		return true;
	}

	bool create_case (const hnd_arg_t& arg)
	{
		assert( !m_switch_stack. empty() );
		if (! m_switch_stack. top() -> add_case(arg. staging. pop()))
		{
			throw ex::runtime_error()
				<< ex::function("switch")
				<< ex::location(get_accepted_range(arg))
				<< ex::message("no labels are allowed after the default case");
		}
		return true;
	}

	bool create_default (const hnd_arg_t& arg)
	{
		assert( !m_switch_stack. empty() );
		if (! m_switch_stack. top() -> add_default())
		{
			throw ex::runtime_error()
				<< ex::function("switch")
				<< ex::location(get_accepted_range(arg))
				<< ex::message("redefinition of the default case");
		}
		return true;
	}

	bool create_action (const hnd_arg_t& arg)
	{
		assert( !m_switch_stack. empty() );
		m_switch_stack. top() -> add_action(arg. staging. pop());
		return true;
	}

	bool create_switch (const hnd_arg_t& arg)
	{
		assert( !m_switch_stack. empty() );
		arg. staging. push(action_pointer(m_switch_stack. top()));
		m_switch_stack. pop();
		return true;
	}

	plugin::instance<IConstruct> m_expression, m_statement;
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
		m_expression ("nparse.script.Expression"),
		m_statement ("nparse.script.Statement"),
// <DEBUG_NODE_NAMING>
		entry_		("Switch.Entry")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;
		using namespace anta::dsel;
		using boost::proto::lit;

		anta::Label<SG>
			doCreateCondition = hnd_t(this, &Construct::create_condition),
			doCreateCase = hnd_t(this, &Construct::create_case),
			doCreateDefault = hnd_t(this, &Construct::create_default),
			doCreateAction = hnd_t(this, &Construct::create_action),
			doCreateSwitch = hnd_t(this, &Construct::create_switch);

		reference<SG>::type loop = ref<SG>("loop");

		entry_ =
			(	(+alnum) [ loop = delta(), true ] > space > ':' > space
			|	pass [ loop = "", true ]
			)
		>	regex("\\Aswitch\\s*\\(\\s*") * M0 > m_expression -> entry()
		>	regex("\\A\\s*\\)\\s*\\{\\s*") [ push(loop) ] *M1* doCreateCondition
		>  *(	(	regex("\\Acase(\\s+|(?![_[:alnum:]]))")
						> m_expression -> entry() > space * doCreateCase
				|	regex("\\Adefault\\s*") * doCreateDefault
				)
			>	':'
			>	space % (m_statement -> entry() > pass * doCreateAction)
			)
		>	lit('}') [ pop(loop) ] * doCreateSwitch;
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Construct, construct_switch, nparse.script.constructs.Switch, 1 )
