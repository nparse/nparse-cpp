/*
 * @file $/include/nparse/specs/sg/observer.hpp
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
#ifndef NPARSE_SPECS_SG_OBSERVER_HPP_
#define NPARSE_SPECS_SG_OBSERVER_HPP_

namespace nparse {

/**
 *	A type that describes errors in the script syntax (also known as the
 *	syntactic deadlocks).
 */
typedef std::pair<anta::iterator<SG>::type, string_t> deadlock_t;

} // namespace nparse

/**
 *	The anta::observer<...> template specialization for the script grammar model
 *	makes syntax error tracking possible.
 */
namespace anta {

template <>
struct observer<nparse::SG>
{
	// The queue elements are the analysis state pointers. The top element of
	// the queue is always the farthest left reached element from the current
	// position. It is done this way because we do not need to keep all the
	// reached elements but a certain amount of the closest ones. Therefore,
	// when the queue exceeds the allowed size limit, the top element is being
	// popped out.
	class type
	{
		typedef const State<nparse::SG>* item_t;

		struct offset_order
		{
			bool operator() (const item_t& u, const item_t& v) const
			{
				return u -> get_range(). first > v -> get_range(). first;
			}

		};

		typedef std::priority_queue<item_t, std::vector<item_t>, offset_order>
			queue_t;
		queue_t m_queue;
		queue_t::size_type m_queue_max;
		unsigned int m_trace_max;

	public:
		/**
		 *	Implementations of functions required by the observer concept.
		 *	@{ */

		void notify (const observer_event_t a_event,
				const State<nparse::SG>* a_state)
		{
			m_queue. push(a_state);
			if (m_queue. size() > m_queue_max)
				m_queue. pop();
		}

		void status (
				const uint_t a_iteration_count,
				const uint_t a_pool_usage,
				const uint_t a_queue_length,
				const uint_t a_trace_count,
				const uint_t a_deferred_count)
		{
		}

		void reset ()
		{
			// No magic, just clean up the queue.
			m_queue. ~queue_t();
			new (&m_queue) queue_t();
		}

		/**	@} */

	public:
		/**
		 *	The only constructor.
		 */
		type ():
			m_queue_max (10), m_trace_max (3)
		{
		}

		/**
		 *	Set the maximum length of the tracking queue.
		 */
		void set_queue_max (const queue_t::size_type a_queue_max)
		{
			m_queue_max = a_queue_max;
		}

		/**
		 *	Set the maximum length of trace heads.
		 */
		void set_trace_max (const unsigned int a_trace_max)
		{
			m_trace_max = a_trace_max;
		}

		/**
		 *	Collect data regarding syntactic deadlocks.
		 *	@see See observer_sg.cpp for the implementation details.
		 */
		void analyze (std::vector<nparse::deadlock_t>& a_dls,
				const bool a_reverse = true) const;

	};

};

} // namespace anta

#endif /* NPARSE_SPECS_SG_OBSERVER_HPP_ */
