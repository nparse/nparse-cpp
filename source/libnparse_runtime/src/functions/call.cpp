/*
 * @file $/source/libnparse_runtime/src/functions/call.cpp
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
#include <sstream>
#include <nparse/nparse.hpp>
#include "../static.hpp"

namespace {

using namespace nparse;

// @todo: can we avoid argument list copying?
class Function: public IFunction
{
public:
	// Overridden from IFunction:

	result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		if (a_arguments. empty())
		{
			throw ex::runtime_error()
				<< ex::function("call/val")
				<< ex::message("invalid argument count");
		}

		const arguments_type args(a_arguments. begin() + 1, a_arguments. end());
		return instantiate(a_env, a_arguments). evalVal(a_env, args);
	}

	result_type& evalRef (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		if (a_arguments. empty())
		{
			throw ex::runtime_error()
				<< ex::function("call/ref")
				<< ex::message("invalid argument count");
		}

		const arguments_type args(a_arguments. begin() + 1, a_arguments. end());
		return instantiate(a_env, a_arguments). evalRef(a_env, args);
	}

	void link (IStaging* a_staging, const nparse::string_t& a_namespace)
	{
		m_staging = a_staging;
		m_namespace = a_namespace;
	}

public:
	Function ():
		m_staging (NULL)
	{
	}

private:
	IFunction& instantiate (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		// Determine the name of the function.
		const std::string func = a_arguments[0]. evalVal(a_env). as_string();

		// Determine function namespace prefix.
		std::stringstream tmp;
		tmp << "nparse.script.functions." << func;

		// Built-in functions are preferrable.
		if (! plugin::IManager::instance(). factory_exists(tmp. str()))
		{
			tmp. str("");
			tmp. clear();
			tmp << static_cast<const void*>(m_staging) << ':';
			if (func. find_last_of(".:") == func. npos)
				tmp << m_namespace;
			tmp << func;
		}

		// Try to instantiate the function.
		plugin::IPluggable* ptr =
			plugin::IManager::instance(). create(tmp. str());
		if (ptr == NULL)
		{
			// NOTE: Unlike term call, implicit call throws flow control rather
			//		 than 'undefined function/procedure' exception.
			throw flow_control(false);
		}

		IFunction* function = dynamic_cast<IFunction*>(ptr);
		function -> link(m_staging, m_namespace);
		return *function;
	}

	IStaging* m_staging;
	nparse::string_t m_namespace;

};

} // namespace

PLUGIN(Function, function_call, nparse.script.functions.call)
