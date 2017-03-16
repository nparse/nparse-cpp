/*
 * @file $/include/anta/core/arc.hpp
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
#ifndef ANTA_CORE_ARC_HPP_
#define ANTA_CORE_ARC_HPP_

namespace anta {

/******************************************************************************/

/**
 *	Recognizable arc types.
 */
enum arc_type_t
{
	atSimple,
	atInvoke,
	atExtend,
	atPositive,
	atNegative
};

// (forward declaration)
template <typename M_> class Node;

/**
 *	Arc. An object that is used to define connections between nodes (which are
 *	the primary elements) in a network.
 */
template <typename M_>
class Arc: public Base<Arc<M_>, M_>, public Entangled<M_>
{
public:
	/**
	 *	The only constructor.
	 */
	Arc (const Node<M_>& a_target, const Acceptor<M_>& a_acceptor,
			const arc_type_t a_type, const Label<M_>& a_label):
		m_target (&a_target), m_acceptor (&a_acceptor), m_type (a_type),
		m_label (a_label)
	{
	}

	/**
	 *	Get a constant reference to the target node of the arc.
	 */
	const Node<M_>& get_target () const
	{
		return *m_target;
	}

	/**
	 *	Get a constant reference to the acceptor associated with the arc.
	 */
	const Acceptor<M_>& get_acceptor () const
	{
		return *m_acceptor;
	}

	/**
	 *	Get the type of the arc.
	 */
	arc_type_t get_type () const
	{
		return m_type;
	}

	/**
	 *	Get the label of the arc.
	 */
	const Label<M_>& get_label () const
	{
		return m_label;
	}

private:
	const Node<M_>* m_target;
	const Acceptor<M_>* m_acceptor;
	arc_type_t m_type;
	Label<M_> m_label;

};

/******************************************************************************/

} // namespace anta

#endif /* ANTA_CORE_ARC_HPP_ */
