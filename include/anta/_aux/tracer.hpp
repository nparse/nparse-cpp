/*
 * @file $/include/anta/_aux/tracer.hpp
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
#ifndef ANTA_AUX_TRACER_HPP_
#define ANTA_AUX_TRACER_HPP_

namespace anta { namespace aux {

/******************************************************************************/

/**
 *	The Tracer is a helper object that is used for exporting the analysis traces
 *	from the analysis state traveller.
 */
template <typename M_>
class Tracer: public Base<Tracer<M_>, M_>
{
public:
	/**
	 *	The only constructor.
	 */
	Tracer (const Traveller<M_>& a_traveller):
		m_traveller (a_traveller), m_next (false), m_step (false)
	{
	}

public:
	/**
	 *	Move to the next trace.
	 */
	bool next ()
	{
		if (! m_next)
			m_trace_it = m_traveller. get_traced(). begin();
		else
			++ m_trace_it;

		m_next = false;
		for ( ; m_trace_it != m_traveller. get_traced(). end(); ++ m_trace_it)
		{
			m_trace. clear();
			const State<M_>* s;
			for (s = *m_trace_it; s != NULL; s = s -> get_ancestor())
			{
				assert(! s -> is_blocked());
				if (s -> is_split() || s -> get_arc(). get_label(). is_actual())
					m_trace. push_back(s);
			}
			if (s == NULL)
			{
				m_next = true;
				break;
			}
		}

		if (m_next)
		{
			m_step = false;
			return true;
		}

		return false;
	}

	/**
	 *	Step through the current trace's states.
	 */
	bool step ()
	{
		if (! m_next)
			throw std::logic_error("no current trace");

		if (! m_step)
		{
			m_step = true;
			m_state_it = m_trace. rbegin();
		}
		else
		{
			++ m_state_it;
		}

		if (m_state_it == m_trace. rend())
		{
			m_step = false;
			return false;
		}

		return true;
	}

	/**
	 *	Get the type of the current state.
	 */
	int type () const
	{
		// check if the state is a split
		if ((*this) -> is_split())
		{
			return 2;
		}
		else
		// check if the corresponding transition is explicit (actual)
		if ((*this) -> get_arc(). get_label(). is_actual())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	/**
	 *	Rewind the tracer.
	 */
	void rewind ()
	{
		m_next = false;
		m_step = false;
		m_trace. clear();
	}

	/**
	 *	Operator for getting the current state.
	 */
	const State<M_>& operator* () const
	{
		if (! m_step)
		{
			if (! m_next)
				throw std::logic_error("no current trace");
			return **m_trace_it;
		}
		else
		{
			return **m_state_it;
		}
	}

	/**
	 *	Operator for accessing the current state.
	 */
	const State<M_>* operator-> () const
	{
		return &**this;
	}

	/**
	 *	Request a relative state.
	 */
	const State<M_>* relative (const char* a_path) const
	{
		const State<M_>* s = &**this;
		for (const char* p = a_path; s && *p; ++ p)
		{
			switch (*p)
			{
			case '<':
				s = s -> get_ancestor();
				break;
			case ':':
				s = s -> get_callee();
				break;
			case '!':
				s = s -> get_caller();
				break;
			}
		}
		return s;
	}

private:
	const Traveller<M_>& m_traveller;
	bool m_next, m_step;
	typedef std::vector<const anta::State<M_>*> trace_t;
	trace_t m_trace;
	typename Traveller<M_>::traced_type::const_iterator m_trace_it;
	typename trace_t::const_reverse_iterator m_state_it;

};

/******************************************************************************/

}} // namespace anta::aux

#endif /* ANTA_AUX_TRACER_HPP_ */
