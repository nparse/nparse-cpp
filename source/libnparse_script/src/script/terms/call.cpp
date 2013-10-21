/*
 * @file $/source/libnparse_script/src/script/terms/call.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.3

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
#include <sstream>
#include <algorithm>
#include <nparse/nparse.hpp>
#include <nparse/util/linked_action.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/regex.hpp>
#include <anta/dsel/rt/assign.hpp>
#include <anta/dsel/rt/comma.hpp>
#include <anta/dsel/rt/pre_inc.hpp>
#include <anta/dsel/rt/plus.hpp>
#include "../_varname.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

class Action: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		return instantiate(). evalVal(a_env, m_arguments);
	}

	result_type& evalRef (IEnvironment& a_env) const
	{
		return instantiate(). evalRef(a_env, m_arguments);
	}

public:
	Action (const anta::range<SG>::type& a_range, const std::string& a_name,
			IFunction::arguments_type& a_arguments, IStaging& a_staging):
		LinkedAction (a_range), m_name (a_name), m_staging (&a_staging),
		m_namespace (a_staging. getNamespace())
	{
		m_arguments. swap(a_arguments);
	}

private:
	IFunction& instantiate () const
	{
		if (! m_instance)
		{
			try
			{
				m_instance. instantiate(m_name);
				m_instance -> link(m_staging, m_namespace);
			}
			catch (const plugin::unknown_interface& err)
			{
				anta::range<SG>::type where;
				getLocation(where);
				throw ex::runtime_error()
					<< ex::function(m_name)
					<< ex::location(where)
					<< ex::message("undefined function or procedure");
			}
		}
		return *m_instance;
	}

	std::string m_name;
	IStaging* m_staging;
	string_t m_namespace;
	IFunction::arguments_type m_arguments;
	mutable plugin::instance<IFunction> m_instance;

};

class Construct: public IConstruct
{
	plugin::instance<IConstruct> m_expression;
	anta::ndl::Rule<SG> entry_;

	bool create_call (const hnd_arg_t& arg)
	{
		// Determine the name of the function.
		const anta::string<SG>::type func =
			arg. state. val("func"). as_string();

		// Determine the argument count.
		const anta::aux::integer<SG>::type argc =
			arg. state. val("argc"). as_integer();

		// Determine function namespace prefix.
		std::stringstream tmp;
		tmp << "nparse.script.functions." << func;

		// Built-in functions are preferrable.
		if (! plugin::manager::instance(). is_provided(tmp. str()))
		{
			tmp. str("");
			tmp. clear();
			tmp << static_cast<const void*>(& arg. staging) << ':';
			if (func. find_last_of(L".:") == func. npos)
				tmp << arg. staging. getNamespace();
			tmp << func;
		}

		// Collect the arguments.
		IFunction::arguments_type args;
		args. reserve(argc);
		for (int n = 0; n < argc; ++ n)
			args. push_back(arg. staging. pop());
		std::reverse(args. begin(), args. end());

		// Create and push a function call action.
		action_pointer impl(new Action(
			get_marked_range(arg), tmp. str(), args, arg. staging
		));
		arg. staging. push(impl);

		return true;
	}

public:
	Construct ():
		m_expression ("nparse.script.Expression"),
// <DEBUG_NODE_NAMING>
		entry_		("Call.Entry")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;
		using namespace anta::dsel;
		using boost::proto::lit;

		anta::Label<SG> doCreateCall = hnd_t(this, &Construct::create_call);

		reference<SG>::type
			pref = ref<SG>("pref"), // Namespace prefix.
			func = ref<SG>("func"),	// Function name.
			argc = ref<SG>("argc");	// Argument count.

		entry_ =
				(	regex("\\A(?=\\w)[-_\\.\\w]+(?<=\\w)(?=\\s*::)")
						[ pref = delta() + ".", true ] > regex("\\s*::\\s*")
				|	pass[ pref = "", true ]
				)
			>	varName [ func = pref + delta(), argc = 0, true ] * M0
			>	space
			>	'('
			>  ~(	(	space [ ++ argc, push(func), push(argc) ]
					>	m_expression -> entry(-1)
					>	pass [ pop(func), pop(argc) ]
					) % (
						space
					>	','
					)
				)
			>	space * M1 * doCreateCall
			>	')';
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Construct, term_call, nparse.script.terms.Call, 1 )
