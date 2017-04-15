/*
 * @file $/include/anta/core/state.compressed.hpp
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
#ifndef ANTA_CORE_STATE_HPP_
#define ANTA_CORE_STATE_HPP_

/*
 *	State
 *	  |
 *	  |-- StateCommon
 *	  |
 *	  \-- StateSplit
 *	        |
 *	        \-- StateSplitShifted
 *	              |
 *	              \-- StateSplitExtended
 *
 */

#include <util/gag.hpp>

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
		m_offsets (0), m_bunch (a_bunch)
	{
		set_first(a_ancestor);
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
		return -1 == reinterpret_cast<long>(get_first());
	}

	void block ()
	{
		if (is_blocked())
		{
			// This should never happen.
			throw std::logic_error("state is already blocked");
		}
		set_first(reinterpret_cast<const State<M_>*>(-1));
	}

	bool is_split () const
	{
		return get_ancestor() != State<M_>::get_ancestor();
	}

public:
	virtual const State<M_>* get_ancestor () const
	{
		return is_blocked() ? NULL : get_first();
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

	virtual std::size_t size () const = 0;

protected:
	void set_first (const State<M_>* a_ptr)
	{
		if (a_ptr == NULL)
		{
			m_offsets &= UPPER;
		}
		else if (a_ptr == reinterpret_cast<const State<M_>*>(-1))
		{
			m_offsets |= LOWER;
		}
		else
		{
			const char* base = reinterpret_cast<const char*>(this);
			const char* target = reinterpret_cast<const char*>(a_ptr);
			m_offsets = (m_offsets & UPPER) | (base - target);
		}
	}

	const State<M_>* get_first () const
	{
		const std::size_t offset = m_offsets & LOWER;
		if (offset == 0)
		{
			return NULL;
		}
		else if (offset == LOWER)
		{
			return reinterpret_cast<const State<M_>*>(-1);
		}
		else
		{
			const char* base = reinterpret_cast<const char*>(this);
			return reinterpret_cast<const State<M_>*>(base - offset);
		}
	}

	void set_second (const State<M_>* a_ptr)
	{
		if (a_ptr == NULL)
		{
			m_offsets &= LOWER;
		}
		else if (a_ptr == this)
		{
			m_offsets |= UPPER;
		}
		else
		{
			const char* base = reinterpret_cast<const char*>(this);
			const char* target = reinterpret_cast<const char*>(a_ptr);
			m_offsets = (m_offsets & LOWER) | ((base - target) << SHIFT);
		}
	}

	const State<M_>* get_second () const
	{
		const std::size_t offset = (m_offsets >> SHIFT) & LOWER;
		if (offset == 0)
		{
			return NULL;
		}
		else if (offset == LOWER)
		{
			return this;
		}
		else
		{
			const char* base = reinterpret_cast<const char*>(this);
			return reinterpret_cast<const State<M_>*>(base - offset);
		}
	}

private:
	static const std::size_t LOWER = 0x00000000FFFFFFFFULL;
	static const std::size_t UPPER = 0xFFFFFFFF00000000ULL;
	static const int SHIFT = 32;
	std::size_t m_offsets;
	typename Node<M_>::bunch_type m_bunch;

};

/**
 *	A derivative object representing common analysis states.
 *	Corresponds to arcs that define jumps (simple type).
 */
template <typename M_>
class StateCommon: public State<M_>
{
	using State<M_>::set_second;
	using State<M_>::get_second;

public:
	StateCommon (const State<M_>* a_ancestor, const Arc<M_>* a_arc,
			const typename iterator<M_>::type& a_from,
			const typename iterator<M_>::type& a_to):
		State<M_> (a_arc -> get_target(). get_bunch(), a_ancestor),
		m_arc (a_arc), m_range (a_from, a_to)
	{
		set_second(
			a_arc -> get_type() != atSimple
			? this
			: (a_ancestor ? a_ancestor -> get_callee() : NULL)
		);
	}

public:
	// Overridden from State<M_>:

	const State<M_>* get_callee () const
	{
		return get_second();
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

	std::size_t size () const
	{
		return sizeof(StateCommon<M_>);
	}

private:
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
	using State<M_>::set_second;
	using State<M_>::get_second;

public:
	StateSplit (const State<M_>* a_caller):
		State<M_> (a_caller -> get_bunch(), a_caller -> get_ancestor())
	{
		set_second(a_caller);
	}

public:
	// Overridden from State<M_>:

	const State<M_>* get_callee () const
	{
		return get_second() -> get_callee();
	}

	const State<M_>* get_caller () const
	{
		return get_second();
	}

	const typename range<M_>::type& get_range () const
	{
		return get_second() -> get_range();
	}

	const Arc<M_>& get_arc () const
	{
		return get_second() -> get_arc();
	}

	std::size_t size () const
	{
		return sizeof(StateSplit<M_>);
	}

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

	std::size_t size () const
	{
		return sizeof(StateSplitShifted<M_>);
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

	std::size_t size () const
	{
		return sizeof(StateSplitExtended<M_>);
	}

private:
	typename range<M_>::type m_range;

};

/******************************************************************************/

} // namespace anta

#include <util/gag.hpp>

#endif /* ANTA_CORE_STATE_HPP_ */
