/*
 * @file $/include/anta/core/node.hpp
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
#ifndef ANTA_CORE_NODE_HPP_
#define ANTA_CORE_NODE_HPP_

namespace anta {

/******************************************************************************/

/**
 *	Node. An object that is used to define the primary elements of a network.
 */
template <typename M_>
class Node: public Base<Node<M_>, M_>, public Entangled<M_>
{
public:
	typedef std::vector<Arc<M_>*> arcs_type;

	/**
	 *	Auxiliary arc enumerator. Due to pool ecomony each of class instances
	 *	contains only one iterator because the last arc in an arc list of a node
	 *	is always null, therefore the end of a bunch can always be detected.
	 */
	class bunch_type
	{
	public:
		bunch_type (const typename arcs_type::const_iterator& a_iterator):
			m_iterator (a_iterator)
		{
		}

		bool get (const Arc<M_>*& a_arc)
		{
			if (*m_iterator != NULL)
			{
				a_arc = *(m_iterator ++);
				return true;
			}
			return false;
		}

	private:
		typename arcs_type::const_iterator m_iterator;

	};

public:
	//
	//	General purpose elements.
	//

	/**
	 *	The only constructor.
	 */
	Node ()
	{
		// Initially append a null arc to the arc list.
		m_arcs. push_back(NULL);
	}

	/**
	 *	The destructor.
	 */
	~Node ()
	{
		utility::free_all(m_arcs);
	}

	/**
	 *	Create an arc between two nodes.
	 */
	Arc<M_>& link (const Node<M_>& a_target_node,
			const Acceptor<M_>& a_acceptor, const arc_type_t a_arc_type,
			const Label<M_>& a_label = Label<M_>())
	{
		// A new arc is appended before the null arc located at the end of the
		// node's arc list.
		return **m_arcs. insert(m_arcs. begin() + (m_arcs. size() - 1),
			new Arc<M_>(a_target_node, a_acceptor, a_arc_type, a_label));
	}

public:
	//
	//	ANTA implementation specific elements.
	//

	/**
	 *	Check whether the node is final, i.e. contains no arcs except for the
	 *	null arc at the end of the list.
	 */
	bool is_final () const
	{
		// NOTE: Again, the null arc is not being taken into account.
		return (m_arcs. size() == 1);
	}

	/**
	 *	Get the bunch (which is the arc enumerator).
	 */
	bunch_type get_bunch () const
	{
		return bunch_type(m_arcs. begin());
	}

protected:
	arcs_type m_arcs;

};

/******************************************************************************/

} // namespace anta

#endif /* ANTA_CORE_NODE_HPP_ */
