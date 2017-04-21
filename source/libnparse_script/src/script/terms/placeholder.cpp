/*
 * @file $/source/libnparse_script/src/script/terms/placeholder.cpp
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
#include <nparse/nparse.hpp>
#include <nparse/util/linked_action.hpp>
#include <nparse/util/offset_state.hpp>
#include <anta/sas/regex.hpp>
#include "../../static.hpp"

namespace {

using namespace nparse;

class Action: public LinkedAction
{
protected:
	virtual int offset (IEnvironment& a_env) const = 0;

public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		result_type res;

		const anta::State<NLG>* s = offset_state<NLG>(
				& a_env. get_processor(). get_state(), offset(a_env));

		if (!s) throw flow_control(false);

		try
		{
			const anta::range<NLG>::type& r = s -> get_range();
			result_type(r. first, r. second). swap(res);
			switch (m_type)
			{
			case 'b':
				res = res. as_boolean();
				break;
			case 'i':
				res = res. as_integer();
				break;
			case 'f':
			case 'd':
			case 'r':
				res = res. as_real();
				break;
			case 's':
			default:
				break;
			}
		}
		catch (...) // we assume it's a bad cast
		{
			throw flow_control(false);
		}

		return res;
	}

public:
	Action (const anta::range<SG>::type& a_range, const char a_type):
		LinkedAction (a_range), m_type (a_type)
	{
	}

private:
	char m_type;

};

class ActionConstant: public Action
{
protected:
	int offset (IEnvironment& a_env) const
	{
		return m_offset;
	}

public:
	ActionConstant (const anta::range<SG>::type& a_range, const char a_type,
			const int a_offset):
		Action (a_range, a_type), m_offset (a_offset)
	{
	}

private:
	int m_offset;

};

class ActionParametrized: public Action
{
protected:
	int offset (IEnvironment& a_env) const
	{
		return static_cast<int>(m_offset -> evalVal(a_env). as_integer());
	}

public:
	ActionParametrized (const anta::range<SG>::type& a_range, const char a_type,
			IStaging& a_staging):
		Action (a_range, a_type)
	{
		m_offset = a_staging. pop();
	}

private:
	action_pointer m_offset;

};

class Construct: public IConstruct
{
	bool create_fixed (const hnd_arg_t& arg)
	{
		anta::range<SG>::type range = get_accepted_range(arg);
		assert(static_cast<char>(*range. first) == '$');

		// Identify enforced type.
		char type = static_cast<char>(*(++ range. first));
		switch (type)
		{
		case 'b': // boolean
		case 'i': // integer
		case 'f': // float
		case 'd': // double
		case 'r': // real
		case 's': // string
			++ range. first;
			break;

		default:
			type = 's';
			break;
		}

		// Identify placeholder offset.
		const string_t value_str(range. first, range. second);
		const int offset = value_str. empty()
			? 0 : boost::lexical_cast<int>(encode::unwrap(value_str));

		// Instantiate placeholder and add to the LL stack.
		arg. staging. push(new ActionConstant(range, type, offset));

		return true;
	}

	bool create_parametric (const hnd_arg_t& arg)
	{
		anta::range<SG>::type range = get_marked_range(arg);
		assert(static_cast<char>(*range. first) == '$');

		// Identify enforced type.
		char type = static_cast<char>(*(++ range. first));

		arg. staging. push(new ActionParametrized(range, type, arg. staging));
		return true;
	}

	plugin::instance<IConstruct> m_expression;
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
		m_expression ("nparse.script.Expression"),
// <DEBUG_NODE_NAMING>
		entry_		("Placeholder.Entry")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;

		anta::Label<SG>
			doFixed = hnd_t(this, &Construct::create_fixed),
			doParametric = hnd_t(this, &Construct::create_parametric);

		entry_ =
			re("\\$[bifdrs]?\\d*") * doFixed
		|	(	re("\\$[bifdrs]?\\s*\\(\\s*") * M0
			>	m_expression -> entry()
			>	re("\\s*\\)") * M1 * doParametric
			);
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, term_placeholder, nparse.script.terms.Placeholder)
