/*
 * @file $/include/anta/ndl/generator.hpp
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
#ifndef ANTA_NDL_GENERATOR_HPP_
#define ANTA_NDL_GENERATOR_HPP_

/*
 *	Generator
 *
 *	Bunch --<>-- BunchImpl
 *	BunchSet
 *
 *	LinkBase
 *	  |
 *	  |-- LinkJump
 *	  |
 *	  \-- LinkCall
 *
 *	JointBase
 *	  |
 *	  |-- JointJump
 *	  |     |
 *	  |     |-- JointCall
 *	  |     |
 *	  |     |-- JointStmt
 *	  |     |
 *	  |     |-- JointKleene
 *	  |     |
 *	  |     |-- JointEnum
 *	  |     |
 *	  |     |-- JointPerm
 *	  |     |
 *	  |     \-- JointAffx
 *	  |
 *	  \-- JointSeq
 *	        |
 *	        \-- JointAlt
 */

// NOTE: The usage of boost::container::flat_set here is strictly experimental
//		 and can be replaced with std::set if necessary.
#include <boost/container/flat_set.hpp>

namespace anta { namespace ndl {

// (forward declarations)
template <typename M_> class Bunch;
template <typename M_> class BunchImpl;
template <typename M_> class BunchSet;
template <typename M_> class LinkBase;
template <typename M_> class LinkJump;
template <typename M_> class LinkCall;
template <typename M_> class JointBase;
template <typename M_> class JointJump;
template <typename M_> class JointCall;
template <typename M_> class JointStmt;
template <typename M_> class JointKleene;
template <typename M_> class JointEnum;
template <typename M_> class JointPerm;
template <typename M_> class JointAffx;
template <typename M_> class JointSeq;
template <typename M_> class JointAlt;

/******************************************************************************/

/**
 *	Generator<M_> is a template base class for objects that generate acceptor
 *	network topologies by the process of sequential addition of new nodes and
 *	creation of arcs between them and already existing nodes.
 */
template <typename M_>
class Generator: public Base<Generator<M_>, M_>
{
public:
	/**
	 *	The polymorphic destructor.
	 */
	virtual ~Generator () {}

	/**
	 *	Create an arc between two inner network nodes.
	 *
	 *	@param	a_node_from
	 *		Source node index
	 *	@param	a_node_to
	 *		Destination node index
	 *	@param	a_acceptor
	 *		Acceptor for the new arc to be associated with
	 *	@param	a_arc_type
	 *		New arc type
	 *	@param	a_label
	 *		New arc label
	 *	@return
	 *		Reference to the created arc
	 */
	virtual Arc<M_>& link (const uint_t a_node_from, const uint_t a_node_to,
			const Acceptor<M_>& a_acceptor, const arc_type_t a_arc_type,
			const Label<M_>& a_label) = 0;

	/**
	 *	Create an arc between an inner and an outer network nodes.
	 *
	 *	@param	a_node_from
	 *		Source node index
	 *	@param	a_node_to
	 *		Destination node reference
	 *	@param	a_acceptor
	 *		Acceptor for the new arc to be associated with
	 *	@param	a_arc_type
	 *		New arc type
	 *	@param	a_label
	 *		New arc label
	 *	@return
	 *		Reference to the created arc
	 */
	virtual Arc<M_>& link (const uint_t a_node_from,
			const ::anta::Node<M_>& a_node_to, const Acceptor<M_>& a_acceptor,
			const arc_type_t a_arc_type, const Label<M_>& a_label) = 0;

protected:
	/**
	 *	Generate new or augment existing acceptor network topology from the
	 *	given joint expression.
	 *
	 *	@param	a_joint
	 *		Joint expression
	 *	@param	a_start_index
	 *		First available node index
	 *	@return
	 *		Next available node index (same as the total number of added nodes)
	 */
	uint_t generate (const JointBase<M_>& a_joint,
			const uint_t a_start_index = 0)
	{
		uint_t next_index = a_start_index;
		BunchSet<M_> processed;
		std::queue<Bunch<M_> > queue;
		queue. push(a_joint. entry());
		while (! queue. empty())
		{
			const Bunch<M_> s0 = queue. front();
			const uint_t index = s0 -> index(next_index);
			queue. pop();
			for (typename BunchImpl<M_>::iterator t = s0 -> begin();
					t != s0 -> end(); ++ t)
			{
				const Bunch<M_> s1 = (*t) -> get_bunch();
				if (s1 && processed. insert(s1. get()). second)
				{
					queue. push(s1);
				}
				(*t) -> deploy(*this, index, next_index);
			}
		}
		return next_index;
	}

};

/******************************************************************************/

/**
 *	Bunch<M_> template class is a convenience safely value-copyable wrapper for
 *	BunchImpl<M_> instance pointers with restricted instantiation options.
 */
template <typename M_>
class Bunch: public boost::shared_ptr<BunchImpl<M_> >
{
	typedef boost::shared_ptr<BunchImpl<M_> > base_type;

public:
	/**
	 *	The constructor for a normal empty bunch.
	 */
	Bunch ():
		base_type(new BunchImpl<M_>())
	{
	}

