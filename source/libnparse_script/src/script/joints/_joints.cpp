/*
 * @file $/source/libnparse_script/src/script/joints/_joints.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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

namespace nparse {

int merge_flavors (const int a_left, const int a_right)
{
	if (a_left == 1)
		return a_right;
	else
	if (a_right == 1)
		return a_left;
	else
		return std::max(a_left, a_right);
}

anta::ndl::JointJump<NLG> space_optional ()
{
	static const anta::sas::Test<NLG> space(
		std::isspace, anta::sas::tfGreedy | anta::sas::tfAcceptEmpty);
	return anta::ndl::JointJump<NLG>(space, anta::Label<NLG>());
}

anta::ndl::JointJump<NLG> space_required ()
{
	static const anta::sas::Test<NLG> space(
		std::isspace, anta::sas::tfGreedy);
	return anta::ndl::JointJump<NLG>(space, anta::Label<NLG>());
}

IStaging::joint_pointer fold_left (const IStaging::joint_pointer& a_joint)
{
	IStaging::joint_pointer joint;

	switch (a_joint -> get_flavor(0))
	{
	case 0: // no separator
		return a_joint;
	case 1: // optional separator
		joint( space_optional() > *a_joint );
		break;
	case 2: // required separator
		joint( space_required() > *a_joint );
		break;
	}

	joint -> set_flavor(1, a_joint -> get_flavor(1));
	return joint;
}

IStaging::joint_pointer fold_right (const IStaging::joint_pointer& a_joint)
{
	IStaging::joint_pointer joint;

	switch (a_joint -> get_flavor(1))
	{
	case 0: // no separator
		return a_joint;
	case 1: // optional separator
		joint( *a_joint > space_optional() );
		break;
	case 2: // required separator
		joint( *a_joint > space_required() );
		break;
	}

	joint -> set_flavor(0, a_joint -> get_flavor(0));
	return joint;
}

IStaging::joint_pointer fold (const IStaging::joint_pointer& a_joint)
{
	return fold_right(fold_left(a_joint));
}

} // namespace nparse
