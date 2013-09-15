/*
 * @file $/include/anta/core/traveller.hpp
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
#ifndef ANTA_CORE_TRAVELLER_HPP_
#define ANTA_CORE_TRAVELLER_HPP_

namespace anta {

/******************************************************************************/

/**
 *	Default pool type.
 */
template <typename M_>
struct pool
{
	typedef utility::memory_pool type;

};

/**
 *	Observer event types. (For debug purposes only).
 */
enum observer_event_t
{
	evPUSH,		/**< new state has been pushed to the queue */
	evPULL,		/**< state has been pulled out of the queue */
	evDENY,		/**< state has been blocked due to entry failure */
	evENTRY,	/**< successful entry to a state has been completed */
	evTRACE,	/**< state has been traced */
	evBLOCK,	/**< caller state has been blocked */
	evSPLIT,	/**< new split state has been spawned */
	evDEFER,	/**< entangled state has been deferred */

};

/**
 *	Default observer type. (For debug purposes only).
 */
template <typename M_>
struct observer
{
	class type
	{
	public:
		/**
		 *	Notify the observer about an event that has taken place for the
		 *	given state.
		 */
		void notify (const observer_event_t a_event, const State<M_>* a_state)
		{
		}

		/**
		 *	Submit current traveller status information including the total
		 *	amount of iterations completed so far, current pool usage, queue
		 *	length, trace count and deferred state count.
		 */
		void status (
				const uint_t a_iteration_count,
				const uint_t a_pool_usage,
				const uint_t a_queue_length,
				const uint_t a_trace_count,
				const uint_t a_deferred_count)
		{
		}

		/**
		 *	Reset internal observer's state regardless whatever that may mean.
		 */
		void reset ()
		{
		}

	};

};

/**
 *	Default specialization of the base class for the Traveller class.
 *
 * 	NOTE: The third template parameter which equals to meta::true_ means that
 *		  this base class specialization is applied for basic models only.
 */
template <typename M_>
class Base<Traveller<M_>, M_, meta::true_>
{
public:
	/**
	 *	Reset internal traveller's state, regardless whatever that may mean.
	 */
	void reset ()
	{
	}

protected:
	State<M_>* m_state;

};

/******************************************************************************/

/**
 *	Analysis state traveller.
 */
template <typename M_>
class Traveller: public Base<Traveller<M_>, M_>, public pool<M_>::type
{
public:
	/**
	 *	Model type.
	 */
	typedef M_ model_type;

	/**
	 *	The only constructor.
	 *	@param a_entry_node		The entry node of an acceptor network.
	 *	@param a_label			A label to mark the entry (initial) arc.
	 */
	Traveller (const Node<M_>& a_entry_node,
			const Label<M_>& a_label = Label<M_>()):
		m_entry_arc (a_entry_node, unconditional<M_>(), atSimple, a_label)
	{
	}

	/**
	 *	The destructor.
	 */
	~Traveller ()
	{
		reset();
	}

	/**
	 *	Get a reference to the local observing object.
	 */
	typename observer<M_>::type& get_observer ()
	{
		return m_observer;
	}

public:
	/**
	 *	Initialize the traveller by setting the source range.
	 *	@param a_from			The beginning of the range (an iterator).
	 *	@param a_to				The end of the range (an iterator).
	 */
	void init (const typename iterator<M_>::type& a_from,
			const typename iterator<M_>::type& a_to)
	{
		// Initialize the working variables.
		m_C. first = a_from;
		m_C. second = a_to;
		m_arc = &m_entry_arc;
		m_state = NULL;
		// Spawn an initial state and push it to the traveller's queue.
		// NOTE: Storing the initial state's pointer as the current state's
		//		 pointer allows to predefine the trace variables before
		//		 running the acceptor network traversal algorithm.
		push(m_state = spawn(a_from, a_from));
	}