	/**
	 *	The constructor for an ephemeral bunch.
	 *
	 *	@param	a_intent
	 *		Mandatory integer value that designates the explicit intent to
	 *		create an ephemeral bunch, so it will not happen by an accident
	 */
	Bunch (const int a_intent):
		base_type()
	{
		assert(a_intent == -1);
	}

};

/******************************************************************************/

/**
 *	BunchImpl<M_> template class represents a bunch of outgoing links from a
 *	single acceptor network node.
 */
template <typename M_>
class BunchImpl: public std::list<LinkBase<M_>*>
{
	typedef std::list<LinkBase<M_>*> base_type;

public:
	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;

public:
	/**
	 *	The only constructor.
	 */
	BunchImpl ():
		m_index (0)
	{
	}

	/**
	 *	The destructor.
	 */
	~BunchImpl ()
	{
		utility::free_all(*this);
	}

	/**
	 *	Assign the next index to this bunch iff no index has been assigned yet.
	 *
	 *	@param	a_next_index
	 *		Mutable reference to the next available node index
	 *	@return
	 *		Index assigned to this bunch
	 */
	uint_t index (uint_t& a_next_index)
	{
		if (m_index == 0)
		{
			m_index = a_next_index ++;
		}
		return m_index;
	}

	/**
	 *	Replace all pointers to a particular bunch with a pointer to some other
	 *	bunch.
	 *
	 * 	@param	a_from
	 * 		Replaceable bunch reference
	 * 	@param	a_to
	 *		Replacer bunch reference
	 * 	@param	a_bs
	 *		Processed bunch set (recursive protection)
	 */
	void replace (const Bunch<M_>& a_from, const Bunch<M_>& a_to,
			BunchSet<M_>& a_bs)
	{
		if (is_recursive(a_bs))
		{
			return ;
		}
		for (iterator i = this -> begin(); i != this -> end(); ++ i)
		{
			(*i) -> replace(a_from, a_to, a_bs);
		}
	}

	/**
	 *	Merge the given bunch into this one.
	 *
	 *	@param	a_bunch
	 *		Mergeable bunch reference
	 *
	 *	NOTE: The mergeable bunch is implicitly mutable despite that it is
	 *		  passed via a constant wrapper reference.
	 */
	void merge (const Bunch<M_>& a_bunch)
	{
		this -> splice(this -> end(), *a_bunch);
		assert(a_bunch -> empty());
	}

	/**
	 *	Find a unique link to a particular bunch if it exists.
	 *
	 *	@param	a_bunch
	 *		Searcheable bunch reference
	 *	@param	a_link_ptr
	 *		Result receiver (a link pointer)
	 * 	@param	a_bs
	 *		Processed bunch set (protection)
	 *	@return
	 *		True, if the link was not found or it is unique within the target
	 *		domain (i.e. bunch and its referees), or false otherwise
	 */
	bool find_link_to (const Bunch<M_>& a_bunch, LinkBase<M_>*& a_link_ptr,
			BunchSet<M_>& a_bs) const
	{
		// If the call is recursive then this bunch is already being searched.
		if (is_recursive(a_bs))
		{
			return true;
		}
		for (const_iterator i = this -> begin(); i != this -> end(); ++ i)
		{
			if (! (*i) -> find_link_to(a_bunch, a_link_ptr, a_bs))
			{
				return false;
			}
		}
		return true;
	}

