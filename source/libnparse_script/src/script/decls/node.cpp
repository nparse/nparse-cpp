/*
 * @file $/source/libnparse_script/src/script/decls/node.cpp
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
#include <algorithm>
#include <functional>
#include <boost/lexical_cast.hpp>
#include <nparse/nparse.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/not.hpp>
#include "../../tokenizer.hpp"
#include "../../static.hpp"
#include "../_punct.hpp"

namespace {

using namespace nparse;

class Construct: public IConstruct
{
//	<LOCAL_PARSER_DATA>
	enum int_label_t
	{
		SOURCE_NODE = 0,
		ARC_TYPE,
		ACCEPTOR,
		SUBNETWORK,
		TARGET_NODE,
		ARC_LABEL,
		LAST_INDEX

	};

	std::vector<string_t> m_params;
	int m_param_index;
	anta::ndl::Node<NLG>* m_source_node;
	typedef std::vector<action_pointer> actions_t;
	actions_t m_actions;

//	</LOCAL_PARSER_DATA>

	// Select a parameter to be assigned.
	bool select_param (const hnd_arg_t& arg)
	{
		m_param_index = get_label_int(arg);
		return true;
	}

	// Assign a value to the previously selected parameter.
	bool assign_param (const hnd_arg_t& arg)
	{
		m_params[m_param_index] = Tokenizer::decode(get_accepted_range(arg));
		return true;
	}

	// Create (or pick) the source node.
	bool create_node (const hnd_arg_t& arg)
	{
		m_source_node =& arg. staging. cluster(m_params[SOURCE_NODE]);
		attach_actions();
		return true;
	}

	// Create an arc from the source node to another node and clear the
	// parameter list.
	bool create_arc (const hnd_arg_t& arg)
	{
		if (m_params[SUBNETWORK]. empty())
		{
			(*m_source_node)
				(	anta::ndl::Node<NLG>::link_type
					(	arg. staging. cluster(m_params[TARGET_NODE])
					,	make_arc_type(m_params[ARC_TYPE])
					)
				,	arg. staging. acceptor(m_params[ACCEPTOR])
				,	make_arc_label(m_params[ARC_LABEL])
				);
		}
		else
		{
			(*m_source_node)
				(	anta::ndl::Node<NLG>::link_type
					(	arg. staging. cluster(m_params[TARGET_NODE])
					,	make_arc_type(m_params[ARC_TYPE])
					)
				,	arg. staging. cluster(m_params[SUBNETWORK])
				,	make_arc_label(m_params[ARC_LABEL])
				);
		}
		std::for_each(m_params. begin(), m_params. end(),
				std::mem_fun_ref(&string_t::clear));
		attach_actions();
		return true;
	}

	// Attach the collected actions to the last created element of the network.
	void attach_actions ()
	{
		if (m_actions. empty())
			return ;
		for (actions_t::iterator i = m_actions. begin(); i != m_actions. end();
				++ i)
		{
			(*m_source_node)[*i];
		}
		m_actions. clear();
	}

	// Convert the symbolic designation of an arc specifier to the internal
	// type.
	static anta::arc_type_t make_arc_type (const string_t& a_def)
	{
		switch (a_def. empty() ? '>' : encode::make<char>::from(a_def[0]))
		{
		case '>': return anta::atSimple;
		case '&': return anta::atInvoke;
		case '@': return anta::atExtend;
		case '?': return anta::atPositive;
		case '!': return anta::atNegative;
		default: break;
		}
		assert(false);
		return anta::atSimple;
	}

	// Convert the symbolic designation of an arc label to the internal type.
	static anta::Label<NLG> make_arc_label (const string_t& a_def)
	{
		int label = 1;
		if (! a_def. empty())
			label = boost::lexical_cast<int>(encode::unwrap(a_def));
		return anta::Label<NLG>(label);
	}

	// Attach semantic action to the last defined node or branch.
	bool define_action (const hnd_arg_t& arg)
	{
		assert(m_actions. empty());
		arg. staging. swap(m_actions);
		return true;
	}

	plugin::instance<IConstruct> m_block;
	anta::ndl::Rule<SG> node_name_, acceptor_, arc_type_, arc_label_, entry_;

public:
	Construct ():
		m_params (LAST_INDEX), m_param_index (-1), m_source_node (NULL),
		m_block ("nparse.script.constructs.Block"),
// <DEBUG_NODE_NAMING>
		node_name_	("Node.NodeName"),
		acceptor_	("Node.Acceptor"),
		arc_type_	("Node.ArcType"),
		arc_label_	("Node.ArcLabel"),
		entry_		("Node.Entry")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		anta::Label<SG>
			doSelectParam	= hnd_t(this, &Construct::select_param),
			doAssignParam	= hnd_t(this, &Construct::assign_param),
			doCreateNode	= hnd_t(this, &Construct::create_node),
			doCreateArc		= hnd_t(this, &Construct::create_arc),
			doDefineAction	= hnd_t(this, &Construct::define_action);

		node_name_ = token<Separator>(PUNCT ",", true) * doAssignParam;
		acceptor_ = token<Separator>(">&@?!", false) * doAssignParam;
		arc_type_ = set(">&@?!") * doAssignParam;
		arc_label_ = +digit * doAssignParam;

// a helper macro for the complex label definitions
#define L(x) anta::Label<SG>(#x, x)

		// @todo: remove abusive negation operator
		entry_ =
		   ~(	"node"
			>  +space
			)
		>	(
				node_name_					* L(SOURCE_NODE)
			>  ~(	space
				>	m_block -> entry()
				>	pass * doDefineAction
				)
			>	pass * doCreateNode
			>  ~(	space
				>	':' > !lit('=')
				>	(	space
					>	(	Not<SG, Symbol>('$')
						>	acceptor_		* L(ACCEPTOR)
						|	'$'
						>	node_name_		* L(SUBNETWORK)
						)
					>	space
					>	arc_type_			* L(ARC_TYPE)
					>	node_name_			* L(TARGET_NODE)
					>  ~(	space
						>	'*'
						>	space
						>	arc_label_		* L(ARC_LABEL)
						)
					>  ~(	space
						>	m_block -> entry()
						>	pass * doDefineAction
						)
					>	pass * doCreateArc
					) % (
						space
					>	','
					)
				)
			) * doSelectParam; // attaches action to labelled elements

#undef L
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, decl_node, nparse.script.decls.Node)
