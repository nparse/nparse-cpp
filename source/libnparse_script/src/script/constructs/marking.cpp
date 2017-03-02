/*
 * @file $/source/libnparse_script/src/script/constructs/marking.cpp
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
#include <nparse/nparse.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/regex.hpp>
#include "../_varname.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

typedef std::vector<IEnvironment::key_type> targets_t;

class Action: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		anta::Traveller<NLG>& traveller = a_env. get_traveller();
		anta::ndl::Context<NLG>* context =
			traveller. get_state(). context(&traveller);

		switch (m_type)
		{
		case 0:
			for (targets_t::const_iterator i = m_targets. begin();
					i != m_targets. end(); ++ i)
			{
				context -> push(*i);
			}
			break;

		case 1:
			for (targets_t::const_iterator i = m_targets. begin();
					i != m_targets. end(); ++ i)
			{
				context -> pop(*i);
			}
			break;

		default:
			throw std::logic_error("unsupported marking type");
		}

		return result_type();
	}

public:
	Action (const int a_type, targets_t& a_targets):
		m_type (a_type)
	{
		m_targets. swap(a_targets);
	}

private:
	int m_type;
	targets_t m_targets;

};

class Construct: public IConstruct
{
//	<LOCAL_PARSER_DATA>
	int m_type;
	targets_t m_targets;

//	</LOCAL_PARSER_DATA>

	bool set_type (const hnd_arg_t& arg)
	{
		assert(m_targets. empty());

		const string_t type = get_accepted_str(arg);

		if (type == L"push")
		{
			m_type = 0;
		}
		else
		if (type == L"pop")
		{
			m_type = 1;
		}
		else
		{
			throw std::logic_error("bad marking operator");
		}

		return true;
	}

	bool add_target (const hnd_arg_t& arg)
	{
		const targets_t::value_type target(get_accepted_str(arg));
		m_targets. push_back(target);
		return true;
	}

	bool create_action (const hnd_arg_t& arg)
	{
		assert(m_type == 0 || m_type == 1);
		assert(!m_targets. empty());

		arg. staging. push(new Action(m_type, m_targets));
		return true;
	}

	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
// <DEBUG_NODE_NAMING>
		entry_		("Marking.Entry")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		anta::Label<SG>
			doSetType = hnd_t(this, &Construct::set_type),
			doAddTarget = hnd_t(this, &Construct::add_target),
			doCreateAction = hnd_t(this, &Construct::create_action);

		entry_ =
			regex("\\A(push|pop)") * doSetType > +space
		>	varName * doAddTarget % (space > ',' > space)
		>	space * doCreateAction > ';';
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, construct_marking, nparse.script.constructs.Marking)
