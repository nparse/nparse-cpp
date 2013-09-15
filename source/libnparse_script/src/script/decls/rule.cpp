/*
 * @file $/source/libnparse_script/src/script/decls/rule.cpp
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
#include <algorithm>
#include <plugin/category.hpp>
#include <nparse/nparse.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/once.hpp>
#include <anta/dsel/rt/assign.hpp>
#include <anta/dsel/rt/comma.hpp>
#include "../../tokenizer.hpp"
#include "../../static.hpp"
#include "../_punct.hpp"
#include "../joints/_joints.hpp"

namespace {

using namespace nparse;

class Construct: public IConstruct
{
	// NOTE: In general, storing parser data locally is not a good idea because
	//		 it may lead to heavy side effects, especially if the grammar allows
	//		 local ambiguity. Though, if there's no ambiguity this technique may
	//		 work out pretty well. I'm leaving it here as an example.

//	<LOCAL PARSER DATA>
	sg_string_t m_rule_name;

//	</LOCAL PARSER DATA>

	bool set_name (const hnd_arg_t& arg)
	{
		m_rule_name = Tokenizer::decode(get_accepted_str(arg));
		return true;
	}

	bool create_rule (const hnd_arg_t& arg)
	{
		// NOTE: Due to the inner script logic, a rule cannot have right flavor
		//		 of type 1 and has 0 by default. This prevents several unobvious
		//		 side effects in structures like ( $rule % '\n' ) and similar.
		const IStaging::joint_pointer joint = arg. staging. popJoint();
		arg. staging. cluster(m_rule_name) =
			(joint -> get_flavor(1) == 2) ? *fold(joint) : *fold_left(joint);
		return true;
	}

	typedef plugin::category<IOperator> operators_t;
	operators_t m_operators;
	anta::ndl::Rule<SG> entry_;
	boost::ptr_vector<anta::ndl::Rule<SG> > levels_;
	static const std::string sc_operators_family;

public:
	Construct ():
		m_operators (sc_operators_family),
// <DEBUG_NODE_NAMING>
		entry_		("Rule.Entry")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;
		using namespace anta::dsel;
		using boost::proto::lit;

		// Sort operator category in priority ascending order.
		std::sort(m_operators. begin(), m_operators. end(), less_priority());

		// Instantiate the set of rules representing expression levels.
		levels_. resize(m_operators. size());

		// Initialize expression levels by deploying operators.
		std::stringstream tmp;
		for (operators_t::const_iterator i = m_operators. begin();
				i != m_operators. end(); ++ i)
		{
			const int index = static_cast<int>(i - m_operators. begin());
// <DEBUG_NODE_NAMING>
			tmp << "Rule.Level(" << index << ", "
				<< i -> first. substr(sc_operators_family. length() + 1) << ')';
			levels_[index]. cluster(). set_name(tmp. str());
			tmp. str("");
			tmp. clear();
// </DEBUG_NODE_NAMING>
			i -> second -> deploy(
				levels_[index],
				index ? levels_[index - 1] : levels_[index],
				levels_. back()
			);
		}

		anta::Label<SG>
			doSetName = hnd_t(this, &Construct::set_name),
			doCreateRule = hnd_t(this, &Construct::create_rule);

		entry_ =
		   ~("rule" > +space)
		>	token<Separator>(PUNCT) * doSetName
		>	space > ":=" > space > levels_. back() > pass * doCreateRule;
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

const std::string Construct::sc_operators_family("nparse.script.joints");

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Construct, decl_rule, nparse.script.decls.Rule, 1 )
