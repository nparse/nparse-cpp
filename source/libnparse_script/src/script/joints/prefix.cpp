/*
 * @file $/source/libnparse_script/src/script/joints/prefix.cpp
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
#include <nparse/nparse.hpp>
#include <anta/sas/regex.hpp>
#include "_priority.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

class Operator: public IOperator
{
	bool assertion (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();
		IStaging::joint_pointer joint(
			& (*joint0)
		);
		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	bool negation (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();
		IStaging::joint_pointer joint(
			! (*joint0)
		);
		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	anta::Label<SG> m_assertion, m_negation;

public:
	Operator ()
	{
		m_assertion = hnd_t(this, &Operator::assertion);
		m_negation = hnd_t(this, &Operator::negation);
	}

public:
	int priority () const
	{
		return PRIORITY_PREFIX;
	}

	void deploy (level_t a_current, level_t a_previous) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		|	re("&\\s*") > a_current > pass * m_assertion
		|	re("!\\s*") > a_current > pass * m_negation;
	}

};

} // namespace

PLUGIN(Operator, joint_prefix, nparse.script.joints.Prefix)