	/**
	 *	Link the given bunch with a simple arc.
	 *
	 * 	@param	a_bunch
	 * 		Bunch to link
	 * 	@param	a_acceptor
	 * 		Acceptor to be set on the new arc
	 * 	@param	a_label
	 * 		Label for the arc to be marked with
	 * 	@return
	 * 		Reference to a new link instance
	 */
	LinkBase<M_>& link (const Bunch<M_>& a_bunch,
			const Acceptor<M_>& a_acceptor = unconditional<M_>(),
			const Label<M_>& a_label = Label<M_>())
	{
		this -> push_back(new LinkJump<M_>(a_bunch, a_acceptor, atSimple,
					a_label));
		return *(this -> back());
	}

	/**
	 *	Link the given bunch with an invoking arc.
	 *
	 *	@param	a_bunch
	 *		Bunch to link
	 *	@param	a_arc_type
	 *		Arc type for the bunch to be linked with
	 *	@return
	 *		Reference to a new link instance
	 */
	LinkBase<M_>& link (const Bunch<M_>& a_bunch, const arc_type_t a_arc_type)
	{
		this -> push_back(new LinkJump<M_>(a_bunch, unconditional<M_>(false),
					a_arc_type, Label<M_>()));
		return *(this -> back());
	}

	/**
	 *	Link the given outer node with an invoking arc.
	 *
	 *	@param	a_node
	 *		Node to link
	 *	@return
	 *		Reference to a new link instance
	 */
	LinkBase<M_>& link (const ::anta::Node<M_>& a_node)
	{
		this -> push_back(new LinkCall<M_>(a_node));
		return *(this -> back());
	}

	/**
	 *	Set or amend labels of all future arcs for this bunch.
	 *
	 *	@param	a_label
	 *		Label for this bunch to be amdended with
	 *	@param	a_bs
	 *		Processed bunch set (protection)
	 */
	void set_label (const Label<M_>& a_label, BunchSet<M_>& a_bs)
	{
		if (is_recursive(a_bs))
		{
			return ;
		}
		for (iterator i = this -> begin(); i != this -> end(); ++ i)
		{
			(*i) -> set_label(a_label, a_bs);
		}
	}

private:
	/**
	 *	Check whether the call is topologically recursive, i.e. check if this
	 *	bunch has already been processed in the current context.
	 *
	 *	@param	a_bs
	 *		Processed bunch set (protection)
	 *	@return
	 *		True, if call is recursive, or false otherwise
	 */
	bool is_recursive (BunchSet<M_>& a_bs) const
	{
		if (a_bs. find(this) != a_bs. end())
		{
			return true;
		}
		else
		{
			a_bs. insert(this);
			return false;
		}
	}

	/**
	 *	Assigned node index.
	 */
	uint_t m_index;

};

/******************************************************************************/

/**
 *	BunchSet<M_> is an auxiliary container for unique BunchImpl<M_> instance
 *	pointers. It is used to keep track of already processed bunch instances
 *	during the acceptor network generation phase in order to handle circular
 *	links correctly.
 */
template <typename M_>
class BunchSet: public boost::container::flat_set<const BunchImpl<M_>*>
{
public:
	BunchSet ()
	{
	}

};

/******************************************************************************/

/**
 *	LinkBase<M_> is a template base class for generator agents that deploy links
 *	between acceptor network nodes.
 */
template <typename M_>
class LinkBase: public Base<LinkBase<M_>, M_>, public Entangled<M_>
{
public:
	/**
	 *	The only constructor.
	 *
	 *	@param	a_label
	 *		Initial future arc label
	 */
	LinkBase (const Label<M_>& a_label = Label<M_>(true)):
		m_label (a_label)
	{
	}

	/**
	 *	The polymorphic destructor.
	 */
	virtual ~LinkBase () {}

	/**
	 *	Set or amend the label of the future arc.
	 *
	 *	@param	a_label
	 *		Future arc label
	 */
	void set_label (const Label<M_>& a_label, BunchSet<M_>& a_bs)
	{
		Bunch<M_> bunch = get_bunch();
		if (bunch)
		{
			bunch -> set_label(a_label, a_bs);
		}
		m_label. amend(a_label);
	}

