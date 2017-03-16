/*
 * @file $/source/libnparse_runtime/src/functions/nth.cpp
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
#include <nparse/nparse.hpp>
#include "../static.hpp"

namespace {

using namespace nparse;

class Function: public IFunction
{
	arguments_type::value_type select (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		if (a_arguments. size() < 2)
		{
			throw ex::runtime_error()
				<< ex::function("nth")
				<< ex::message("invalid argument count");
		}

		typedef anta::aux::integer<NLG>::type int_t;
		const int_t n = a_arguments. front(). evalVal(a_env). as_integer();

		if (1 > n || n >= static_cast<int_t>(a_arguments. size()))
		{
			throw ex::runtime_error()
				<< ex::function("nth")
				<< ex::message("invalid index");
		}

		return a_arguments[n];
	}

public:
	// Overridden from IFunction:

	result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		return select(a_env, a_arguments). evalVal(a_env);
	}

	result_type& evalRef (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		return select(a_env, a_arguments). evalRef(a_env);
	}

};

} // namespace

PLUGIN(Function, function_nth, nparse.script.functions.nth)
