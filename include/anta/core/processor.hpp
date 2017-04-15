/*
 * @file $/include/anta/core/processor.hpp
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
#ifndef ANTA_CORE_PROCESSOR_HPP_
#define ANTA_CORE_PROCESSOR_HPP_

namespace anta {

/******************************************************************************/

/**
 *	The default pool type.
 */
template <typename M_>
struct pool
{
	typedef utility::memory_pool type;

};

/**
 *	The definition and the default implementation of analysis state entry point.
 */
template <typename Processor_, typename M_>
struct entry_point
{
	static bool f (Processor_&)
	{
		return true;
	}

};

/**
 *	An invocation helper for analysis state entry points.
 */
template <typename Processor_>
inline bool enter (Processor_& a_proc)
{
	return entry_point<Processor_, typename Processor_::model_type>::f(a_proc);
}

/**
 *	The definition and the default implementation of the analysis state rollback
 *	mechanism. It removes defunct states from the tail of the memory pool along
 *	with as many of their ancestors as possible.
 */
template <typename Processor_, typename M_>
struct rollback_performer
{
	static void f (Processor_& a_proc, const State<M_>* p)
	{
		while (p != NULL && a_proc. evict(p, p -> size()))
		{
			a_proc. get_observer(). notify(evEVICT, p);
			p = p -> get_ancestor();
		}
	}

};

/**
 *	An invocation helper for the analysis state rollback mechanism.
 */
template <typename Processor_, typename M_>
inline void rollback (Processor_& a_proc, const State<M_>* a_state)
{
	rollback_performer<Processor_, M_>::f(a_proc, a_state);
}

/**
 *	The default specialization of the template base class for the Processor.
 *
 * 	NOTE: The third template parameter here, which is set to meta::true_ here
 *		  implies that this specialization is applied for basic models only.
 */
template <typename M_>
class Base<Processor<M_>, M_, meta::true_>: public pool<M_>::type
{
public:
	/**
	 *	Reset the processor.
	 */
	void reset ()
	{
	}

protected:
	State<M_>* m_state; /**< current state pointer */

};

/**
 *	The analysis state processor.
 */
template <typename M_>
class Processor: public Base<Processor<M_>, M_>
{
public:
	/**
	 *	The model for which this processor is specialized.
	 */
	typedef M_ model_type;

	/**
	 *	The only constructor.
	 *
	 *	@param	a_entry_node
	 *		Entry node of an acceptor network
	 *	@param	a_label
	 *		Virtual entry arc label
	 */
	Processor (const Node<M_>& a_entry_node,
			const Label<M_>& a_label = Label<M_>()):
		m_entry_arc (a_entry_node, unconditional<M_>(), atSimple, a_label)
	{
	}

	/**
	 *	The destructor.
	 */
	~Processor ()
	{
		reset();
	}

	/**
	 *	Get the observer.
	 */
	typename observer<M_>::type& get_observer ()
	{
		return m_observer;
	}

public:
	/**
	 *	Initialize the processor by setting a source range.
	 *
	 *	@param	a_from
	 *		Iterator pointing to the beginning of the source range (inclusive)
	 *	@param	a_to
	 *		Iterator pointing to the end of the source range (exclusive)
	 */
	void init (const typename iterator<M_>::type& a_from,
			const typename iterator<M_>::type& a_to)
	{
		// Initialize the working variables.
		m_C. first = a_from;
		m_C. second = a_to;
		m_arc =& m_entry_arc;
		m_state = NULL;
		// Spawn an initial state and push it to the processing queue.
		// NOTE: Storing the initial state pointer as the current state pointer
		//		 allows to predefine some trace variables before running the
		//		 Acceptor Network Traversal Algorithm.
		push(m_state = spawn(a_from, a_from));
	}

	/**
	 *	Run the processor.
	 *
	 *	@return
	 *		Number of completed iterations
	 */
	uint_t run ();

	/**
	 *	Initialize the processor by setting a source range and run it at once.
	 *
	 *	@param	a_from
	 *		Iterator pointing to the beginning of the source range (inclusive)
	 *	@param	a_to
	 *		Iterator pointing to the end of the source range (exclusive)
	 *	@return
	 *		Number of completed iterations
	 */
	uint_t run (const typename iterator<M_>::type& a_from,
			const typename iterator<M_>::type& a_to)
	{
		init(a_from, a_to);
		return run();
	}

	/**
	 *	Reset the processor.
	 */
	void reset ()
	{
		Base<Processor<M_>, M_>::reset();
		pool<M_>::type::clear();
		m_queue. clear();
		m_traced. clear();
		m_deferred. clear();
		m_observer. reset();
	}

	/**
	 *	The container type for found traces.
	 */
	typedef std::vector<const State<M_>*> traced_type;