	/**
	 *	Run the acceptor network traversal algorithm.
	 *	@return					The number of performed iterations.
	 */
	uint_t run ();

	/**
	 *	Initialize the traveller and run the algorithm at once.
	 */
	uint_t run (const typename iterator<M_>::type& a_from,
			const typename iterator<M_>::type& a_to)
	{
		init(a_from, a_to);
		return run();
	}

	/**
	 *	Reset the traveller to the initial state.
	 */
	void reset ()
	{
		Base<Traveller<M_>, M_>::reset();
		pool<M_>::type::clear();
		m_queue. clear();
		m_traced. clear();
		m_deferred. clear();
		m_observer. reset();
	}

public:
	/**
	 *	A container type for the trace list.
	 */
	typedef std::vector<const State<M_>*> traced_type;

	/**
	 *	Get a constant reference to the local trace list.
	 */
	const traced_type& get_traced () const
	{
		return m_traced;
	}

public:
	/**
	 *	Spawn a new descendant state for the given range.
	 */
	State<M_>* spawn (const typename iterator<M_>::type& a_from,
		const typename iterator<M_>::type& a_to)
	{
		return new(*this) StateCommon<M_>(m_state, m_arc, a_from, a_to);
	}

	/**
	 *	Push a new descendant state into the traveller's queue.
	 */
	State<M_>* push (State<M_>* a_descendant)
	{
		if (a_descendant -> get_arc(). get_target(). get_entanglement() == 0)
		{
			m_queue. push_back(a_descendant);
			m_observer. notify(evPUSH, a_descendant);
		}
		else
		{
			m_deferred. push_back(a_descendant);
			m_observer. notify(evDEFER, a_descendant);
		}
		return a_descendant;
	}

	/**
	 *	Spawn a new descendant state for the given range and push it to the
	 *	traveller's queue at once.
	 */
	State<M_>* push (const typename iterator<M_>::type& a_from,
		const typename iterator<M_>::type& a_to)
	{
		// NOTE: Splitting of the process of generation of new states in two
		//		 logical steps (spawning and pushing) allows acceptors to
		//		 perform additional actions on descendant states such as setting
		//		 trace variables, etc.
		return push(spawn(a_from, a_to));
	}

private:
	/**
	 *	Implements the inner loop of the acceptor network traversal algorithm.
	 *	@return					The number of performed iterations.
	 */
	uint_t run0 ();

	/**
	 *	Filter out all the descendants of the given ancestor state in a
	 *	container.
	 */
	template <typename Container_>
	void filter (const State<M_>* a_ancestor, Container_& a_container);

private:
	typedef std::vector<const State<M_>*> deferred_type;

	const Arc<M_> m_entry_arc;				/**< entry arc */
	std::deque<State<M_>*> m_queue;			/**< traveller's queue */
	traced_type m_traced;					/**< traced state list */
	deferred_type m_deferred;				/**< deferred state list */
	typename range<M_>::type m_C;			/**< source range */
	const Arc<M_>* m_arc;					/**< current arc pointer */
	using Base<Traveller<M_>, M_>::m_state;	/**< current state pointer */
	typename observer<M_>::type m_observer;	/**< local observing object */

};

/**
 *	An auxiliary structure that allows to filter out unwanted entangled states.
 */
template <typename M_>
struct spawner
{
	State<M_>* state;
	const State<M_>* root;
	int priority;

	/**
	 *	The only constructor.
	 */
	spawner (const State<M_>* a_state, const State<M_>* a_root,
			const int a_priority):
		state (const_cast<State<M_>*>(a_state)), root (a_root),
		priority (a_priority)
	{
	}

	/**
	 *	Group/priority/offset ordering operator.
	 */
	bool operator< (const spawner& a_sp) const
	{
		// Sort rules:
		// 1. root of entangled group, descending
		// 2. arc priority, ascending
		// 3. accepted range length, descending [ optional ]
		return
			(root != a_sp. root)
			?	root > a_sp. root
			:	priority < a_sp. priority;
//			:	(priority != a_sp. priority)
//				?	priority < a_sp. priority
//				:	  state -> get_range(). second
//					> a_sp. state -> get_range(). second;
	}

};