	/**
	 *	Get the label of the future arc.
	 *
	 * 	@return
	 *		Current value of the future arc label
	 */
	const Label<M_>& get_label () const
	{
		return m_label;
	}

	/**
	 *	Find a unique link to a particular bunch.
	 *
	 *	@param	a_bunch
	 *		Searcheable bunch reference
	 *	@param	a_link_ptr
	 *		Result receiver (a link pointer)
	 * 	@param	a_bs
	 *		Processed bunch set (protection)
	 *	@return
	 *		True, if the link was not found or it is unique within the target
	 *		domain (i.e. bunch and its referees), or false otherwise
	 */
	bool find_link_to (const Bunch<M_>& a_bunch, LinkBase<M_>*& a_link_ptr,
			BunchSet<M_>& a_bs) // NOTE: non-constant function
	{
		Bunch<M_> bunch = get_bunch();
		if (bunch != a_bunch)
		{
			if (bunch)
			{
				// Target bunch is set, but is different from the one that we
				// are searching for, so we continue searching there.
				return bunch -> find_link_to(a_bunch, a_link_ptr, a_bs);
			}
			else
			{
				// This can be a potential match because the target bunch has
				// not been set yet, so it is safe to assume that the link may
				// not be unique.
				assert(a_link_ptr != this);
				return false;
			}
		}
		else if (! a_link_ptr)
		{
			// This is the first match, so we can assume the link is unique.
			a_link_ptr = this;
			return true;
		}
		else if (a_link_ptr == this)
		{
			// This is not the first match, however the link pointer is the
			// same, so we can continue to assume that the link is unique.
			return true;
		}
		else
		{
			// There is a different link pointing to the same bunch, that is the
			// link we have found is not unique.
			return false;
		}
	}

	/**
	 *	Attach a semantic action to this link object to be passed to the future
	 *	arc upon its deployment.
	 *
	 *	@param	a_action
	 *		Semantic action to be passed to the future arc
	 */
	void attach_action (const typename action<M_>::type& a_action)
	{
		m_actions. push_back(a_action);
	}

	/**
	 *	Pass the semantic actions to an arc.
	 *
	 *	@param	a_arc
	 *		Arc for the semantic actions to be passed to
	 */
	void pass_actions (Arc<M_>& a_arc)
	{
		for (typename actions_t::iterator i = m_actions. begin();
				i != m_actions. end(); ++ i)
		{
			a_arc. attach(*i);
		}
		m_actions. clear();
	}

	/**
	 *	Get the target bunch if it exists.
	 *
	 *	@return
	 *		Target bunch
	 */
	virtual Bunch<M_> get_bunch () const = 0;

	/**
	 *	Replace all pointers to a particular bunch with a pointer to some other
	 *	bunch within the target domain.
	 *
	 * 	@param	a_from
	 * 		Replaceable bunch reference
	 * 	@param	a_to
	 *		Replacer bunch reference
	 * 	@param	a_bs
	 *		Processed bunch set (protection)
	 */
	virtual void replace (const Bunch<M_>& a_from, const Bunch<M_>& a_to,
			BunchSet<M_>& a_bs) = 0;

	/**
	 *	Deploy the link on the given network node.
	 *
	 * 	@param	a_generator
	 *		Generator reference
	 * 	@param	a_node_from
	 *		Source node index
	 * 	@param	a_start_index
	 *		Mutable reference to the next available node index
	 *	@return
	 *		Deployed arc reference
	 */
	virtual Arc<M_>& deploy (Generator<M_>& a_generator,
			const uint_t a_node_from, uint_t& a_next_index) = 0;

private:
	/**
	 *	The label to be set on the future arc when it is deployed.
	 */
	Label<M_> m_label;

	/**
	 *	The container type for the semantic actions.
	 */
	typedef std::vector<typename action<M_>::type> actions_t;

