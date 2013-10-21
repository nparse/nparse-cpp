/*
 * @file $/source/libnparse_runtime/src/functions/self.cpp
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
		if (a_arguments. size() > 1)
		{
			throw ex::runtime_error()
				<< ex::function("self")
				<< ex::message("invalid argument count");
		}

		const anta::ndl::Context<NLG>* self = a_env. self();

		if (! a_arguments. empty())
		{
			result_type arg = a_arguments. front(). evalVal(a_env);
			if (! arg. is_array())
			{
				throw ex::runtime_error()
					<< ex::function("self")
					<< ex::message("invalid argument type");
			}

			// <HACK> Makes it possible to override current context within a
			//		  syntactic functor.
			static const IEnvironment::key_type zero = IEnvironment::key_type();
			const anta::ndl::Context<NLG>* chained = self -> find(zero);
			if (chained != NULL)
			{
				// NOTE: For the explanation of the need of double ancestor
				//		 backward step below, see the definition of the zero
				//		 member in ../joints/functor.cpp : PreCond::evalVal()
				anta::aux::array<NLG>::type chained2 = a_env. create(
						chained -> get_ancestor() -> get_ancestor());
				chained2 -> ref(zero, true) = chained -> val(zero);
				chained2 -> push(zero);

				const anta::ndl::Context<NLG>* target =& arg. array();
				while (target -> get_ancestor() != NULL)
					target = target -> get_ancestor();
				const_cast<anta::ndl::Context<NLG>*>(target)
					-> set_ancestor(&* chained2);
			}
			// </HACK>

			a_env. get_traveller(). get_state(). substitute(& arg. array());
		}

		return anta::aux::array<NLG>::type(a_env. create(self));
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Function, function_self, nparse.script.functions.self, 1 )
