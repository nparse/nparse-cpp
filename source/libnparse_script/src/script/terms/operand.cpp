/*
 * @file $/source/libnparse_script/src/script/terms/operand.cpp
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
#include <anta/sas/test.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/regex.hpp>
#include <anta/sas/token.hpp>
#include "../_action_string.hpp"
#include "../_varname.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

// Represents a scalar constant.
class Action: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		return m_value;
	}

public:
	Action (const result_type& a_value = result_type()):
		m_value (a_value)
	{
	}

protected:
	result_type m_value;

};

// Represents an active variable reference.
class ActionReference: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		return a_env. val(m_action -> evalVal(a_env). as_string());
	}

public:
	ActionReference (const IAction* a_action):
		m_action (a_action)
	{
	}

private:
	const IAction* m_action;

};

// Represents an array constant.
class ActionArray: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		anta::aux::array<NLG>::type res = a_env. create();

		for (key_value_list_t::const_iterator i = m_key_value_list. begin();
				i != m_key_value_list. end(); ++ i)
		{
			i -> second. evalVal(a_env). swap(
				res -> ref(i -> first. evalVal(a_env). as_string(), true));
		}

		return result_type(res);
	}

public:
	void add (const action_pointer& a_action)
	{
		if (m_auto_keys)
		{
			const anta::aux::integer<NLG>::type key_ = m_key_value_list. size();
			IAction::result_type key = key_;
			m_key_value_list. push_back(key_value_t(new Action(key), a_action));
		}
		else if ((++ m_index) & 1)
		{
			m_key_value_list. push_back(key_value_t());
			m_key_value_list. back(). first = a_action;
		}
		else
		{
			m_key_value_list. back(). second = a_action;
		}
	}

	void add (IAction* a_action)
	{
		return add(action_pointer(a_action));
	}

public:
	ActionArray (const bool a_auto_keys):
		m_auto_keys (a_auto_keys), m_index (0)
	{
	}

private:
	typedef std::pair<action_pointer, action_pointer> key_value_t;
	typedef std::vector<key_value_t> key_value_list_t;
	key_value_list_t m_key_value_list;
	const bool m_auto_keys;
	int m_index;

};

class ArrayConstruct: public IConstruct,
	public plugin::auto_factory<ArrayConstruct, plugin::singleton_factory>
{
public:
	// Overridden from IConstruct:

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

public:
	// Overridden from IFactory:

	plugin::IPluggable* construct (void* a_address)
	{
		// @todo: side effect of multiple inheritance
		return this;
	}

public:
	ArrayConstruct (const anta::ndl::Rule<SG>& a_entry):
		auto_factory ("nparse.script.$.Array"), entry_ (a_entry)
	{
	}

private:
	const anta::ndl::Rule<SG>& entry_;

};

class Construct: public IConstruct
{
	bool create_null (const hnd_arg_t& arg)
	{
		arg. staging. push(new Action());
		return true;
	}

	bool create_boolean (const hnd_arg_t& arg)
	{
		const string_t value_str = get_accepted_str(arg);
		bool value;
		if (value_str == L"true")
		{
			value = true;
		}
		else if (value_str == L"false")
		{
			value = false;
		}
		else
		{
			// This should never happen.
			throw std::logic_error("bad logical constant");
		}

		arg. staging. push(new Action(value));
		return true;
	}

	bool create_numeric (const hnd_arg_t& arg)
	{
		const string_t value_str = get_accepted_str(arg);
		IAction::result_type value;
		if (value_str. find_first_of(L".eE") == value_str. npos)
		{
			value = boost::lexical_cast<anta::aux::integer<NLG>::type>(
					encode::unwrap(value_str));
		}
		else
		{
			value = boost::lexical_cast<anta::aux::real<NLG>::type>(
					encode::unwrap(value_str));
		}

		arg. staging. push(new Action(value));
		return true;
	}

	bool create_string (const hnd_arg_t& arg)
	{
		const anta::range<SG>::type& range = get_accepted_range(arg);
		if (*(range. first) == L'"')
		{
			arg. staging. push(new ActionString(range));
		}
		else
		{
			try
			{
				arg. staging. push(new Action(Tokenizer::decode(range)));
			}
			catch (const std::runtime_error& err)
			{
				throw ex::syntax_error()
					<< ex::location(range)
					<< ex::message(err. what());
			}
		}
		return true;
	}

// <LOCAL_PARSER_DATA>
	action_pointer m_last_key;
// </LOCAL_PARSER_DATA>

	bool create_array (const hnd_arg_t& arg)
	{
		arg. staging. push(new ActionArray(false));
		return true;
	}

	bool add_array_key (const hnd_arg_t& arg)
	{
		m_last_key = arg. staging. pop();
		arg. staging. top_as<ActionArray>() -> add(m_last_key);
		return true;
	}

	bool add_array_value (const hnd_arg_t& arg)
	{
		action_pointer value = arg. staging. pop();
		arg. staging. top_as<ActionArray>() -> add(value);
		return true;
	}

	bool add_array_entry (const hnd_arg_t& arg)
	{
		arg. staging. top_as<ActionArray>() -> add(
				new ActionReference(m_last_key. get()));
		return true;
	}

	bool create_list (const hnd_arg_t& arg)
	{
		arg. staging. push(new ActionArray(true));
		return true;
	}

	bool add_list_value (const hnd_arg_t& arg)
	{
		action_pointer value = arg. staging. pop();
		arg. staging. top_as<ActionArray>() -> add(value);
		return true;
	}

	plugin::instance<IConstruct> m_expression;
	ArrayConstruct m_array_construct;
	anta::ndl::Rule<SG> entry_, null_, boolean_, numeric_, string_, array_,
		list_;
	anta::sas::TokenClass<SG> m_tc;

public:
	Construct ():
		m_expression ("nparse.script.Expression"),
		m_array_construct (array_),
// <DEBUG_NODE_NAMING>
		entry_		("Operand.Entry"),
		null_		("Operand.Null"),
		boolean_	("Operand.Boolean"),
		numeric_	("Operand.Numeric"),
		string_		("Operand.String"),
		array_		("Operand.Array"),
		list_		("Operand.List")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		anta::Label<SG>
			doCreateNull	= hnd_t(this, &Construct::create_null),
			doCreateBoolean	= hnd_t(this, &Construct::create_boolean),
			doCreateNumeric	= hnd_t(this, &Construct::create_numeric),
			doCreateString	= hnd_t(this, &Construct::create_string),
			doCreateArray	= hnd_t(this, &Construct::create_array),
			doAddArrayKey	= hnd_t(this, &Construct::add_array_key),
			doAddArrayValue	= hnd_t(this, &Construct::add_array_value),
			doAddArrayEntry	= hnd_t(this, &Construct::add_array_entry),
			doCreateList	= hnd_t(this, &Construct::create_list),
			doAddListValue	= hnd_t(this, &Construct::add_list_value);

		entry_ =
			null_		> pass * doCreateNull
		|	boolean_	> pass * doCreateBoolean
		|	numeric_	> pass * doCreateNumeric
		|	string_		> pass * doCreateString
		|	array_
		|	list_;

		null_ =
			lit("null");

		boolean_ =
			re("(true|false)");

		numeric_ =
			re("[-+]?\\d+(\\.\\d+)?([eE][-+]?\\d+)?");

		// (string constants must be enclosed in single or double quotes)
		string_ =
			re("(?=['\"])") > token(m_tc);

		array_ =
			re("\\{\\s*") * doCreateArray
		>  ~(	(	(	numeric_ > pass * doCreateNumeric
					|	(string_ | varName) > pass * doCreateString
					|	re("\\(\\s*") > m_expression -> entry() > re("\\s*\\)")
					)
				>	space * doAddArrayKey
				>	(	re(":\\s*") > m_expression -> entry(-1)
							> pass * doAddArrayValue
					|	pass * doAddArrayEntry
					)
				)
			%	re("\\s*,\\s*")
			)
		>	re("\\s*\\}");

		list_ =
			re("\\[\\s*") * doCreateList
		>  ~(	(	m_expression -> entry(-1)
				>	pass * doAddListValue
				)
			%	re("\\s*,\\s*")
			)
		>	re("\\s*]");
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, term_operand, nparse.script.terms.Operand)