	/**
	 *	The semantic actions container.
	 */
	actions_t m_actions;

};

/******************************************************************************/

/**
 *	LinkJump<M_> template class is a generator agent that connects inner
 *	acceptor network nodes by deploying links of 'jump' type between them.
 */
template <typename M_>
class LinkJump: public LinkBase<M_>
{
public:
	LinkJump (const Bunch<M_>& a_bunch, const Acceptor<M_>& a_acceptor,
			const arc_type_t a_arc_type, const Label<M_>& a_label):
		LinkBase<M_> (a_label), m_bunch (a_bunch), m_acceptor (&a_acceptor),
		m_arc_type (a_arc_type)
	{
	}

public:
	// Overridden from LinkBase<M_>:

	Bunch<M_> get_bunch () const
	{
		return m_bunch;
	}

	void replace (const Bunch<M_>& a_from, const Bunch<M_>& a_to,
			BunchSet<M_>& a_bs)
	{
		if (m_bunch == a_from)
		{
			m_bunch = a_to;
		}
		else
		{
			m_bunch -> replace(a_from, a_to, a_bs);
		}
	}

	Arc<M_>& deploy (Generator<M_>& a_generator, const uint_t a_node_from,
			uint_t& a_start_index)
	{
		// Create an arc between two inner network nodes.
		Arc<M_>& arc = a_generator. link(a_node_from,
				m_bunch -> index(a_start_index), *m_acceptor, m_arc_type,
				this -> get_label());
		// Set entanglement identifier and priority on the new arc.
		arc. set_entanglement(this -> get_entanglement());
		arc. set_priority(this -> get_priority());
		// Pass semantic actions.
		this -> pass_actions(arc);
		// Prevent cyclic references.
		m_bunch. reset();
		return arc;
	}

private:
	Bunch<M_> m_bunch;
	const Acceptor<M_>* m_acceptor;
	arc_type_t m_arc_type;

};

/******************************************************************************/

/**
 *	LinkCall<M_> template class is a generator agent that connects inner and
 *	outer acceptor network nodes by deploying links of 'call' type between them.
 */
template <typename M_>
class LinkCall: public LinkBase<M_>
{
public:
	LinkCall (const ::anta::Node<M_>& a_node):
		LinkBase<M_> (), m_node (&a_node)
	{
	}

public:
	// Overridden from LinkBase<M_>:

	Bunch<M_> get_bunch () const
	{
		// There is no target domain, so an emphemeral bunch is returned.
		return Bunch<M_>(-1);
	}

	void replace (const Bunch<M_>& a_from, const Bunch<M_>& a_to,
			BunchSet<M_>& a_bs)
	{
		// It does nothing because there is no target domain.
	}

	Arc<M_>& deploy (Generator<M_>& a_generator, const uint_t a_node_from,
			uint_t& a_start_index)
	{
		// Create an arc between an inner and an outer network nodes.
		Arc<M_>& arc = a_generator. link(a_node_from, *m_node,
				unconditional<M_>(false), atExtend, this -> get_label());
		// Set entanglement identifier and priority on the new arc.
		arc. set_entanglement(this -> get_entanglement());
		arc. set_priority(this -> get_priority());
		// Pass semantic actions.
		this -> pass_actions(arc);
		return arc;
	}

private:
	const ::anta::Node<M_>* m_node;

};

/******************************************************************************/

/**
 *	JointBase<M_> is a template base class for objects that constitute joint
 *	expressions and generate distinctive functional elements in acceptor network
 *	topology.
 */
template <typename M_>
class JointBase: public Base<JointBase<M_>, M_>
{
public:
	/**
	 *	The polymorphic destructor.
	 */
	virtual ~JointBase() {}

	/**
	 *	Get the entry bunch.
	 *
	 *	@return
	 *		Value-copyable bunch pointer
	 */
	virtual Bunch<M_> entry () const = 0;

	/**
	 *	Get the exit bunch.
	 *
	 *	@return
	 *		Value-copyable bunch pointer
	 */
	virtual Bunch<M_> exit () const = 0;

public:
	/**
	 *	Alternation operator.
	 *	@{ */
	JointSeq<M_> operator| (const JointBase& j) const
	{
		BunchSet<M_> bs;
		j. entry() -> replace(j. exit(), exit(), bs);
		entry() -> merge(j. entry());
		return JointSeq<M_>(entry(), exit());
	}

