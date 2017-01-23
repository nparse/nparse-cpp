/*
 * @file $/include/anta/ndl/node.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.6

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
#ifndef ANTA_NDL_NODE_HPP_
#define ANTA_NDL_NODE_HPP_

#include <utility/gag.hpp>

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	Default type for the name of a node.
 */
template <typename M_>
struct node_name
{
	typedef typename string<M_>::type type;

};

/**
 *	Customized node class.
 */
template <typename M_>
class Node: public anta::Node<M_>
{
	using anta::Node<M_>::m_arcs;

public:
	/**
	 *	The default constructor.
	 */
	Node ():
		m_action_receiver (NULL)
	{
	}

	/**
	 *	The naming constructor.
	 */
	Node (const typename node_name<M_>::type& a_name):
		m_action_receiver (NULL)
	{
		// NOTE: See the anta::Node prototype specialization for the model.
		//		 (./prototypes.cpp : 90)
		this -> set_name(a_name);
	}

	/**
	 *	The naming constructor for string constants.
	 */
	Node (const typename character<M_>::type* a_name):
		m_action_receiver (NULL)
	{
		this -> set_name(a_name);
	}

public:
	//
	//	The network definition language (NDL) support.
	//

	/**
	 *	Auxiliary object that represents link of a certain type between network
	 *	nodes.
	 */
	struct link_type
	{
		const anta::Node<M_>& target_node;
		arc_type_t arc_type;

		link_type (const anta::Node<M_>& a_target_node,
				const arc_type_t a_arc_type = atSimple):
			target_node (a_target_node), arc_type (a_arc_type)
		{
		}

		link_type (const anta::Node<M_>* a_target_node):
			target_node (*a_target_node), arc_type (atInvoke)
		{
		}

	};

	/**
	 *	Create a positive linking object to the node.
	 */
	link_type operator+ () const
	{
		return link_type(*this, atPositive);
	}

	/**
	 *	Create a negative linking object to the node.
	 */
	link_type operator- () const
	{
		return link_type(*this, atNegative);
	}

	/**
	 *	Deploy the given linking object into an arc using the given acceptor and
	 *	arc label.
	 */
	Node& operator() (const link_type& a_jump, const Acceptor<M_>& a_acceptor,
			const Label<M_>& a_label = Label<M_>())
	{
		m_action_receiver =& this -> link(a_jump. target_node, a_acceptor,
				a_jump. arc_type, a_label);
		return *this;
	}

	/**
	 *	Deploy the given linking object into an arc using the unconditional
	 *	repeating (or not repeating) acceptor and the given arc label.
	 *	@{
	 */
	Node& operator() (const link_type& a_jump, const bool a_repeating = true,
			const Label<M_>& a_label = Label<M_>())
	{
		m_action_receiver =& this -> link(a_jump. target_node,
				unconditional<M_>(a_repeating), a_jump. arc_type, a_label);
		return *this;
	}

	Node& operator() (const link_type& a_jump, const Label<M_>& a_label,
			const bool a_repeating = true)
	{
		m_action_receiver =& this -> link(a_jump. target_node,
				unconditional<M_>(a_repeating), a_jump. arc_type, a_label);
		return *this;
	}
	/**	}@ */

public:
	//
	//	NDL extension support: parentheses operator.
	//

