/*
 * @file $/source/libnparse_script/src/script/constructs/control.cpp
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
#include <anta/dsel/rt/assign.hpp>
#include <anta/dsel/rt/comma.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include "../../static.hpp"
#include "control.hpp"

namespace {

using namespace nparse;

class Action: public IAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		throw m_ctrl;
	}

public:
	Action (const bool a_exit = true, const std::string& a_target =
			std::string()):
		m_ctrl (a_exit, a_target)
	{
	}

private:
	loop_control_t m_ctrl;

};

class Construct: public IConstruct
{
	bool create_action (const hnd_arg_t& arg)
	{
		// Determine the type of loop control (break or continue).
		const anta::aux::boolean<SG>::type
			exit = arg. state. val("exit"). as_boolean();

		// Determine the name of the loop.
		const anta::string<SG>::type
			loop = arg. state. val("loop"). as_string();

		arg. staging. push(new Action(exit, loop));

		return true;
	}

	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
// <DEBUG_NODE_NAMING>
		entry_		("Control.Entry")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;
		using namespace anta::dsel;
		using boost::proto::lit;

		anta::Label<SG>
			doCreateAction = hnd_t(this, &Construct::create_action);

		reference<SG>::type
			exit = ref<SG>("exit"),
			loop = ref<SG>("loop");

		entry_ =
			(	lit("break") [ exit = true, true ]
			|	lit("continue") [ exit = false, true ]
			)
		>	(	+space > (+alnum) [ loop = delta(), true ]
			|	pass [ loop = "", true ]
			)
		>	space * doCreateAction > ';';
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, construct_control, nparse.script.constructs.Control)