	JointSeq<M_> operator|| (const JointBase& j) const
	{
		return (*this) | j;
	}
	/**	@} */

	/**
	 *	Sequencing (expectation) operator.
	 */
	JointSeq<M_> operator> (const JointBase& j) const
	{
		BunchSet<M_> bs;
		entry() -> replace(exit(), j. entry(), bs);
		return JointSeq<M_>(entry(), j. exit());
	}

	/**
	 *	Assertion operator.
	 */
	JointStmt<M_> operator& () const
	{
		return JointStmt<M_>(entry(), atPositive);
	}

	/**
	 *	Negation operator.
	 */
	JointStmt<M_> operator! () const
	{
		return JointStmt<M_>(entry(), atNegative);
	}

	/**
	 *	Grouping operator.
	 */
	JointStmt<M_> operator- () const
	{
		return JointStmt<M_>(entry(), atExtend);
	}

	/**
	 *	Omission operator.
	 */
	JointSeq<M_> operator~ () const
	{
		entry() -> link(exit());
		return JointSeq<M_>(entry(), exit());
	}

	/**
	 *	Kleene repetition (plus) operator: one or more times.
	 */
	JointKleene<M_> operator+ () const
	{
		return JointKleene<M_>(entry(), false);
	}

	/**
	 *	Kleene repetition (star) operator: zero or more times.
	 */
	JointKleene<M_> operator* () const
	{
		return JointKleene<M_>(entry(), true);
	}

	/**
	 *	Enumeration operator.
	 */
	JointEnum<M_> operator% (const JointBase& j) const
	{
		return JointEnum<M_>(entry(), j. entry());
	}

	/**
	 *	Non-associative undefined order (permutation) operator.
	 */
	JointPerm<M_> operator^ (const JointBase& j) const
	{
		return JointPerm<M_>(entry(), j. entry());
	}

	/**
	 *	Optional prefix or suffix (affixation) operator.
	 */
	JointAffx<M_> operator/ (const JointBase& j) const
	{
		return JointAffx<M_>(entry(), j. entry());
	}

	/**
	 *	Unconditional semantic action attachment operator.
	 */
	JointStmt<M_> operator& (const typename action<M_>::type& a_action) const
	{
		return JointStmt<M_>(a_action, entry());
	}

	/**
	 *	Conditional semantic action attachment operator, type A.
	 */
	JointStmt<M_> operator() (const typename action<M_>::type& a_action) const
	{
		return JointStmt<M_>(entry(), a_action);
	}

	/**
	 *	Conditional semantic action attachment operator, type B.
	 */
	JointAlt<M_> operator[] (const typename action<M_>::type& a_action) const
	{
		LinkBase<M_>* link = NULL;
		BunchSet<M_> bs;
		if (entry() -> find_link_to(exit(), link, bs))
		{
			// If there is one unique link to the exit bunch then the semantic
			// action is being attached to that link instance.
			link -> attach_action(a_action);
			return JointAlt<M_>(entry(), exit());
		}
		else
		{
			// Otherwise, the semantic action is being attached to a new link
			// instance that connects the exit bunch and an additional bunch.
			return JointAlt<M_>(entry(), exit(), a_action);
		}
	}

	/**
	 *	Operator for assignment of the accepted range to a trace variable.
	 */
	JointStmt<M_> operator> (const typename context_key<M_>::type& a_key) const
	{
		return (*this)(action_assign<M_, delta<M_> >(a_key, delta<M_>()));
	}

	/**
	 *	Labelling operator.
	 */
	JointSeq<M_> operator* (const Label<M_>& a_label) const
	{
		BunchSet<M_> bs;
		entry() -> set_label(a_label, bs);
		return JointSeq<M_>(entry(), exit());
	}

};

/******************************************************************************/

/**
 *	JointJump<M_> is an instantiable base class for joint expression elements
 *	that generate connected inner nodes.
 */
template <typename M_>
class JointJump: public JointBase<M_>
{
protected:
	JointJump ()
	{
	}

public:
	JointJump (const Acceptor<M_>& a_acceptor, const Label<M_>& a_label)
	{
		entry() -> link(exit(), a_acceptor, a_label);
	}

public:
	// Overridden from JointBase<M_>:

