/*
 * @file $/source/libnparse_script/src/script/joints/prefix.cpp
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
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include "../../static.hpp"
#include "_priority.hpp"

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

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		|	'&' > space > a_current > pass * m_assertion
		|	'!' > space > a_current > pass * m_negation;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Operator, joint_prefix, nparse.script.joints.Prefix, 1 )
