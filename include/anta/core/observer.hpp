/*
 * @file $/include/anta/core/observer.hpp
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
#ifndef ANTA_CORE_OBSERVER_HPP_
#define ANTA_CORE_OBSERVER_HPP_

namespace anta {

/******************************************************************************/

/**
 *	Observable processor events.
 */
enum observer_event_t
{
	evPUSH,		/**< new state has been pushed to the queue */
	evPULL,		/**< state has been pulled out of the queue */
	evDENY,		/**< state has been blocked due to an entry failure */
	evENTRY,	/**< successful entry to a state has been completed */
	evTRACE,	/**< state has been traced */
	evBLOCK,	/**< caller state has been blocked */
	evSPLIT,	/**< new split state has been spawned */
	evDEFER,	/**< entangled state has been deferred */
	evEVICT,	/**< evict state from the pool */

};

/**
 *	The default processor observer implementation.
 */
template <typename M_>
struct observer
{
	struct type
	{
		/**
		 *	Reset the observer.
		 */
		void reset ()
		{
		}

		/**
		 *	Notify the observer about an event that has just taken place for the
		 *	provided state.
		 */
		void notify (const observer_event_t a_event, const State<M_>* a_state)
		{
		}

		/**
		 *	Submit current processor status information including the total
		 *	amount of iterations completed so far, current and peak pool usage,
		 *	queue length, trace count and deferred state count.
		 */
		void status (
			const uint_t a_iteration_count,
			const uint_t a_queue_length,
			const uint_t a_trace_count,
			const uint_t a_deferred_count,
			const uint_t a_pool_usage,
			const uint_t a_peak_pool_usage,
			const uint_t a_evicted_size)
		{
		}

	};

};

/******************************************************************************/

} // namespace anta

#endif /* ANTA_CORE_OBSERVER_HPP_ */