	Bunch<M_> entry () const
	{
		return m_entry;
	}

	Bunch<M_> exit () const
	{
		return m_exit;
	}

private:
	Bunch<M_> m_entry, m_exit;

};

/******************************************************************************/

/**
 *	JointCall<M_> is a joint expression element that generates call to an outer
 *	(foreign) node.
 */
template <typename M_>
class JointCall: public JointJump<M_>
{
public:
	JointCall (const ::anta::Node<M_>& a_node):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid);
		m_mid -> link(a_node);
		m_mid -> link(this -> exit());
	}

private:
	Bunch<M_> m_mid;

};

/******************************************************************************/

/**
 *	JointSeq<M_> is a joint expression element that generates sequence of inner
 *	nodes.
 */
template <typename M_>
class JointSeq: public JointBase<M_>
{
public:
	JointSeq (const Bunch<M_>& a_entry, const Bunch<M_>& a_exit):
		m_entry (a_entry), m_exit (a_exit)
	{
	}

public:
	// Overridden from JointBase<M_>:

	Bunch<M_> entry () const
	{
		return m_entry;
	}

	Bunch<M_> exit () const
	{
		return m_exit;
	}

private:
	Bunch<M_> m_entry, m_exit;

};

/******************************************************************************/

/**
 *	JointStmt<M_> is a joint expression element that generates a statement, i.e.
 *	a specialized call to an inner node.
 */
template <typename M_>
class JointStmt: public JointJump<M_>
{
public:
	/**
	 *	Constructor: creates an arc of the given type.
	 */
	JointStmt (const Bunch<M_>& a_entry, const arc_type_t a_arc_type):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid);
		m_mid -> link(a_entry, a_arc_type);
		m_mid -> link(this -> exit());
	}

	/**
	 *	Constructor: attaches the given semantic action to the entry.
	 */
	JointStmt (const typename action<M_>::type& a_action,
			const Bunch<M_>& a_entry):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid, unconditional<M_>(false))
			. attach_action(a_action);
		m_mid -> link(a_entry, atInvoke);
		m_mid -> link(this -> exit());
	}

	/**
	 *	Constructor: attaches the given semantic action to the exit.
	 */
	JointStmt (const Bunch<M_>& a_entry,
			const typename action<M_>::type& a_action):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid, unconditional<M_>(false));
		m_mid -> link(a_entry, atInvoke);
		m_mid -> link(this -> exit()). attach_action(a_action);
	}

private:
	Bunch<M_> m_mid;

};

/******************************************************************************/

/**
 *	JointKleene<M_> is a joint expression element that generates a structural
 *	equivalent of the Kleene star operator.
 */
template <typename M_>
class JointKleene: public JointJump<M_>
{
public:
	JointKleene (const Bunch<M_>& a_entry, const bool a_accept_empty):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid);
		if (a_accept_empty)
		{
			m_mid -> link(this -> exit());
			m_mid -> link(a_entry, atInvoke);
			m_mid -> link(m_mid);
		}
		else
		{
			m_mid -> link(a_entry, atInvoke);
			m_mid -> link(m_mid);
			m_mid -> link(this -> exit());
		}
	}

private:
	Bunch<M_> m_mid;

};

/******************************************************************************/

/**
 *	JointEnum<M_> is a joint expression element that generates a structural
 *	equivalent of the enumeration operator.
 */
template <typename M_>
class JointEnum: public JointJump<M_>
{
public:
	JointEnum (const Bunch<M_>& a_entry0, const Bunch<M_>& a_entry1):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid0);
		m_mid0 -> link(a_entry0, atInvoke);
		m_mid0 -> link(m_mid1);
		m_mid0 -> link(this -> exit());
		m_mid1 -> link(a_entry1, atInvoke);
		m_mid1 -> link(m_mid0);
	}

	JointEnum (const Bunch<M_>& a_entry0, const Bunch<M_>& a_entry1,
			const Bunch<M_>& a_spacer_left, const Bunch<M_>& a_spacer_right,
			const int a_flags = 3):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid0);
		m_mid0 -> link(a_entry0, atInvoke);
		m_mid0 -> link(m_mid1);
		m_mid0 -> link(this -> exit());
		if (a_flags & 1)
		{
			m_mid1 -> link(a_spacer_left, atInvoke);
		}
		m_mid1 -> link(a_entry1, atInvoke);
		if (a_flags & 2)
		{
			m_mid1 -> link(a_spacer_right, atInvoke);
		}
		m_mid1 -> link(m_mid0);
	}

