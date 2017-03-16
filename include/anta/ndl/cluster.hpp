/*
 * @file $/include/anta/ndl/cluster.hpp
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
#ifndef ANTA_NDL_CLUSTER_HPP_
#define ANTA_NDL_CLUSTER_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	The Cluster template class is used for generation of acceptor network
 *	clusters (that basically are subnetworks with indexed nodes) from
 *	determinative operator-based expressions.
 */
template <typename M_>
class Cluster: public Generator<M_>, public Node<M_>
{
	typedef boost::ptr_vector<Node<M_> > nodes_type;

public:
	/**
	 *	The default constructor.
	 */
	Cluster ():
		m_start_index (0)
	{
	}

	/**
	 *	The naming constructor.
	 */
	Cluster (const typename node_name<M_>::type& a_name):
		Node<M_> (a_name), m_start_index (0)
	{
	}

	/**
	 *	The generating constructor.
	 */
	Cluster (const JointBase<M_>& a_joint):
		m_start_index (0)
	{
		*this = a_joint;
	}

	/**
	 *	The generating assignment operator.
	 */
	uint_t operator= (const JointBase<M_>& a_joint)
	{
		return this->generate(a_joint, m_start_index = m_nodes. size());
	}

private:
	/**
	 *	Get a reference to a local (numbered) node.
	 */
	Node<M_>& node (const typename nodes_type::size_type a_node_index)
	{
		if (a_node_index == m_start_index)
		{
			return *this;
		}

		if (a_node_index - 1 >= m_nodes. size())
		{
			assert(a_node_index - 1 == m_nodes. size());
			m_nodes. push_back(new Node<M_>());
		}

		return m_nodes[a_node_index - 1];
	}

private:
	// Overridden from Generator<M_>:

	/**
	 *	Create an arc between two local (numbered) nodes.
	 */
	Arc<M_>& link (const uint_t a_node_from, const uint_t a_node_to,
			const Acceptor<M_>& a_acceptor, const arc_type_t a_arc_type,
			const Label<M_>& a_label)
	{
		return node(a_node_from). link(node(a_node_to), a_acceptor, a_arc_type,
				a_label);
	}

	/**
	 *	Create an arc between a local (numbered) node and an outer node.
	 */
	Arc<M_>& link (const uint_t a_node_from, const ::anta::Node<M_>& a_node_to,
			const Acceptor<M_>& a_acceptor, const arc_type_t a_arc_type,
			const Label<M_>& a_label)
	{
		return node(a_node_from). link(a_node_to, a_acceptor, a_arc_type,
				a_label);
	}

public:
	/**
	 *	The operator for attaching initial semantic actions.
	 */
	Cluster& operator[] (const typename action<M_>::type& a_action)
	{
		Node<M_>::operator[](a_action);
		return *this;
	}

public:
	/**
	 *	Get total node count including the cluster itself.
	 */
	uint_t get_node_count () const
	{
		return static_cast<uint_t>(m_nodes. size() + 1);
	}

	/**
	 *	Get a constant reference to a local node.
	 */
	const Node<M_>& get_node (const uint_t a_node_index) const
	{
		if (! a_node_index)
			return *this;
		else
			return m_nodes[a_node_index-1];
	}

private:
	nodes_type m_nodes;
	uint_t m_start_index;

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_CLUSTER_HPP_ */