template <typename M_>
uint_t Traveller<M_>::run ()
{
	uint_t iteration_count = 0;
	typedef spawner<M_> spawner_type;
	typedef std::vector<spawner_type> spawners_type;
	spawners_type spawners;

	// The outer loop.
	while (true)
	{
		// Run the inner loop.
		iteration_count += run0();

		// Check if there are any deferred states.
		if (m_deferred. empty())
			break;

		// Reserve space in spawner list.
		spawners. reserve(m_deferred. size());

		// Determine spawner arc for each deferred state.
		for (typename deferred_type::iterator i = m_deferred. begin();
				i != m_deferred. end(); ++ i)
		{
			// Get the entanglement identifier.
			const uint_t eid =
				(*i) -> get_arc(). get_target(). get_entanglement();
			assert( eid != 0 );

			// Find a state that corresponds to the spawner arc.
			const State<M_>* p = *i;
			int level = 0;
			while (p != NULL)
			{
				const Arc<M_>& arc = p -> get_arc();

				if (eid == arc. get_target(). get_entanglement())
				{
					++ level;
				}

				if (eid == arc. get_entanglement())
				{
					-- level;
				}

				if (level == 0)
					break;

				p = p -> State<M_>::get_ancestor();
			}
			assert( p != NULL );

			// Save spawner state.
			spawners. push_back(spawner_type(
				*i,
				p -> State<M_>::get_ancestor(),
				p -> get_arc(). get_priority()
			));
		}
		m_deferred. clear();

		// Sort spawners first by entangled group id, then by arc priority and
		// then by the length of accepted subsequence (see member operator `<').
		std::sort(spawners. begin(), spawners. end());

		// Return top state back to the traveller's queue.
		typename spawners_type::const_iterator j = spawners. begin();
		m_queue. push_back(j -> state);

		// Return other states of the same entangled group and the same priority
		// to the traveller's queue.
		const State<M_>* root = j -> root;
		int priority = j -> priority;
		while (++ j != spawners. end() && j -> root == root)
		{
			if (j -> priority == priority)
				m_queue. push_back(j -> state);
		}

		// Save other entangled groups for later. 
		for ( ; j != spawners. end(); ++ j)
		{
			if (root != j -> root)
			{
				root = j -> root;
				priority = j -> priority;
			}
			if (priority == j -> priority)
			{
				m_deferred. push_back(j -> state);
			}
		}

		// Clean up.
		spawners. clear();
	}

	return iteration_count;
}

