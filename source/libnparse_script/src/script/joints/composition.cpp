/*
 * @file $/source/libnparse_script/src/script/joints/composition.cpp
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
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include "../../static.hpp"
#include "_priority.hpp"
#include "_joints.hpp"

namespace {

using namespace nparse;

class Operator: public IOperator
{
	bool enumeration (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint1 = arg. staging. popJoint();
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();

		if (joint0 -> is_virtual() || joint1 -> is_virtual())
		{
			throw ex::syntax_error()
				<< ex::location(get_accepted_range(arg))
				<< ex::message("enumeration with a sequencing modifier");
		}

		const int
			l = merge_flavors(joint0 -> get_flavor(1), joint1 -> get_flavor(0)),
			r = merge_flavors(joint1 -> get_flavor(1), joint0 -> get_flavor(0));

		IStaging::joint_pointer joint;
		switch ((l << 4) | r)
		{
		case 0x00: // no separator on either side
			joint( *joint0 % *joint1 );
			break;

		case 0x01: // no separator before the delimiter, optional after
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_optional(). entry(),
				space_optional(). entry(),
				0 | 2
			));
			break;

		case 0x02: // no separator before the delimiter, required after
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_required(). entry(),
				space_required(). entry(),
				0 | 2
			));
			break;

		case 0x10: // optional before, no after
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_optional(). entry(),
				space_optional(). entry(),
				1 | 0
			));
			break;

		case 0x11: // both optional
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_optional(). entry(),
				space_optional(). entry(),
				1 | 2
			));
			break;

		case 0x12: // optional before, required after
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_optional(). entry(),
				space_required(). entry(),
				1 | 2
			));
			break;

		case 0x20: // required before, no after
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_required(). entry(),
				space_required(). entry(),
				1 | 0
			));
			break;

		case 0x21: // required before, optional after
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_required(). entry(),
				space_optional(). entry(),
				1 | 2
			));
			break;

		case 0x22: // both required
			joint. reset(new anta::ndl::JointEnum<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_required(). entry(),
				space_required(). entry(),
				1 | 2
			));
			break;

		default:
			throw std::logic_error("invalid separator merge");
		}

		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint0 -> get_flavor(1));

		arg. staging. pushJoint(joint);
		return true;
	}

	bool permutation (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint1 = arg. staging. popJoint();
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();

		if (joint0 -> is_virtual() || joint1 -> is_virtual())
		{
			throw ex::syntax_error()
				<< ex::location(get_accepted_range(arg))
				<< ex::message("permutation with a sequencing modifier");
		}

		IStaging::joint_pointer joint;
		switch (merge_flavors(joint0 -> get_flavor(1), joint1 -> get_flavor(0)))
		{
		case 0: // no separator
			joint( *joint0 ^ *joint1 );
			break;

		case 1: // optional separator
			joint. reset(new anta::ndl::JointPerm<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_optional(). entry()
			));
			break;

		case 2: // required separator
			joint. reset(new anta::ndl::JointPerm<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_required(). entry()
			));
			break;
		}

		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint1 -> get_flavor(1));

		arg. staging. pushJoint(joint);
		return true;
	}

	bool affixation (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint1 = arg. staging. popJoint();
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();

		if (joint0 -> is_virtual() || joint1 -> is_virtual())
		{
			throw ex::syntax_error()
				<< ex::location(get_accepted_range(arg))
				<< ex::message("affixation with a sequencing modifier");
		}

		IStaging::joint_pointer joint;
		switch (merge_flavors(joint0 -> get_flavor(1), joint1 -> get_flavor(0)))
		{
		case 0: // no separator
			joint( *joint0 / *joint1 );
			break;

		case 1: // optional separator
			joint. reset(new anta::ndl::JointAffx<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_optional(). entry()
			));
			break;

		case 2: // required separator
			joint. reset(new anta::ndl::JointAffx<NLG>(
				joint0 -> entry(),
				joint1 -> entry(),
				space_required(). entry()
			));
			break;
		}

		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint1 -> get_flavor(1));

		arg. staging. pushJoint(joint);
		return true;
	}

	anta::Label<SG> m_permutation, m_affixation, m_enumeration;

public:
	Operator ()
	{
		m_enumeration = hnd_t(this, &Operator::enumeration);
		m_permutation = hnd_t(this, &Operator::permutation);
		m_affixation = hnd_t(this, &Operator::affixation);
	}

	int priority () const
	{
		return PRIORITY_COMPOSITION;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		>  *(	space
			>	(	'^' > +space > a_previous > pass * m_permutation
				|	'%' > space > a_previous > pass * m_enumeration
				|	'/' > space > a_previous > pass * m_affixation
				)
			);
	}

};

} // namespace

PLUGIN(Operator, joint_composition, nparse.script.joints.Composition)
