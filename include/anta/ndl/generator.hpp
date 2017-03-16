/*
 * @file $/include/anta/ndl/generator.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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

// NOTE: Using flat_set here is experimental, std::set also works fine.
#include <boost/container/flat_set.hpp>

/*
 *	Generator
 *
 *	LinkBase
 *	  |
 *	  |-- LinkJump
 *	  |
 *	  \-- LinkCall
 *
 *	Bunch
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

namespace anta { namespace ndl {

// (forward declarations)
template <typename M_> class BunchImpl;
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
 *	Value-copyable bunch (see the definition of the BunchImpl class below).
 */
template <typename M_>
class Bunch: public boost::shared_ptr<BunchImpl<M_> >
{
public:
	Bunch ():
		boost::shared_ptr<BunchImpl<M_> >(new BunchImpl<M_>())
	{
	}

	Bunch (const int a_init):
		boost::shared_ptr<BunchImpl<M_> >()
	{
		assert(a_init == -1);
	}

};

/******************************************************************************/

/**
 *	An auxiliary bunch pointer container that is used to track processed
 *	instances in order to prevent infinite recursion.
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
 *	Specifies a basic interface for objects that generate network topology by
 *	creating arcs between nodes.
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
	 *	Create an arc between two local (numbered) nodes.
	 */
	virtual Arc<M_>& link (const uint_t a_node_from, const uint_t a_node_to,
			const Acceptor<M_>& a_acceptor, const arc_type_t a_arc_type,
			const Label<M_>& a_label) = 0;

	/**
	 *	Create an arc between a local (numbered) node and an outer node.
	 */
	virtual Arc<M_>& link (const uint_t a_node_from, const ::anta::Node<M_>&,
			const Acceptor<M_>& a_acceptor, const arc_type_t a_arc_type,
			const Label<M_>& a_label) = 0;

protected:
	/**
	 *	Generate the network topology from a joint representation.
	 */
	uint_t generate (const JointBase<M_>& a_joint, uint_t node_counter = 0)
	{
		// Define the list of processed bunches.
		BunchSet<M_> processed;

		// Define primary and secondary sets of bunches.
		typedef std::vector<Bunch<M_> > bunches_t;
		bunches_t s0, s1;

		// Initialize the primary set.
		s0. push_back(a_joint. entry());

		// Repeat until the primary set is empty.
		while (! s0. empty())
		{
			// Iterate through bunches of the primary set.
			for (typename bunches_t::iterator s = s0. begin(); s != s0. end();
					++ s)
			{
				// Check if the current bunch hasn't been processed yet.
				if (! (*s && processed. insert(s -> get()). second))
					continue;
				// Determine index of the source node.
				const uint_t index = (*s) -> index(node_counter);
				// Iterate through links of the current bunch.
				for (typename BunchImpl<M_>::iterator t = (*s) -> begin();
						t != (*s) -> end(); ++ t)
				{
					// Store target bunch pointer in the secondary set.
					s1. push_back((*t) -> get_bunch());
					// Deploy link between source and target bunches.
					(*t) -> deploy(*this, index, node_counter);
				}
			}
			// Swap contents of primary and secondary sets,
			s1. swap(s0);
			// then clear the secondary set.
			s1. clear();
		}

		return node_counter;
	}

};

/******************************************************************************/

/**
 *	Specifies a basic interface for links between network nodes.
 */
template <typename M_>
class LinkBase: public Base<LinkBase<M_>, M_>, public Entangled<M_>
{
	typedef std::vector<typename action<M_>::type> actions_t;

public:
	/**
	 *	The only constructor.
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
	 *	Set the label of the future arc.
	 */
	void set_label (const Label<M_>& a_label, BunchSet<M_>& a_bs)
	{
		Bunch<M_> bunch = get_bunch();
		if (bunch)
		{
			bunch -> set_label(a_label, a_bs);
		}
		m_label. advance(a_label);
	}

	/**
	 *	Get the label of the future arc.
	 */
	const Label<M_>& get_label () const
	{
		return m_label;
	}

