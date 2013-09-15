/*
 * @file $/source/libnparse_script/src/script/terms/constant.cpp
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
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/set.hpp>
#include <anta/sas/token.hpp>
#include <anta/sas/regex.hpp>
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
				res -> ref(
					i -> first. evalVal(a_env). as_string(), true
				)
			);
		}

		return result_type(res);
	}

public:
	void add (const action_pointer& a_action)
	{
		if ((m_index++ & 1) == 0)
		{
			// Creating a new key-value pair.
			m_key_value_list. push_back(key_value_t());
			// Adding key.
			m_key_value_list. back(). first = a_action;
		}
		else
		{
			// Adding value.
			m_key_value_list. back(). second = a_action;
		}
	}

	void add (IAction* a_action)
	{
		return add(action_pointer(a_action));
	}

public:
	ActionArray ():
		m_index (0)
	{
	}

private:
	typedef std::pair<action_pointer, action_pointer>
		key_value_t;
	typedef std::vector<key_value_t> key_value_list_t;
	key_value_list_t m_key_value_list;
	int m_index;

};

class Construct: public IConstruct
{
	bool create_empty (const hnd_arg_t& arg)
	{
		action_pointer impl(new Action());
		arg. staging. push(impl);
		return true;
	}

	bool create_boolean (const hnd_arg_t& arg)
	{
		const string_t value_str = get_accepted_str(arg);
		bool value;
		if (value_str == L"true")
			value = true;
		else
		if (value_str == L"false")
			value = false;
		else
		{
			throw std::logic_error("bad logical constant");
		//	throw ex::syntax_error()
		//		<< ex::message("bad logical constant");
		}

		action_pointer impl(new Action(value));
		arg. staging. push(impl);
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

		action_pointer impl(new Action(value));
		arg. staging. push(impl);
		return true;
	}

	bool create_string (const hnd_arg_t& arg)
	{
		const anta::range<SG>::type& range = get_accepted_range(arg);
		if (*arg. state. get_range(). first == L'"')
		{
			arg. staging. push(action_pointer(new ActionString(range)));
		}
		else
		{
			try
			{
				arg. staging. push(action_pointer(new Action(
					Tokenizer::decode(range)
				)));
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

	bool m_expecting_value;
	std::stack<ActionArray*> m_array_stack;
	action_pointer m_key_value;

	bool create_key (const hnd_arg_t& arg)
	{
		// If the array stack is empty then create an empty array and push it to
		// the top.
		if (m_array_stack. empty())
		{
			ActionArray* val = new ActionArray();
			m_array_stack. push(val);
		}

		// When a value is expected and a key is given it means that an enclosed
		// array is being created. In this case we create an empty array and add
		// it as a value to the array on the top of the stack. The given key
		// goes to the newly created subarray.
		if (m_expecting_value)
		{
			ActionArray* val = new ActionArray();
			m_array_stack. top() -> add(val);
			m_array_stack. push(val);
		}

		// Add the key.
		m_key_value = arg. staging. pop();
		m_array_stack. top() -> add(m_key_value);

		// Expect a value.
		m_expecting_value = true;
		return true;
	}

	bool create_value (const hnd_arg_t& arg)
	{
		// A subarray has already been added as a value.
		if (! m_expecting_value)
			return true;

		assert(! m_array_stack. empty());

		// Add the value.
		m_array_stack. top() -> add(arg. staging. pop());

		// Expect the next key or end of array.
		m_expecting_value = false;
		return true;
	}

	bool create_flag (const hnd_arg_t& arg)
	{
		// A subarray has already been added as a value.
		if (! m_expecting_value)
			return true;

		assert(! m_array_stack. empty());

		// Add the value.
		action_pointer impl(new ActionReference(
			m_key_value. get()
		));
		m_array_stack. top() -> add(impl);

		// Expect the next key or end of array.
		m_expecting_value = false;
		return true;
	}

	bool create_array (const hnd_arg_t& arg)
	{
		if (m_expecting_value || m_array_stack. empty())
		{
			// Push an empty array action.
			arg. staging. push(action_pointer(new ActionArray()));
		}
		else
		{
			// Pop the top from the array stack.
			ActionArray* top = m_array_stack. top();
			m_array_stack. pop();

			// If the top array was the only item in the stack then push it as
			// an action to the staging object.
			if (m_array_stack. empty())
				arg. staging. push(action_pointer(top));
		}
		return true;
	}

	plugin::instance<IConstruct> m_expression;
	anta::ndl::Rule<SG> entry_, empty_, boolean_, numeric_, string_, array_;
	anta::sas::TokenClass<SG> m_tc;

public:
	Construct ():
		m_expecting_value (false),
		m_expression ("nparse.script.Expression"),
// <DEBUG_NODE_NAMING>
		entry_		("Constant.Entry"),
		empty_		("Constant.Empty"),
		boolean_	("Constant.Boolean"),
		numeric_	("Constant.Numeric"),
		string_		("Constant.String"),
		array_		("Constant.Array")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		anta::Label<SG>
			doCreateEmpty	= hnd_t(this, &Construct::create_empty),
			doCreateBoolean	= hnd_t(this, &Construct::create_boolean),
			doCreateNumeric	= hnd_t(this, &Construct::create_numeric),
			doCreateString	= hnd_t(this, &Construct::create_string),
			doCreateArray	= hnd_t(this, &Construct::create_array),
			doCreateKey		= hnd_t(this, &Construct::create_key),
			doCreateValue	= hnd_t(this, &Construct::create_value),
			doCreateFlag	= hnd_t(this, &Construct::create_flag);

		entry_ =
			empty_		> pass * doCreateEmpty
		|	boolean_	> pass * doCreateBoolean
		|	numeric_	> pass * doCreateNumeric
		|	string_		> pass * doCreateString
		|	array_		> pass * doCreateArray;

		empty_ = lit("null");

		boolean_ =
			regex("\\A(true|false)(?![_\\w])");
//			lit("true") | "false";

		numeric_ =
			regex("\\A[-+]?\\d+(\\.\\d+)?([eE][-+]?\\d+)?");
//		   ~(lit('-') | '+')	// sign
//		>	+digit				// integer part;
//		>  ~('.' > +digit)		// fractional part
//		>  ~((lit('e') | 'E') > ~(lit('-') | '+') > +digit); // exponent

		string_ = &set("'\"") > token(m_tc); // string constants must be quoted

		array_ =
			'{'
		>  ~(	(	space
				>	(	numeric_ > pass * doCreateNumeric
					|	(string_ | varName) > pass * doCreateString
					|	'(' > space > m_expression -> entry() > space > ')' )
				>	space * doCreateKey
				>	(	':' > space > m_expression -> entry(-1)
							> pass * doCreateValue
					|	pass * doCreateFlag )
				)
			%	(space > ',')
			)
		>	space > '}';
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Construct, term_constant, nparse.script.terms.Constant, 1 )
