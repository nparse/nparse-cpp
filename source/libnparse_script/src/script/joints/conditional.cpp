/*
 * @file $/source/libnparse_script/src/script/joints/conditional.cpp
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
#include "_joints.hpp"
#include "_priority.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

class LeftOperator: public IOperator
{
	bool conditional (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint1 = arg. staging. popJoint();
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();

		if (joint0 -> is_virtual() || joint1 -> is_virtual())
		{
			throw ex::syntax_error()
				<< ex::location(get_accepted_range(arg))
				<< ex::message("left conditional composition with a sequencing"
						" modifier");
		}

		IStaging::joint_pointer joint;
		switch (merge_flavors(joint0 -> get_flavor(1), joint1 -> get_flavor(0)))
		{
		case 0: // no separator
			joint(
				*joint0 > ~ (*joint1)
			);
			break;

		case 1: // optional separator
			joint(
				*joint0 > ~ (space_optional() > *joint1)
			);
			break;

		case 2: // required separator
			joint(
				*joint0 > ~ (space_required() > *joint1)
			);
			break;
		}

		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint1 -> get_flavor(1));

		arg. staging. pushJoint(joint);
		return true;
	}

	anta::Label<SG> m_conditional;

public:
	LeftOperator ()
	{
		m_conditional = hnd_t(this, &LeftOperator::conditional);
	}

	int priority () const
	{
		return PRIORITY_CONDITIONAL;
	}

	void deploy (level_t a_current, level_t a_previous) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		>  *(	re("\\s*\\?>\\s*")
			>	a_previous
			>	pass * m_conditional
			);
	}

};

class RightOperator: public IOperator
{
	bool conditional (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint1 = arg. staging. popJoint();
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();

		if (joint0 -> is_virtual() || joint1 -> is_virtual())
		{
			throw ex::syntax_error()
				<< ex::location(get_accepted_range(arg))
				<< ex::message("right conditional composition with a sequencing"
						" modifier");
		}

		IStaging::joint_pointer joint;
		switch (merge_flavors(joint0 -> get_flavor(1), joint1 -> get_flavor(0)))
		{
		case 0: // no separator
			joint(
				~(*joint0) > *joint1
			);
			break;

		case 1: // optional separator
			joint(
				~(*joint0 > space_optional()) > *joint1
			);
			break;

		case 2: // required separator
			joint(
				~(*joint0 > space_required()) > *joint1
			);
			break;
		}

		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint1 -> get_flavor(1));

		arg. staging. pushJoint(joint);
		return true;
	}

	anta::Label<SG> m_conditional;

public:
	RightOperator ()
	{
		m_conditional = hnd_t(this, &RightOperator::conditional);
	}

	int priority () const
	{
		return PRIORITY_CONDITIONAL;
	}

	void deploy (level_t a_current, level_t a_previous) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		>  ~(	re("\\s*<\\?\\s*")
			>	a_current
			>	pass * m_conditional
			);
	}

};


} // namespace

PLUGIN(LeftOperator, joint_conditional_left,
		nparse.script.joints.ConditionalLeft)
PLUGIN(RightOperator, joint_conditional_right,
		nparse.script.joints.ConditionalRight)