	/**
	 *	Find an only link to a particular bunch.
	 */
	bool find_link_to (const Bunch<M_>& a_to, LinkBase<M_>*& a_ptr,
			BunchSet<M_>& a_bs) // NOTE: non-constant function
	{
		Bunch<M_> bunch = get_bunch();
		if (bunch != a_to)
		{
			if (bunch)
			{
				return bunch -> find_link_to(a_to, a_ptr, a_bs);
			}
			else
			{
				return false;
			}
		}
		else if (! a_ptr)
		{
			a_ptr = this;
			return true;
		}
		else if (a_ptr == this)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/**
	 *	Attach a semantic action to the link object.
	 */
	void attach_action (const typename action<M_>::type& a_action)
	{
		m_actions. push_back(a_action);
	}

	/**
	 *	Pass the semantic actions to an arc.
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
	 *	Get a pointer to the target bunch (if there is such one).
	 */
	virtual Bunch<M_> get_bunch () const
	{
		// NOTE: There is no target bunch by default.
		return Bunch<M_>(-1);
	}

	/**
	 *	Replace all pointers to a particular bunch in the target domain with a
	 *	pointer to another bunch.
	 */
	virtual void replace (const Bunch<M_>& a_from, const Bunch<M_>& a_to,
			BunchSet<M_>& a_bs)
	{
		// NOTE: It does nothing because there is no target domain by default.
	}

	/**
	 *	Deploy a link of a certain type between two network nodes.
	 */
	virtual Arc<M_>& deploy (Generator<M_>& a_generator,
			const uint_t a_node_from, uint_t& a_node_counter) = 0;

private:
	Label<M_> m_label;
	actions_t m_actions;

};

/******************************************************************************/

/**
 *	Represents a jump link between two local network nodes.
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
			uint_t& a_node_counter)
	{
		// Generate a link between two local numbered nodes.
		Arc<M_>& arc = a_generator. link(a_node_from,
				m_bunch -> index(a_node_counter), *m_acceptor, m_arc_type,
				this -> get_label());
		// Pass entanglement identifier to the new arc.
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
 *	Represents a call link between a local network node and an outer node.
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

	Arc<M_>& deploy (Generator<M_>& a_generator, const uint_t a_node_from,
			uint_t& a_node_counter)
	{
		// Generate a call link from a local numbered node to an outer node.
		Arc<M_>& arc = a_generator. link(a_node_from, *m_node,
				unconditional<M_>(false), atExtend, this -> get_label());
		// Pass entanglement identifier to the new arc.
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
 *	Represents a bunch of outgoing links of a network node.
 */
template <typename M_>
class BunchImpl: public std::list<LinkBase<M_>*>
{
	typedef std::list<LinkBase<M_>*> base_type;

public:
	typedef typename base_type::value_type value_type;
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
	 *	Assign an index to the bunch (if it hasn't been done already) and return
	 *	its value.
	 */
	uint_t index (uint_t& counter)
	{
		if (m_index == 0)
		{
			m_index = (counter ++);
		}
		return m_index;
	}

	/**
	 *	Replace all of the pointers to a particular bunch with a pointer to
	 *	another bunch.
	 */
	void replace (const Bunch<M_>& a_from, const Bunch<M_>& a_to,
			BunchSet<M_>& a_bs)
	{
		if (is_call_recursive(a_bs))
		{
			return ;
		}
		for (iterator i = this -> begin(); i != this -> end(); ++ i)
		{
			(*i) -> replace(a_from, a_to, a_bs);
		}
	}

	/**
	 *	Merge two bunches into one.
	 */
	void merge (const Bunch<M_>& a_bunch)
	{
		this -> splice(this -> end(), *a_bunch);
		assert(a_bunch -> empty());
	}

	/**
	 *	Find an only link to a particular bunch.
	 */
	bool find_link_to (const Bunch<M_>& a_to, LinkBase<M_>*& a_ptr,
			BunchSet<M_>& a_bs) const
	{
		if (is_call_recursive(a_bs))
		{
			return true;
		}
		for (const_iterator i = this -> begin(); i != this -> end(); ++ i)
		{
			if (! (*i) -> find_link_to(a_to, a_ptr, a_bs))
				return false;
		}
		return true;
	}

	/**
	 *	Link a local bunch by appending a simple arc.
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
	 *	Link a local bunch by appending an invoking arc.
	 */
	LinkBase<M_>& link (const Bunch<M_>& a_bunch, const arc_type_t a_arc_type)
	{
		this -> push_back(new LinkJump<M_>(a_bunch, unconditional<M_>(false),
					a_arc_type, Label<M_>()));
		return *(this -> back());
	}

	/**
	 *	Link an outer node by appending an invoking arc.
	 */
	LinkBase<M_>& link (const ::anta::Node<M_>& a_node)
	{
		this -> push_back(new LinkCall<M_>(a_node));
		return *(this -> back());
	}

	/**
	 *	Set the labels of the future arcs for the whole bunch.
	 */
	void set_label (const Label<M_>& a_label, BunchSet<M_>& a_bs)
	{
		if (is_call_recursive(a_bs))
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
	 *	Check whether the call is topologically recursive.
	 */
	bool is_call_recursive (BunchSet<M_>& a_bs) const
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

private:
	uint_t m_index;

};

/******************************************************************************/

/**
 *	Specifies a basic interface for objects (joints) that are used for defining
 *	the topology of a network.
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
	 *	Get a reference to the entry bunch.
	 */
	virtual Bunch<M_> entry () const = 0;

	/**
	 *	Get a reference to the exit bunch.
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
			// If there is only one link to the exit bunch then the semantic
			// action is being attached to that link.
			link -> attach_action(a_action);
			return JointAlt<M_>(entry(), exit());
		}
		else
		{
			// Otherwise, the semantic action is being attached to a new link
			// object that connects the exit bunch and an additional bunch.
			return JointAlt<M_>(entry(), exit(), a_action);
		}
	}

	/**
	 *	Operator for assignment of the accepted range to a trace variable.
	 */
	JointStmt<M_> operator>> (const typename context_key<M_>::type& a_key) const
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
 *	Represents two local interconnected nodes.
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
 *	Represents a call to an outer node.
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
 *	Represents a sequence of local nodes.
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
 *	Represents a statement (a typified call to a local node).
 */
template <typename M_>
class JointStmt: public JointJump<M_>
{
public:
	/**
	 *	Constructor: creates an arc of a special type.
	 */
	JointStmt (const Bunch<M_>& a_entry, const arc_type_t a_arc_type):
		JointJump<M_> ()
	{
		this -> entry() -> link(m_mid);
		m_mid -> link(a_entry, a_arc_type);
		m_mid -> link(this -> exit());
	}

	/**
	 *	Constructor: attaches an action to the entry.
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
	 *	Constructor: attaches an action to the exit.
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
 *	Represents the Kleene star operator.
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
 *	Represents the enumeration operator.
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
 *	Represents the nonassociative undefined order operator.
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
 *	Represents the optional prefix or suffix operator.
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
 *	Represents an altered sequence of local nodes.
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