template <typename M_>
uint_t Traveller<M_>::run0 ()
{
	// Auxiliary working variables.
	bool has_negations = false;
	uint_t iteration_count = 0;
	const State<M_>* callee;
	const State<M_>* caller;

	// The inner loop.
	while (! m_queue. empty())
	{
		// Submit status.
		m_observer. status(
			/* iteration count */	iteration_count ++,
			/* queue size */		m_queue. size(),
			/* pool usage */		pool<M_>::type::get_usage(),
			/* trace count */		m_traced. size(),
			/* deferred count */	m_deferred. size()
		);

		// Pull another state out of the traveller's queue.
		if (has_negations)
		{
			// (breadth-first search)
			m_state = m_queue. front();
			m_queue. pop_front();
		}
		else
		{
			// (depth-first search)
			m_state = m_queue. back();
			m_queue. pop_back();
		}
		m_observer. notify(evPULL, m_state);

		// Try to enter the state (perform the semantic actions).
		if (! entry(*this))
		{
			// The blocking of the current state is unnecessary in this case. It
			// is done for debug purposes only. This will show that the state
			// had been blocked for some reason and not just left unhandled.
			m_state -> block();
			m_observer. notify(evDENY, m_state);
			continue;
		}

		m_observer. notify(evENTRY, m_state);

		// Check whether the state corresponds to an endpoint node.
		if (m_state -> get_arc(). get_target(). is_endpoint())
		{
			// Get the callee and the caller state pointers.
			callee = m_state -> get_callee();
			caller = (callee != NULL) ? callee -> get_ancestor() : NULL;

			// If there is no caller state then we're on the surface, and
			// therefore another trace head has to be registered within the
			// trace list.
			if (caller == NULL)
			{
				m_traced. push_back(m_state);
				m_observer. notify(evTRACE, m_state);
				continue;
			}

			// Create a split state of the type that corresponds to the type of
			// the callee arc.
			switch (callee -> get_arc_type())
			{
			// NOTE: We drop the original state pointer here and move on to the
			//		 arc enumeration phase with the split state pointer.
			case atInvoke:
				m_state = new(*this) StateSplitShifted<M_>(caller, m_state);
				break;

			case atExtend:
				m_state = new(*this) StateSplitExtended<M_>(caller, m_state);
				break;

			case atPositive:
				m_state = new(*this) StateSplit<M_>(caller);
				break;

			case atNegative:
				// The caller state becomes blocked if a return form a negative
				// statement call happens.
				if (const_cast<State<M_>*>(caller) -> block())
				{
					m_observer. notify(evBLOCK, caller);
					// In order to cut off unwanted traces we filter out all
					// descendants of the blocked state in the queue.
					filter(caller, m_queue);
					// The local trace list may need some cleaning too,
					filter(caller, m_traced);
					// and the deferred state list as well.
					filter(caller, m_deferred);
				}
				else
				{
					// @todo: is this possible at all?
				}
				// no break;

			default:
				continue;
			}

			m_observer. notify(evSPLIT, m_state);
		}

		// NOTE: State<M_>::get_bunch() and State<M_>::get_range() are not
		// 		 virtual member functions besides they imply inlinable calls,
		// 		 therefore we won't get any extra perfomance from usage of
		// 		 references in this case.

		// Iterate through the arcs that are left unprocessed for the current
		// state.
		while (m_state -> get_bunch(). get(m_arc))
		{
			// Generate a portion of descendant states for the current arc and
			// the current position in the analyzed context.
			m_arc -> get_acceptor(). accept(m_C, m_state -> get_range(), *this);

			// If the current arc implies an invocation or an assertion
			const arc_type_t arc_type = m_arc -> get_type();
			if (arc_type == atInvoke || arc_type == atExtend
					|| arc_type == atPositive)
			{
				// then we interrupt the iteration through the arcs.
				break;
			}
			// Otherwise, if there is a negation, we virtually flip the queue in
			// order to save pool space.
			else if (arc_type == atNegative)
			{
				has_negations = true;
			}
		}
	}

	return iteration_count;
}

template <typename M_>
template <typename Container_>
void Traveller<M_>::filter (const State<M_>* a_ancestor,
		Container_& a_container)
{
	for (typename Container_::iterator i = a_container. begin();
			i != a_container. end(); ++ i)
	{
		// NOTE: The scan below is computationally expensive, but the outcome
		//		 happens to be worth it. It utilizes the fact that ancestor's
		//		 address is always less than descentant's.
		const State<M_>* p = *i;
		while (p > a_ancestor)
			p = p -> get_ancestor();

		// If a descendant was found, then
		if (p == a_ancestor)
		{
			// 1) block the descendant
			const_cast<State<M_>*>(*i) -> block();
			m_observer. notify(evBLOCK, *i);

			// 2) remove it from the container
			const typename Container_::difference_type index =
				i - a_container. begin();
			a_container. erase(i);
			i = a_container. begin() + (index - 1);
		}
	}
}

/******************************************************************************/

} // namespace anta

#endif /* ANTA_CORE_TRAVELLER_HPP_ */
