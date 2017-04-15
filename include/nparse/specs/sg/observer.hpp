/*
 * @file $/include/nparse/specs/sg/observer.hpp
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
#ifndef NPARSE_SPECS_SG_OBSERVER_HPP_
#define NPARSE_SPECS_SG_OBSERVER_HPP_

namespace nparse {

/**
 *	A type that describes syntax errors in the script, also referred to as
 *	syntactic deadlocks, or analysis states without descendants.
 */
typedef std::pair<anta::iterator<SG>::type, string_t> deadlock_t;

} // namespace nparse

/**
 *	The anta::observer<nparse::SG> template specialization for the script
 *	grammar model makes syntax error tracking possible.
 */
namespace anta {

// (forward declaration)
struct observer_sg_data_t;

template <>
struct observer<nparse::SG>: public boost::noncopyable
{
	class type
	{
	public:
		/**
		 *	The only constructor.
		 */
		type ();

		/**
		 *	The destructor.
		 */
		~type ();

		/**
		 *	Set maximum trace count.
		 */
		void set_max_trace_count (const std::size_t a_max_trace_count);

		/**
		 *	Set maximum trace depth.
		 */
		void set_max_trace_depth (const std::size_t a_max_trace_depth);

		/**
		 *	Analyze trace heads and extract syntactic deadlocks.
		 */
		void analyze (std::vector<nparse::deadlock_t>& a_dls,
				const bool a_reverse = true) const;

	public:
		/**
		 *	Implementations of functions required by the observer concept.
		 *	@{ */

		void reset ();

		void notify (const observer_event_t a_event,
				const State<nparse::SG>* a_state);

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

		/**	@} */

	private:
		observer_sg_data_t* m_;

	};

};

} // namespace anta

#endif /* NPARSE_SPECS_SG_OBSERVER_HPP_ */