	/**
	 *	Get found traces.
	 *
	 *	@return
	 *		Constant reference to the found traces container
	 */
	const traced_type& get_traced () const
	{
		return m_traced;
	}

	/**
	 *	Spawn a new descendant state for the given source range.
	 *
	 *	@param	a_from
	 *		Iterator pointing to the beginning of the source range (inclusive)
	 *	@param	a_to
	 *		Iterator pointing to the end of the source range (exclusive)
	 *	@return
	 *		Descendant state pointer
	 */
	State<M_>* spawn (const typename iterator<M_>::type& a_from,
		const typename iterator<M_>::type& a_to)
	{
		return new(*this) StateCommon<M_>(m_state, m_arc, a_from, a_to);
	}

	/**
	 *	Push a descendant state into the processing queue.
	 *
	 *	@param	a_descendant
	 *		Descendant state pointer
	 *	@return
	 *		Same descendant state pointer (convenience)
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
	 *	Spawn a new descendant state for the given source range and push it to
	 *	the processing queue at once.
	 *
	 *	@param	a_from
	 *		Iterator pointing to the beginning of the source range (inclusive)
	 *	@param	a_to
	 *		Iterator pointing to the end of the source range (exclusive)
	 *	@return
	 *		Descendant state pointer
	 */
	State<M_>* push (const typename iterator<M_>::type& a_from,
		const typename iterator<M_>::type& a_to)
	{
		// NOTE: The generation of new states implemented as two separate steps,
		//		 namely spawn and push, provides acceptors with the crucially
		//		 important ability to interact with descendant states before
		//		 they get added to the processing queue.
		return push(spawn(a_from, a_to));
	}

private:
	/**
	 *	Implements the inner loop of the traversal algorithm.
	 *
	 *	@return
	 *		Number of completed iterations
	 */
	uint_t run0 ();

	/**
	 *	Filter out all descendants of the given ancestor from a state container.
	 *
	 *	@param	a_ancestor
	 *		Ancestor state
	 *	@param	a_container
	 *		State container
	 */
	template <typename Container_>
	void filter (const State<M_>* a_ancestor, Container_& a_container);

private:
	/**
	 *	The container type for deferred states.
	 */
	typedef std::vector<const State<M_>*> deferred_type;

	const Arc<M_> m_entry_arc;					/**< entry arc */
	std::deque<State<M_>*> m_queue;				/**< processing queue */
	traced_type m_traced;						/**< found traces */
	deferred_type m_deferred;					/**< deferred states */
	typename range<M_>::type m_C;				/**< source range */
	const Arc<M_>* m_arc;						/**< current arc pointer */
	using Base<Processor<M_>, M_>::m_state;		/**< current state pointer */
	typename observer<M_>::type m_observer;		/**< observer */

};

/**
 *	spawner<M_> is an auxiliary sorting unit which function is to rearrange
 *	entangled states in the decreasing priority order.
 */
template <typename M_>
struct spawner
{
	State<M_>* state;
	const State<M_>* origin;
	int priority;

	/**
	 *	The only constructor.
	 *
	 *	@param	a_state
	 *		Deferred state
	 *	@param	a_origin
	 *		Entanglement group (origin)
	 *	@param	a_priority
	 *		Arc priority
	 */
	spawner (const State<M_>* a_state, const State<M_>* a_origin,
			const int a_priority):
		state (const_cast<State<M_>*>(a_state)), origin (a_origin),
		priority (a_priority)
	{
	}

	/**
	 *	The ordering operator: (entanglement group, arc priority).
	 *
	 *	@param	a_sp
	 *		Another spawner instance to be compared against this one
	 */
	bool operator< (const spawner& a_sp) const
	{
		return (origin != a_sp. origin)
			?	origin > a_sp. origin
			:	priority < a_sp. priority;
	}

};

