/*
 * @file $/source/libnparse_script/src/script/terms/variable.cpp
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
#include <nparse/nparse.hpp>
#include <nparse/util/linked_action.hpp>
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
		return a_env. val(m_key);
	}

	result_type& evalRef (IEnvironment& a_env) const
	{
		return a_env. ref(m_key, false);
	}

public:
	Action (const anta::range<SG>::type& a_range):
		LinkedAction (a_range),
		m_key (string_t(a_range. first, a_range. second))
	{
	}

private:
	IEnvironment::key_type m_key;

};

class Construct: public IConstruct
{
	bool create_reference (const hnd_arg_t& arg)
	{
		action_pointer impl(new Action(get_accepted_range(arg)));
		arg. staging. push(impl);
		return true;
	}

	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
// <DEBUG_NODE_NAMING>
		entry_		("Variable.Entry")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;

		anta::Label<SG>
			doCreateReference = hnd_t(this, &Construct::create_reference);

		entry_ = varName * doCreateReference;
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Construct, term_variable, nparse.script.terms.Variable, 1 )