	/**
	 *	Deploy the given linking object into an arc using a subnetwork call
	 *	instead of the acceptor.
	 */
	Node& operator() (const link_type& a_jump, const Node& a_sub_entry,
			const Label<M_>& a_label = Label<M_>())
	{
		// Allocate an inner auxiliary node.
		m_auxiliary_nodes. push_back(Node());
		Node& aux = m_auxiliary_nodes. back();
		// Create an arc from the source node to the auxiliary node instead of
		// the target node.
		this -> link(aux, unconditional<M_>(false), a_jump. arc_type);
		// Create a caller arc from the auxiliary node to the subnetwork entry
		// node.
		aux. link(a_sub_entry, unconditional<M_>(), atExtend);
		if (a_jump. arc_type == atSimple)
		{
			/* If the target node is linked using an arc of simple type then we
			 * simply create an additional arc from the auxiliary node to the
			 * target node.
			 *
			 *	In terms of NDL the result structure is as follows below:
			 *		SOURCE_NODE		...
			 *						(   AUXILIARY_NODE )
			 *						... ;
			 *		AUXILIARY_NODE	( & SUBNETWORK_ENTRY )
			 *						(   TARGET_NODE );	// single additional arc
			 */
			m_action_receiver =& aux. link(a_jump. target_node,
					unconditional<M_>(), atSimple, a_label);
		}
		else
		{
			/* Otherwise, if the target node is linked using an arc of any of
			 * the functional types, i.e. invoke, extend, positive or negative,
			 * then we have to create two additional arcs from the auxiliary
			 * node because the last arc in a bunch is required to be simple.
			 * The second additional arc links the auxiliary node with an empty
			 * node that acts as a point of return.
			 *
			 *	In terms of NDL the result structure is as follows below:
			 *		SOURCE_NODE		...
			 *						( % AUXILIARY_NODE )
			 *						... ;
			 *		AUXILIARY_NODE	( & SUBNETWORK_ENTRY )
			 *						( & TARGET_NODE )	// first additional arc
			 *						(   RETURN );		// second additional arc
			 *
			 *	In the scheme above the symbol '%' designates a call of the
			 *	given type.
			 */
			m_action_receiver =& aux. link(a_jump. target_node,
					unconditional<M_>(), atInvoke, a_label);
			//
			static const Node sc_return = Node();
			aux. link(sc_return, unconditional<M_>(), atSimple);
		}
		return *this;
	}

	/**
	 *	Quick semantic action: assigns the current accepted range to the
	 *	specified trace variable.
	 */
	Node& operator() (const typename context_key<M_>::type& a_key)
	{
		return (*this)[ action_assign<M_, delta<M_> >(a_key, delta<M_>()) ];
	}

	/**
	 *	Quick semantic action: assigns the given constant value to the specified
	 *	trace variable.
	 *	@{ */
	Node& operator() (const typename context_key<M_>::type& a_key,
			const typename context_value<M_>::type& a_value)
	{
		return (*this)
			[ action_assign<M_, value<M_> >(a_key, value<M_>(a_value)) ];
	}

	Node& operator() (const typename context_key<M_>::type& a_key,
			const typename character<M_>::type* a_value)
	{
		return (*this)
			[ action_assign<M_, value<M_> >(a_key, value<M_>(a_value)) ];
	}
	/**	@} */

	/**
	 *	Quick semantic action: assigns the value of the given selector to the
	 *	specified trace variable.
	 */
	template <typename Selector_>
	Node& operator() (const typename context_key<M_>::type& a_key,
			const Selector_& a_selector)
	{
		return (*this)[ action_assign<M_, Selector_>(a_key, a_selector) ];
	}

public:
	//
	//	NDL extension support: square brackets operator.
	//

	/**
	 *	The operator for attaching custom semantic actions to the node itself or
	 *	to the last of its arcs.
	 */
	Node& operator[] (const typename action<M_>::type& a_action)
	{
		// NOTE: At the very beginning the receiver of the next semantic action
		//		 is the node itself.
		(m_action_receiver ? m_action_receiver : this) -> attach(a_action);
		return *this;
	}

private:
	// Object that receives the next semantic action to be attached to the node.
	ActionPerformer<M_>* m_action_receiver;
	// List of auxiliary nodes.
	std::list<Node> m_auxiliary_nodes;

#if defined(ANTA_NDL_SUBNODES)
private:
	// List of numbered subnodes.
	typedef std::map<uint_t, Node*> subnodes_t;
	subnodes_t m_subnodes;

public:
	/**
	 *	Get a reference to a numbered subnode.
	 */
	Node& operator[] (const uint_t a_number)
	{
		typename subnodes_t::iterator found_at = m_subnodes. find(a_number);
		if (found_at == m_subnodes. end())
		{
			// Append an auxiliary node.
			m_auxiliary_nodes. push_back(Node());
			// Register the pointer to the node within the inner subnode map.
			const std::pair<typename subnodes_t::iterator, bool> p =
				m_subnodes. insert(typename subnodes_t::value_type(a_number,
							& m_auxiliary_nodes. back()));
			assert(p. second);
			found_at = p. first;
			// Set the derivative name for the auxiliary node.
			std::basic_stringstream<typename character<M_>::type> tmp;
			tmp << this -> get_name() << a_number;
			found_at -> second -> set_name(tmp. str());
		}
		return *(found_at -> second);
	}
#endif // ANTA_NDL_SUBNODES

};

/******************************************************************************/

}} // namespace anta::ndl

#include <utility/gag.hpp>

#endif /* ANTA_NDL_NODE_HPP_ */