template <typename M_>
uint_t Processor<M_>::run ()
{
	uint_t total_iteration_count = 0;

	typedef spawner<M_> spawner_type;
	typedef std::vector<spawner_type> spawners_type;
	spawners_type spawners;

	// The outer loop.
	while (true)
	{
		// Run the inner loop.
		total_iteration_count += run0();

		// Check if there are any deferred states.
		if (m_deferred. empty())
		{
			break;
		}

		// Reserve space in the spawner list.
		spawners. reserve(m_deferred. size());

		// Determine origin for each deferred state.
		for (typename deferred_type::iterator i = m_deferred. begin();
				i != m_deferred. end(); ++ i)
		{
			// Get the entanglement identifier.
			const uint_t eid =
				(*i) -> get_arc(). get_target(). get_entanglement();
			assert(eid != 0);

			// Find the origin state.
			int level = 0;
			const State<M_>* p = *i;
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
				{
					break;
				}

				p = p -> State<M_>::get_ancestor();
			}
			assert(p != NULL);

			// Save entangled state, its origin and priority.
			spawners. push_back(spawner_type(
				*i,
				p -> State<M_>::get_ancestor(),
				p -> get_arc(). get_priority()
			));
		}
		m_deferred. clear();

		// Sort spawners first by entanglement group, then by arc priority.
		std::sort(spawners. begin(), spawners. end());

		// Return the top state back to the processing queue.
		typename spawners_type::const_iterator j = spawners. begin();
		m_queue. push_back(j -> state);

		// Return other states of the same entanglement group and the same
		// priority to the processing queue.
		const State<M_>* origin = j -> origin;
		int priority = j -> priority;
		while (++ j != spawners. end() && j -> origin == origin)
		{
			if (j -> priority == priority)
			{
				m_queue. push_back(j -> state);
			}
		}

		// Save top candidates from other entanglement groups for later
		// consideration.
		for ( ; j != spawners. end(); ++ j)
		{
			if (origin != j -> origin)
			{
				origin = j -> origin;
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

	return total_iteration_count;
}

template <typename M_>
uint_t Processor<M_>::run0 ()
{
	// Auxiliary working variables.
	uint_t inner_iteration_count = 0;
	bool has_negations = false;
	const State<M_>* callee;
	const State<M_>* caller;

	// The inner loop.
	while (! m_queue. empty())
	{
		// Submit current processor status.
		m_observer. status(
			inner_iteration_count ++,
			m_queue. size(),
			m_traced. size(),
			m_deferred. size(),
			pool<M_>::type::get_usage(),
			pool<M_>::type::get_peak_usage(),
			pool<M_>::type::get_evicted_size()
		);

		// Pull another state out of the processing queue.
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

		// Try to enter the state (execute semantic actions).
		if (! enter(*this))
		{
			m_observer. notify(evDENY, m_state);
			rollback(*this, m_state);
			continue;
		}

		m_observer. notify(evENTRY, m_state);

		// Check whether the state corresponds to an final node.
		if (m_state -> get_arc(). get_target(). is_final())
		{
			// Get the callee and the caller state pointers.
			callee = m_state -> get_callee();
			caller = (callee != NULL) ? callee -> get_ancestor() : NULL;

			// If there is no caller state then we're on the surface, and
			// therefore another trace is found.
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
			case atSimple:
				// This should never happen.
				throw std::logic_error("call from simple arc");

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
				// The caller state becomes blocked if a return from a negative
				// statement call happens.
				const_cast<State<M_>*>(caller) -> block();
				m_observer. notify(evBLOCK, caller);
				filter(caller, m_queue);
				filter(caller, m_traced);
				filter(caller, m_deferred);
				continue;
			}

			m_observer. notify(evSPLIT, m_state);
		}

		// NOTE: State<M_>::get_bunch() and State<M_>::get_range() are inlinable
		//		 calls, therefore no performance gains are expected from caching
		//		 their return values locally.

		// Process remaining arcs for the current state.
		while (m_state -> get_bunch(). get(m_arc))
		{
			// Generate next portion of descendant states for the current arc
			// and the current position in the analyzed context.
			m_arc -> get_acceptor(). accept(m_C, m_state -> get_range(), *this);

			// If the current arc implies an invocation or an assertion then the
			// processing of rest of the arcs gets postponed.
			const arc_type_t arc_type = m_arc -> get_type();
			if (arc_type == atInvoke || arc_type == atExtend
					|| arc_type == atPositive)
			{
				break;
			}

			// If the current arc implies a negation, then the entire consequent
			// network traversal has to be performed in breadth-first order.
			if (arc_type == atNegative)
			{
				has_negations = true;
			}
		}

		rollback(*this, m_state);
	}

	return inner_iteration_count;
}

template <typename M_>
template <typename Container_>
void Processor<M_>::filter (const State<M_>* a_ancestor,
		Container_& a_container)
{
	for (typename Container_::iterator i = a_container. begin();
			i != a_container. end(); ++ i)
	{
		// NOTE: The scan below is computationally expensive, but the outcome
		//		 happens to be worth it. It effectively utilizes the fact that
		//		 ancestor's address is always less than descendant's.
		const State<M_>* p = *i;
		while (p > a_ancestor)
		{
			p = p -> get_ancestor();
		}

		// If a descendant was found, then
		if (p == a_ancestor)
		{
			// 1) .. block the descendant
			const_cast<State<M_>*>(*i) -> block();
			m_observer. notify(evBLOCK, *i);

			// 2) .. remove the descendant from the container
			const typename Container_::difference_type index =
				i - a_container. begin();
			a_container. erase(i);
			i = a_container. begin();
			std::advance(i, index - 1);
		}
	}
}

/******************************************************************************/

} // namespace anta

#endif /* ANTA_CORE_PROCESSOR_HPP_ */
