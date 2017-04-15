/*
 * @file $/source/libnparse_script/src/script/joints/postfix.cpp
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
#include <anta/sas/symbol.hpp>
#include <anta/sas/regex.hpp>
#include "../_varname.hpp"
#include "../_action_range.hpp"
#include "_joints.hpp"
#include "_priority.hpp"
#include "../../static.hpp"

namespace {

using namespace nparse;

class Operator: public IOperator
{
	bool kleene_star (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();
		IStaging::joint_pointer joint(
			* *fold_left(joint0)
		);
	//	joint -> set_flavor(0, 0);
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	bool kleene_plus (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();
		IStaging::joint_pointer joint(
			+ *fold_left(joint0)
		);
	//	joint -> set_flavor(0, 0);
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	bool omission (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();
		IStaging::joint_pointer joint(
			~ *joint0
		);
		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	bool assignment (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();
		IStaging::joint_pointer joint(
			(*joint0) [ new ActionRange(get_accepted_str(arg)) ]
		);
		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	bool labelling (const hnd_arg_t& arg)
	{
		const int label = boost::lexical_cast<int>(
			encode::wstring(get_accepted_str(arg)));
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();
		IStaging::joint_pointer joint(
			*joint0 * anta::Label<NLG>(label)
		);
		joint -> set_flavor(0, joint0 -> get_flavor(0));
		joint -> set_flavor(1, joint0 -> get_flavor(1));
		arg. staging. pushJoint(joint);
		return true;
	}

	anta::Label<SG> m_kleene_star, m_kleene_plus, m_omission, m_assignment,
		m_labelling;

public:
	Operator ()
	{
		m_kleene_star = hnd_t(this, &Operator::kleene_star);
		m_kleene_plus = hnd_t(this, &Operator::kleene_plus);
		m_omission = hnd_t(this, &Operator::omission);
		m_assignment = hnd_t(this, &Operator::assignment);
		m_labelling = hnd_t(this, &Operator::labelling);
	}

public:
	int priority () const
	{
		return PRIORITY_POSTFIX;
	}

	void deploy (level_t a_current, level_t a_previous) const
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		a_current =
			a_previous
		>  *(	re("\\s*\\+\\s*") * m_kleene_plus
			|	re("\\s*\\?\\s*") * m_omission
			|	re("\\s*:\\s*") > varName * m_assignment
			|	re("\\s*(\\*)\\s*(?![-+]?\\d+)") * m_kleene_star
			|	re("\\s*(\\*)\\s*(?=[-+]?\\d+)")
					> re("[-+]?\\d+") * m_labelling
			);
	}

};

} // namespace

PLUGIN(Operator, joint_postfix, nparse.script.joints.Postfix)