private:
	Bunch<M_> m_mid0, m_mid1;

};

/******************************************************************************/

/**
 *	JointPerm<M_> is a joint expression element that generates a structural
 *	equivalent of the non-associative permutation operator.
 */
template <typename M_>
class JointPerm: public JointJump<M_>
{
public:
	JointPerm (const Bunch<M_>& a_entry0, const Bunch<M_>& a_entry1):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid0);
		this -> entry() -> link(m_mid1);
		m_mid0 -> link(a_entry0, atInvoke);
		m_mid0 -> link(a_entry1, atInvoke);
		m_mid0 -> link(this -> exit());
		m_mid1 -> link(a_entry1, atInvoke);
		m_mid1 -> link(a_entry0, atInvoke);
		m_mid1 -> link(this -> exit());
	}

	JointPerm (const Bunch<M_>& a_entry0, const Bunch<M_>& a_entry1,
			const Bunch<M_>& a_spacer):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid0);
		this -> entry() -> link(m_mid1);
		m_mid0 -> link(a_entry0, atInvoke);
		m_mid0 -> link(a_spacer, atInvoke);
		m_mid0 -> link(a_entry1, atInvoke);
		m_mid0 -> link(this -> exit());
		m_mid1 -> link(a_entry1, atInvoke);
		m_mid1 -> link(a_spacer, atInvoke);
		m_mid1 -> link(a_entry0, atInvoke);
		m_mid1 -> link(this -> exit());
	}

private:
	Bunch<M_> m_mid0, m_mid1;

};

/******************************************************************************/

/**
 *	JointAffx<M_> is a joint expression element that generates a structural
 *	equivalent of the optional "prefix or suffix" operator.
 */
template <typename M_>
class JointAffx: public JointJump<M_>
{
public:
	JointAffx (const Bunch<M_>& a_entry0, const Bunch<M_>& a_entry1):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid0);
		this -> entry() -> link(m_mid1);
		m_mid0 -> link(a_entry0, atInvoke);
		m_mid0 -> link(m_mid2);
		m_mid0 -> link(this -> exit());
		m_mid1 -> link(a_entry1, atInvoke);
		m_mid1 -> link(a_entry0, atInvoke);
		m_mid1 -> link(this -> exit());
		m_mid2 -> link(a_entry1, atInvoke);
		m_mid2 -> link(this -> exit());
	}

	JointAffx (const Bunch<M_>& a_entry0, const Bunch<M_>& a_entry1,
			const Bunch<M_>& a_spacer):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid0);
		this -> entry() -> link(m_mid1);
		m_mid0 -> link(a_entry0, atInvoke);
		m_mid0 -> link(m_mid2);
		m_mid0 -> link(this -> exit());
		m_mid1 -> link(a_entry1, atInvoke);
		m_mid1 -> link(a_spacer, atInvoke);
		m_mid1 -> link(a_entry0, atInvoke);
		m_mid1 -> link(this -> exit());
		m_mid2 -> link(a_spacer, atInvoke);
		m_mid2 -> link(a_entry1, atInvoke);
		m_mid2 -> link(this -> exit());
	}

private:
	Bunch<M_> m_mid0, m_mid1, m_mid2;

};

/******************************************************************************/

/**
 *	JointAlt<M_> is a joint expression element that generates a structural
 *	equivalent of the ateration operator.
 */
template <typename M_>
class JointAlt: public JointSeq<M_>
{
public:
	JointAlt (const Bunch<M_>& a_entry, const Bunch<M_>& a_exit):
		JointSeq<M_> (a_entry, a_exit)
	{
	}

	JointAlt (const Bunch<M_>& a_entry, const Bunch<M_>& a_exit,
			const typename action<M_>::type& a_action):
		JointSeq<M_> (a_entry, Bunch<M_>())
	{
		a_exit -> link(this -> exit()). attach_action(a_action);
	}

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_GENERATOR_HPP_ */
