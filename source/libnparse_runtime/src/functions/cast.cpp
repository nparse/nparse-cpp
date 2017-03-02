/*
 * @file $/source/libnparse_runtime/src/functions/cast.cpp
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
#include "../static.hpp"

namespace {

using namespace nparse;

template <typename Tag_>
class Function: public IFunction
{
public:
	// Overridden from IFunction:

	result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		typedef typename Tag_::type type;
		static const type dummy = type();

		switch (a_arguments. size())
		{
		case 0:
			return result_type(dummy);

		case 1:
			return a_arguments. front(). evalVal(a_env). as<type>();

		default:
			throw ex::runtime_error()
				<< ex::function("type cast")
				<< ex::message("invalid argument count");
		}
	}

};

template <>
class Function<anta::aux::array<NLG> >: public IFunction
{
public:
	// Overridden from IFunction:

	result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		switch (a_arguments. size())
		{
		case 0:
			return result_type(a_env. create());

		case 1:
			{
				anta::aux::array<NLG>::type res(a_env. create());
				res -> ref('0', true) = a_arguments. front(). evalVal(a_env);
				return result_type(res);
			}

		default:
			throw ex::runtime_error()
				<< ex::function("type cast")
				<< ex::message("invalid argument count");
		}
	}

};

} // namespace

PLUGIN(Function<anta::aux::boolean<NLG> >,
		function_boolean, nparse.script.functions.boolean)

PLUGIN(Function<anta::aux::integer<NLG> >,
		function_integer, nparse.script.functions.integer)

PLUGIN(Function<anta::aux::real<NLG> >,
		function_real, nparse.script.functions.real)

PLUGIN(Function<anta::string<NLG> >,
		function_string, nparse.script.functions.string)

PLUGIN(Function<anta::aux::array<NLG> >,
		function_array, nparse.script.functions.array)
