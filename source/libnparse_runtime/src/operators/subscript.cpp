/*
 * @file $/source/libnparse_runtime/src/operators/subscript.cpp
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
#include "_binary_action.hpp"
#include <anta/sas/regex.hpp>
#include "_priority.hpp"
#include "../static.hpp"

namespace {

typedef anta::aux::integer<NLG>::type int_t;

class ActionAccess: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// Evaluate object value.
		result_type u = m_operand. evalVal(a_env);

		// If the object is an array then the field represents its key.
		if (u. is_array())
		{
			return u. as_array() -> val(m_field);
		}

		// Otherwise, access operator doesn't make much sence.
		else
		{
			throw ex::runtime_error()
				<< ex::function("operator . `access` [val]")
				<< ex::message("cannot be used with this type");
		}
	}

	result_type& evalRef (IEnvironment& a_env) const
	{
		// Evaluate object reference.
		result_type& u = m_operand. evalRef(a_env);

		// If the object is an array then the field represents its key.
		if (u. is_array())
		{
			return u. as_array() -> ref(m_field);
		}

		// Otherwise, access operator doesn't make much sence.
		else
		{
			throw ex::runtime_error()
				<< ex::function("operator . `access` [ref]")
				<< ex::message("cannot be used with this type");
		}
	}

private:
	const IEnvironment::key_type m_field;
	action_pointer m_operand;

public:
	ActionAccess (const anta::range<SG>::type& a_range, IStaging& a_staging,
			const string_t a_field):
		LinkedAction (a_range), m_field (a_field)
	{
		m_operand = a_staging. pop();
	}

};

class ActionSubscript: public BinaryAction
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
		else if (u. is_string())
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
			else if (-index <= static_cast<int_t>(u_str. size()))
			{
				const nparse::nlg_string_t::const_iterator offset =
					u_str. begin() + u_str. size() + index;
				return result_type(offset, offset + 1);
			}

			throw ex::runtime_error()
				<< ex::function("operator [] `subscript` [val]")
				<< ex::message("index is out of string boundary");
		}

		// Otherwise, subscript operator doesn't make much sence.
		else
		{
			throw ex::runtime_error()
				<< ex::function("operator [] `subscript` [val]")
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
				<< ex::function("operator [] `subscript` [ref]")
				<< ex::message("cannot be used with this type");
		}
	}

public:
	ActionSubscript (const anta::range<SG>::type& a_range, IStaging& a_staging):
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

class Operator: public IOperator
{
	bool create_access (const hnd_arg_t& arg) const
	{
		arg. staging. push(new ActionAccess(get_marked_range(arg), arg. staging,
					get_accepted_str(arg)));
		return true;
	}

	bool create_subscript (const hnd_arg_t& arg) const
	{
		arg. staging. push(new ActionSubscript(get_marked_range(arg),
					arg. staging));
		return true;
	}

	bool create_substring (const hnd_arg_t& arg)
	{
		arg. staging. push(new ActionSubstring(get_marked_range(arg),
					arg. staging));
		return true;
	}

	anta::Label<SG> m_access, m_subscript, m_substring;

public:
	Operator ()
	{
		m_access = hnd_t(this, &Operator::create_access);
		m_subscript = hnd_t(this, &Operator::create_subscript);
		m_substring = hnd_t(this, &Operator::create_substring);
	}

public:
	// Overridden from IOperator:

	int priority () const
	{
		return PRIORITY_SUBSCRIPT;
	}

	void deploy (level_t a_current, level_t a_previous, const levels_t& a_top)
		const
	{
		using namespace anta::ndl::terminals;
		a_current =
			a_previous
		>  *(	(	re("\\s*\\.\\s*") * M0
				>	re("(?!\\d)[_\\w]+") * M1 * m_access
				)
			|	(	re("\\s*\\[\\s*") * M0 > a_top. back()
				>	(	re("\\s*]") * M1 * m_subscript
					|	re("\\s*:\\s*")	> a_top. back()
						> re("\\s*]") * M1 * m_substring
					)
				)
			);
	}

};

} // namespace

PLUGIN(Operator, operator_subscript, nparse.script.operators.Subscript)
