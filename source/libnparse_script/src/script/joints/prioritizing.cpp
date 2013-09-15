/*
 * @file $/source/libnparse_script/src/script/joints/prioritizing.cpp
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
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/dsel/rt/assign.hpp>
#include <anta/dsel/rt/comma.hpp>
#include <anta/dsel/rt/pre_inc.hpp>
#include "../../static.hpp"
#include "_priority.hpp"
#include "_joints.hpp"

namespace {

using namespace nparse;

/**
 *	An auxiliary object that represents collector node links.
 */
class ColLink: public anta::ndl::LinkJump<NLG>
{
public:
	// Overridden from LinkJump<>:

	anta::Arc<NLG>& deploy (ndl::Generator<NLG>& a_generator,
			const uint_t a_node_from, uint_t& a_node_counter)
	{
		const anta::uint_t eid = get_entanglement();
		set_entanglement(0);
		anta::Arc<NLG>& arc = anta::ndl::LinkJump<NLG>::deploy(
				a_generator, a_node_from, a_node_counter);
		set_entanglement(eid);
		const_cast<anta::Node<NLG>&>(arc. get_target()). set_entanglement(eid);
		return arc;
	}

public:
	ColLink (const anta::ndl::Bunch<NLG>& a_bunch, const anta::uint_t a_eid):
		anta::ndl::LinkJump<NLG> (a_bunch, anta::unconditional<NLG>(),
				anta::atSimple, anta::Label<NLG>())
	{
		set_entanglement(a_eid);
	}

};

class Operator: public IOperator
{
	bool compose (const hnd_arg_t& arg)
	{
		// Determine the operator arity.
		const anta::aux::integer<SG>::type pr =
			arg. state. val("pr_arity"). as_integer();
		assert( pr > 1 );

		// Pop joints out of the LL stack.
		typedef std::vector<IStaging::joint_pointer> joints_t;
		joints_t joints;
		joints. reserve(pr);
		for (int n = static_cast<int>(pr); n; -- n)
		{
			// @todo: folding needs optimization!
			joints. push_back(arg. staging. popJoint());
		}

		// Allocate entanglement identifier.
		static anta::uint_t last_id = 0;
		const anta::uint_t eid = ++ last_id;

		// Create necessary bunches.
		anta::ndl::Bunch<NLG> entry, collector, exit;
		collector -> push_back(new ColLink(exit, eid));

		// Merge popped joints.
		anta::ndl::BunchSet<NLG> bs;
		for (joints_t::iterator j = joints. begin(); j != joints. end(); ++ j)
		{
			(*j) -> entry() -> replace((*j) -> exit(), collector, bs);
			entry -> merge((*j) -> entry());
			bs. clear();
		}

		// Alter links.
		anta::uint_t p = static_cast<anta::uint_t>(pr + 1);
		for (anta::ndl::BunchImpl<NLG>::iterator i = entry -> begin();
				i != entry -> end(); ++ i)
		{
			(*i) -> set_priority(-- p);
			(*i) -> set_entanglement(eid);
		}

		// Push a new joint to the LL stack.
		IStaging::joint_pointer joint;
		joint. reset(new anta::ndl::JointSeq<NLG>(entry, exit));
		joint -> set_flavor(0, 1);
		joint -> set_flavor(1, 1);
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
		return PRIORITY_PRIORITIZING;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		using namespace anta::dsel;
		using boost::proto::lit;

		reference<SG>::type pr = ref<SG>("pr_arity");

		a_current =
			a_previous
		>	space
		>  ~(	lit("|?")[ pr = 1, true ]
			>	(	space[ push(pr), true ]
				>	a_previous
				>	space[ pop(pr), ++ pr, true ]
				) % "|?"
			>	pass * m_compose
			);
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Operator, joint_prioritizing, nparse.script.joints.Prioritizing, 1 )
