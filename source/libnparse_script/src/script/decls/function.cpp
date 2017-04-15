/*
 * @file $/source/libnparse_script/src/script/decls/function.cpp
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
#include <sstream>
#include <nparse/nparse.hpp>
#include <anta/sas/regex.hpp>
#include "../constructs/return.hpp"
#include "../_varname.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

class Function: public IFunction,
	public plugin::auto_factory<Function, plugin::singleton_factory>
{
	typedef std::vector<IEnvironment::key_type> parameters_type;

public:
	// Overridden from IFunction:

	result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		try
		{
			// Create a temporary execution environment.
			IEnvironment temp_env(a_env. get_processor(), m_local_only);

			// Evaluate function arguments.
			parameters_type::const_iterator p = m_parameters. begin();
			arguments_type::const_iterator a = a_arguments. begin();
			while (p != m_parameters. end() && a != a_arguments. end())
			{
				temp_env. ref(*p, true) = a -> evalVal(a_env);
				++ p;
				++ a;
			}

			// Execute function body.
			result_type result = m_body. evalVal(temp_env);

			// If function body does not return a value then compute return
			// value of a local variable which's name matches this function.
			return m_returns_value ? result : temp_env. val(m_name);
		}
		catch (const return_control& rc)
		{
			// Return a value emitted by an explicit return statement inside the
			// function body.
			assert((bool) rc);
			return rc. result();
		}
	}

public:
	// Overridden from IFactory:

	plugin::IPluggable* construct (void* a_address)
	{
		// @todo: This is a side effect of multiple inheritance.
		return this;
	}

public:
	bool add_parameter (const IEnvironment::key_type& a_parameter)
	{
		if (std::find(m_parameters. begin(), m_parameters. end(), a_parameter)
				!= m_parameters. end())
		{
			return false;
		}
		m_parameters. push_back(a_parameter);
		return true;
	}

	bool set_body (const action_pointer& a_body, const bool a_returns_value)
	{
		m_body = a_body;
		m_returns_value = a_returns_value;
		return true;
	}

	std::string get_name () const
	{
		return m_name;
	}

public:
	Function (const std::string& a_name, const bool a_local_only):
		auto_factory (a_name),
		m_local_only (a_local_only),
		m_returns_value (false),
		m_name (a_name. substr(a_name. find_last_of(".:") + 1))
	{
	}

private:
	bool m_local_only;
	bool m_returns_value;
	IEnvironment::key_type m_name;
	parameters_type m_parameters;
	action_pointer m_body;

};

class Construct: public IConstruct
{
//	<LOCAL_PARSER_DATA>
	Function* m_func;

//	</LOCAL_PARSER_DATA>

	std::string getFunctionName (const hnd_arg_t& arg) const
	{
		std::stringstream tmp;
		tmp	<< static_cast<const void*>(& arg. staging) << ':'
			<< arg. staging. getNamespace()
			<< get_accepted_str(arg);
		return tmp. str();
	}

	bool create_function (const hnd_arg_t& arg)
	{
		const std::string& name = getFunctionName(arg);
		try
		{
			m_func = new Function(name, true);
			arg. staging. bind(m_func);
		}
		catch (const plugin::interface_name_conflict& err)
		{
			throw ex::compile_error()
				<< ex::function(get_accepted_str(arg))
				<< ex::location(get_accepted_range(arg))
				<< ex::message("function redefinition");
		}
		return true;
	}

	bool create_procedure (const hnd_arg_t& arg)
	{
		const std::string& name = getFunctionName(arg);
		try
		{
			m_func = new Function(name, false);
			arg. staging. bind(m_func);
		}
		catch (const plugin::interface_name_conflict& err)
		{
			throw ex::compile_error()
				<< ex::function(get_accepted_str(arg))
				<< ex::location(get_accepted_range(arg))
				<< ex::message("procedure redefinition");
		}
		return true;
	}

	bool add_parameter (const hnd_arg_t& arg)
	{
		assert(m_func != NULL);
		if (! m_func -> add_parameter(get_accepted_str(arg)))
		{
			throw ex::compile_error()
				<< ex::function(m_func -> get_name())
				<< ex::location(get_accepted_range(arg))
				<< ex::message("duplicate argument");
		}
		return true;
	}

	bool set_body (const hnd_arg_t& arg)
	{
		assert(m_func != NULL);
		m_func -> set_body(arg. staging. pop(), false);
		m_func = NULL; // plugin manager is the owner
		return true;
	}

	bool set_expr (const hnd_arg_t& arg)
	{
		assert(m_func != NULL);
		m_func -> set_body(arg. staging. pop(), true);
		m_func = NULL; // plugin manager is the owner
		return false;
	}

	plugin::instance<IConstruct> m_block, m_expression;
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
		m_block ("nparse.script.constructs.Block"),
		m_expression ("nparse.script.Expression"),
// <DEBUG_NODE_NAMING>
		entry_		("Function.Entry")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		anta::Label<SG>
			doCreateFunction = hnd_t(this, &Construct::create_function),
			doCreateProcedure = hnd_t(this, &Construct::create_procedure),
			doAddParameter = hnd_t(this, &Construct::add_parameter),
			doSetBody = hnd_t(this, &Construct::set_body),
			doSetExpr = hnd_t(this, &Construct::set_expr);

		entry_ =
			(	re("(func(tion)?\\>\\s*)?") > varName * doCreateFunction
			|	re("proc(edure)?\\>\\s*") > varName * doCreateProcedure
			)
		>	re("\\s*\\(\\s*")
		>  ~( (varName * doAddParameter) % re("\\s*,\\s*") )
		>	re("\\s*\\)\\s*")
		>	(	m_block -> entry() > pass * doSetBody
			|	re("=\\s*") > m_expression -> entry() > pass * doSetExpr
			);
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, decl_function, nparse.script.decls.Function)
