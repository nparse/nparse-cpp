/*
 * @file $/source/libnparse_runtime/src/operators/subscripting.cpp
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
#include <algorithm>
#include <nparse/nparse.hpp>
#include "_binary_action.hpp"
#include "_binary_operator.hpp"
#include "_priority.hpp"
#include "../static.hpp"

namespace {

typedef anta::aux::integer<NLG>::type int_t;

class Action: public BinaryAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// Evaluate object value.
		result_type u = m_left. evalVal(a_env);

		// Evaluate index.
		const result_type v = m_right. evalVal(a_env);

		// If the object is an array then the index represents its key.
		if (u. is_array())
		{
			const IEnvironment::key_type key = v. as_string();
			return u. as_array() -> val(key);
		}

		// If the object is a string then the index represents an offset.
		else
		if (u. is_string())
		{
			const nparse::nlg_string_t u_str = u. as_string();
			const int_t index = v. as_integer();
			if (index >= 0)
			{
				if (index < static_cast<int_t>(u_str. size()))
				{
					const nparse::nlg_string_t::const_iterator offset =
						u_str. begin() + index;
					return result_type(offset, offset + 1);
				}
			}
			else
			if (-index <= static_cast<int_t>(u_str. size()))
			{
				const nparse::nlg_string_t::const_iterator offset =
					u_str. begin() + u_str. size() + index;
				return result_type(offset, offset + 1);
			}
			throw ex::runtime_error()
				<< ex::function("subscript/read")
				<< ex::message("index is out of string boundary");
		}

		// Otherwise, subscript operator doesn't make much sence.
		else
		{
			throw ex::runtime_error()
				<< ex::function("subscript/read")
				<< ex::message("cannot be used with this type");
		}
	}

	result_type& evalRef (IEnvironment& a_env) const
	{
		// Evaluate object reference.
		result_type& u = m_left. evalRef(a_env);

		// Evaluate index.
		const result_type v = m_right. evalVal(a_env);

		// If the object is an array then the index represents its key.
		if (u. is_array())
		{
			const IEnvironment::key_type key = v. as_string();
			return u. as_array() -> ref(key);
		}

		// Otherwise, subscript operator doesn't make much sence.
		else
		{
			throw ex::runtime_error()
				<< ex::function("subscript/write")
				<< ex::message("cannot be used with this type");
		}
	}

public:
	Action (const anta::range<SG>::type& a_range, IStaging& a_staging):
		BinaryAction (a_range, a_staging)
	{
	}

};

class ActionSubstring: public LinkedAction
{
	static int_t clip (const int_t& a_index, const int_t& a_length)
	{
		return (a_index >= 0)
			? std::min<int_t>(a_index, a_length)
			: a_length + std::max<int_t>(a_index, -a_length);
	}

public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// Evaluate object value.
		nparse::nlg_string_t u = m_object. evalVal(a_env). as_string();
		const int_t size = static_cast<int_t>(u. size());
		// Evaluate substring range and map it onto an iterator pair.
		nlg_string_t::iterator
			f = u. begin() + clip(m_begin. evalVal(a_env). as_integer(), size),
			t = u. begin() + clip(m_end. evalVal(a_env). as_integer(), size);
		// Return the selected string region.
		if (f <= t)
		{
			// (direct order)
			return nlg_string_t(f, t);
		}
		else
		{
			// (reverse order)
			std::reverse(t, f);
			return nlg_string_t(t, f);
		}
	}

public:
	ActionSubstring (const anta::range<SG>::type& a_range, IStaging& a_staging):
		LinkedAction (a_range)
	{
		m_end = a_staging. pop();
		m_begin = a_staging. pop();
		m_object = a_staging. pop();
	}

private:
	action_pointer m_object, m_begin, m_end;

};

class Operator: public BinaryOperator
{
public:
	// Overridden from IOperator:

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		a_current = a_previous
			>  *(	space
				>	'[' * M0 > space > a_top > space
				>	(	']' * M1 * m_action
					|	':'	> space > a_top > space > ']' * M1 * m_substring
					)
				);
	}

	// Overridden from BinaryOperator:

	IAction* create_action (const anta::range<SG>::type& a_range,
			IStaging& a_staging) const
	{
		return new Action(a_range, a_staging);
	}

private:
	anta::Label<SG> m_substring;

	bool push_substring (const hnd_arg_t& arg)
	{
		arg. staging. push(new ActionSubstring(get_marked_range(arg),
					arg. staging));
		return true;
	}

public:
	Operator ():
		BinaryOperator (PRIORITY_SUBSCRIPTING, "[]")
	{
		m_substring = hnd_t(this, &Operator::push_substring);
	}

};

} // namespace

PLUGIN(Operator, operator_subscripting, nparse.script.operators.Subscripting)
