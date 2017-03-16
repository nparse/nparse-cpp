/*
 * @file $/source/libnparse_script/src/script/constructs/ifelse.cpp
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
#include <anta/sas/test.hpp>
#include <anta/sas/regex.hpp>
#include "../../static.hpp"

namespace {

using namespace nparse;

// Short variant: no false branch.
class Action1: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		if (m_condition. evalVal(a_env). as_boolean())
			return m_true_branch. evalVal(a_env);
		else
			return result_type(true);
	}

public:
	Action1 (IStaging& a_staging)
	{
		m_true_branch = a_staging. pop();
		m_condition = a_staging. pop();
	}

private:
	action_pointer m_condition, m_true_branch;

};

// Full variant: both true and false branches present.
class Action2: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		if (m_condition. evalVal(a_env). as_boolean())
			return m_true_branch. evalVal(a_env);
		else
			return m_false_branch. evalVal(a_env);
	}

public:
	Action2 (IStaging& a_staging)
	{
		m_false_branch = a_staging. pop();
		m_true_branch = a_staging. pop();
		m_condition = a_staging. pop();
	}

private:
	action_pointer m_condition, m_true_branch, m_false_branch;

};

class Construct: public IConstruct
{
	bool create_action1 (const hnd_arg_t& arg)
	{
		arg. staging. push(new Action1(arg. staging));
		return true;
	}

	bool create_action2 (const hnd_arg_t& arg)
	{
		arg. staging. push(new Action2(arg. staging));
		return true;
	}

	plugin::instance<IConstruct> m_expression, m_statement;
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
		m_expression ("nparse.script.Expression"),
		m_statement ("nparse.script.Statement"),
// <DEBUG_NODE_NAMING>
		entry_		("Block.Entry")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;

		anta::Label<SG>
			doCreateAction1 = hnd_t(this, &Construct::create_action1),
			doCreateAction2 = hnd_t(this, &Construct::create_action2);

		// @todo: remove abusive negation operator
		entry_ =
			regex("\\Aif\\s*\\(\\s*") > m_expression -> entry()
				> regex("\\A\\s*\\)\\s*") > m_statement -> entry()
		>	(  !regex("\\A\\s*else(?![_[:alnum:]])")
					> pass * doCreateAction1
			|	regex("\\A\\s*else(\\s+|(?=(if)?[^_[:alnum:]]))")
					> m_statement -> entry() > pass * doCreateAction2
			);
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, construct_ifelse, nparse.script.constructs.IfElse)
