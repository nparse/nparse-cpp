/*
 * @file $/source/libnparse_runtime/src/functions/swap.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

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
#include "../static.hpp"

namespace {

using namespace nparse;

class Function: public IFunction
{
public:
	// Overridden from IFunction:

	result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		if (a_arguments. size() < 2)
		{
			throw ex::runtime_error()
				<< ex::function("swap")
				<< ex::message("invalid argument count");
		}

		for (arguments_type::const_iterator i = a_arguments. begin() + 1;
				i != a_arguments. end(); ++ i)
		{
			(i - 1) -> evalRef(a_env). swap(i -> evalRef(a_env));
		}

		return result_type();
	}

};

} // namespace

PLUGIN(Function, function_swap, nparse.script.functions.swap)
