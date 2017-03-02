/*
 * @file $/include/anta/core/state.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

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
#ifndef ANTA_CORE_STATE_HPP_
#define ANTA_CORE_STATE_HPP_

#include <utility/gag.hpp>

namespace anta {

/******************************************************************************/

/**
 *	A basic object representing analysis states.
 */
template <typename M_>
class State: public Base<State<M_>, M_>
{
public:
	State (const typename Node<M_>::bunch_type& a_bunch,
			const State<M_>* a_ancestor):
		m_bunch (a_bunch), m_ancestor (a_ancestor)
	{
	}

public:
	typename Node<M_>::bunch_type& get_bunch ()
	{
		return m_bunch;
	}

	const typename Node<M_>::bunch_type& get_bunch () const
	{
		return m_bunch;
	}

	bool is_blocked () const
	{
		return -1 == reinterpret_cast<long>(m_ancestor);
	}

	bool block ()
	{
		if (is_blocked())
		{
			return false;
		}
		else
		{
			m_ancestor = reinterpret_cast<const State<M_>*>(-1);
			return true;
		}
	}

	bool is_split () const
	{
		return get_ancestor() != State<M_>::get_ancestor();
	}

public:
	virtual const State<M_>* get_ancestor () const
	{
		return is_blocked() ? NULL : m_ancestor;
	}

	virtual const State<M_>* get_callee () const = 0;

	// NOTE: The 'get_caller' method is an auxiliary facility used solely for
	//		 debug purposes. It does not play any role in the acceptor network
	//		 traversal algorithm.
	virtual const State<M_>* get_caller () const = 0;

	virtual const typename range<M_>::type& get_range () const = 0;

	virtual const Arc<M_>& get_arc () const = 0;

	virtual arc_type_t get_arc_type () const
	{
		return get_arc(). get_type();
	}

private:
	typename Node<M_>::bunch_type m_bunch;
	const State<M_>* m_ancestor;

};

/**
 *	A derivative object representing common analysis states.
 *	Corresponds to arcs that define jumps (simple type).
 */
template <typename M_>
class StateCommon: public State<M_>
{
public:
	StateCommon (const State<M_>* a_ancestor, const Arc<M_>* a_arc,
			const typename iterator<M_>::type& a_from,
			const typename iterator<M_>::type& a_to):
		State<M_> (a_arc -> get_target(). get_bunch(), a_ancestor),
		m_callee ((a_arc -> get_type() != atSimple)
				? this : (a_ancestor ? a_ancestor -> get_callee() : NULL)),
		m_arc (a_arc), m_range (a_from, a_to)
	{
	}

public:
	// Overridden from State<M_>:

	const State<M_>* get_callee () const
	{
		return m_callee;
	}

	const State<M_>* get_caller () const
	{
		return NULL;
	}

	const typename range<M_>::type& get_range () const
	{
		return m_range;
	}

	const Arc<M_>& get_arc () const
	{
		return *m_arc;
	}

private:
	const State<M_>* m_callee;
	const Arc<M_>* m_arc;
	typename range<M_>::type m_range;

};

/**
 *	A derivative object representing split analysis states.
 *	Corresponds to arcs that define assertions (positive type).
 */
template <typename M_>
class StateSplit: public State<M_>
{
public:
	StateSplit (const State<M_>* a_caller):
		State<M_> (a_caller -> get_bunch(), a_caller -> get_ancestor()),
		m_caller (a_caller)
	{
	}

public:
	// Overridden from State<M_>:

	const State<M_>* get_callee () const
	{
		return m_caller -> get_callee();
	}

	const State<M_>* get_caller () const
	{
		return m_caller;
	}

	const typename range<M_>::type& get_range () const
	{
		return m_caller -> get_range();
	}

	const Arc<M_>& get_arc () const
	{
		return m_caller -> get_arc();
	}

private:
	const State<M_>* m_caller;

};

/**
 *	A derivative object representing shifted split states.
 *	Corresponds to arcs that define invocations.
 */
template <typename M_>
class StateSplitShifted: public StateSplit<M_>
{
public:
	StateSplitShifted (const State<M_>* a_caller, const State<M_>* a_shift):
		StateSplit<M_> (a_caller), m_shift (a_shift)
	{
	}

public:
	// Overridden from StateSplit<M_>:

	const State<M_>* get_ancestor () const
	{
		return m_shift;
	}

	const typename range<M_>::type& get_range () const
	{
		return m_shift -> get_range();
	}

	arc_type_t get_arc_type () const
	{
		return atSimple;
	}

private:
	const State<M_>* m_shift;

};

/**
 *	A derivative object representing shifted split states with extended range.
 *	Corresponds to arcs that define invocations and absorb the initial range.
 */
template <typename M_>
class StateSplitExtended: public StateSplitShifted<M_>
{
public:
	StateSplitExtended (const State<M_>* a_caller, const State<M_>* a_shift):
		StateSplitShifted<M_> (a_caller, a_shift),
		m_range (a_caller -> get_range(). second,
				a_shift -> get_range(). second)
	{
	}

public:
	// Overridden from StateSplitShifted<M_>:

	const typename range<M_>::type& get_range () const
	{
		return m_range;
	}

private:
	typename range<M_>::type m_range;

};

/**
 *	Customizable semantic action provider.
 */
template <typename Trav_, typename Model_>
struct entry_functor
{
	static bool f (Trav_&)
	{
		// Does nothing by default.
		return true;
	}

};

/**
 *	Semantic action provider invocation point.
 */
template <typename Trav_>
inline bool entry (Trav_& a_trav)
{
	return entry_functor<Trav_, typename Trav_::model_type>::f(a_trav);
}

/******************************************************************************/

} // namespace anta

#include <utility/gag.hpp>

#endif /* ANTA_CORE_STATE_HPP_ */
