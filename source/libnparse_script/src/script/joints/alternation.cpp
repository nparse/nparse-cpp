/*
 * @file $/source/libnparse_script/src/script/joints/alternation.cpp
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
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include "../../static.hpp"
#include "_priority.hpp"
#include "_joints.hpp"

namespace {

using namespace nparse;

class Operator: public IOperator
{
	bool compose (const hnd_arg_t& arg)
	{
		const IStaging::joint_pointer joint1 = arg. staging. popJoint();
		const IStaging::joint_pointer joint0 = arg. staging. popJoint();

		IStaging::joint_pointer joint;
		switch
			(	((joint0 -> get_flavor(0) != joint1 -> get_flavor(0)) ? 1 : 0)
			|	((joint0 -> get_flavor(1) != joint1 -> get_flavor(1)) ? 2 : 0)
			)
		{
		case 0: // both sides have equal flavors
			joint(
				*joint0 | *joint1
			);
			joint -> set_flavor(0, joint0 -> get_flavor(0));
			joint -> set_flavor(1, joint0 -> get_flavor(1));
			break;

		case 1: // left side has different flavors
			joint(
				*fold_left(joint0) | *fold_left(joint1)
			);
			joint -> set_flavor(1, joint0 -> get_flavor(1));
			break;

		case 2: // right side has different flavors
			joint( 
				*fold_right(joint0) | *fold_right(joint1)
			);
			joint -> set_flavor(0, joint0 -> get_flavor(0));
			break;

		case 3: // both sides have different flavors
			joint( 
				*fold(joint0) | *fold(joint1)
			);
			break;
		}

		arg. staging. pushJoint(joint);
		return true;
	}

public:
	Operator ()
	{
		m_compose = hnd_t(this, &Operator::compose);
	}

protected:
	anta::Label<SG> m_compose;

};

// Local alternation with lower priority.
class AlternationLocal: public Operator
{
public:
	int priority () const
	{
		return PRIORITY_ALTERNATION_LC;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		a_current =
			a_previous
		>  *(space > '|' > space > a_previous > pass * m_compose);
	}

};

// Global alternation with higher priority.
class AlternationGlobal: public Operator
{
public:
	int priority () const
	{
		return PRIORITY_ALTERNATION_GL;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;

		a_current =
			a_previous
		>  *(space > "||" > space > a_previous > pass * m_compose);
	}

};

} // namespace

PLUGIN(AlternationLocal,
		joint_alternation_lc, nparse.script.joints.AlternationLocal)

PLUGIN(AlternationGlobal,
		joint_alternation_gl, nparse.script.joints.AlternationGlobal)
