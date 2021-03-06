/*
 * @file $/source/libnparse_script/src/script/joints/sequencing.cpp
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
#include <anta/sas/test.hpp>
#include "_joints.hpp"
#include "_priority.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

class Operator: public IOperator
{
	bool compose (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint1 = arg. staging. popJoint();
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();

		if (joint0 -> is_virtual())
		{
			if (joint1 -> is_virtual())
			{
				throw ex::syntax_error()
					<< ex::location(get_accepted_range(arg))
					<< ex::message("unable to compose sequencing modifiers");
			}

			joint1 -> set_flavor(0, joint0 -> get_flavor(1));
			arg. staging. pushJoint(joint1);
			return true;
		}
		else
		if (joint1 -> is_virtual())
		{
			joint0 -> set_flavor(1, joint1 -> get_flavor(0));
			arg. staging. pushJoint(joint0);
			return true;
		}

		IStaging::joint_pointer joint;
		switch (merge_flavors(joint0 -> get_flavor(1), joint1 -> get_flavor(0)))
		{
		case 0: // no separator
			joint(
				*joint0 > *joint1
			);
			break;

		case 1: // optional separator
			joint(
				*joint0 > space_optional() > *joint1
			);
			break;

		case 2: // required separator
			joint(
				*joint0 > space_required() > *joint1
			);
			break;
		}

		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint1 -> get_flavor(1));

		arg. staging. pushJoint(joint);
		return true;
	}

	anta::Label<SG> m_compose;

public:
	Operator ()
	{
		m_compose = hnd_t(this, &Operator::compose);
	}

public:
	int priority () const
	{
		return PRIORITY_SEQUENCING;
	}

	void deploy (level_t a_current, level_t a_previous) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		>  *(space > a_previous > pass * m_compose);
	}

};

} // namespace

PLUGIN(Operator, joint_sequencing, nparse.script.joints.Sequencing)
