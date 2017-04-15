/*
 * @file $/source/libnparse_script/src/script/constructs/block.cpp
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
#include <algorithm>
#include <nparse/nparse.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/dsel/rt/assign.hpp>
#include <anta/dsel/rt/comma.hpp>
#include <anta/dsel/rt/pre_inc.hpp>
#include "../../static.hpp"

namespace {

using namespace nparse;

typedef std::vector<action_pointer> statements_t;

class Action: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		for (statements_t::const_iterator i = m_statements. begin();
				i != m_statements. end(); ++ i)
		{
			(*i). evalVal(a_env);
		}
		return result_type(true);
	}

public:
	Action (statements_t& a_statements)
	{
		m_statements. swap(a_statements);
	}

private:
	statements_t m_statements;

};

class Construct: public IConstruct
{
	bool create_action (const hnd_arg_t& arg)
	{
		// Determine the statement count.
		const anta::aux::integer<SG>::type stmc =
			arg. state. val("stmc"). as_integer();

		// Collect the statements.
		statements_t stms;
		stms. reserve(stmc);
		for (int n = 0; n < stmc; ++ n)
		{
			stms. push_back(arg. staging. pop());
		}
		std::reverse(stms. begin(), stms. end());

		arg. staging. push(new Action(stms));

		return true;
	}

	plugin::instance<IConstruct> m_comment, m_statement;
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
		m_comment ("nparse.script.Comment"),
		m_statement ("nparse.script.Statement"),
// <DEBUG_NODE_NAMING>
		entry_		("Block.Entry")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;
		using namespace anta::dsel;
		using boost::proto::lit;

		anta::Label<SG>
			doCreateAction = hnd_t(this, &Construct::create_action);

		// Statement count.
		reference<SG>::type stmc = ref<SG>("stmc");

		entry_ =
			lit('{') [ stmc = 0L, true ]
		>  *(	pass [ push(stmc) ]
			>	m_statement -> entry()
			>	pass [ pop(stmc), ++ stmc ]
			)
		>	m_comment -> entry()
		>	lit('}') * doCreateAction;
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, construct_block, nparse.script.constructs.Block)
