/*
 * @file $/source/nparse-test/src/utils/parsing.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef SRC_UTILS_PARSING_HPP_
#define SRC_UTILS_PARSING_HPP_

#include <iostream>
#include <sstream>
#include <anta/_aux/tracer.hpp>
#include <anta/_aux/debug_print.hpp>

/**
 *	Given an entry node of an acceptor network and some input text, creates a
 *	string fingerprint that contains information about iteration count and trace
 *	trajectories represented as sequences of passed arc labels.
 */
template <typename M_>
std::string parse (const anta::Node<M_>& entry, const std::string& text,
		const anta::uint_t state_pool_capacity = 1024)
{
	std::stringstream out;

	// prepare traveller
	anta::Traveller<M_> traveller(entry);
	traveller. set_capacity(state_pool_capacity);
#if defined(DEBUG_PRINT)
	traveller. get_observer(). link(& std::cout);
#endif

	// perform parsing
	out << traveller. run(
			&* text. begin(), &* text. begin() + text. size());

	// extract traces
	anta::aux::Tracer<M_> tracer(traveller);
	while (tracer. next())
	{
		out << " |";
		while (tracer. step())
		{
			const int label = tracer -> get_arc(). get_label(). get();
			if (label != 0)
				out << ' ' << label;
		}
	}

	// cleanup [not really necessary]
	tracer. rewind();
	traveller. reset();

	// return fingerprint
	return out. str();
}

/**
 *	Given an entry node of an acceptor network and and some input text, returns
 *	count of allowed traces.
 */
template <typename M_>
unsigned int trace_count (const anta::Node<M_>& entry, const std::string& text,
		const anta::uint_t state_pool_capacity = 1024)
{
	// prepare traveller
	anta::Traveller<M_> traveller(entry);
	traveller. set_capacity(state_pool_capacity);
#if defined(DEBUG_PRINT)
	traveller. get_observer(). link(& std::cout);
#endif

	// perform parsing
	traveller. run(&* text. begin(), &* text. begin() + text. size());

	// extract traces
	anta::aux::Tracer<M_> tracer(traveller);
	unsigned int count = 0;
	while (tracer. next()) ++ count;

	// cleanup [not really necessary]
	tracer. rewind();
	traveller. reset();

	// return trace count
	return count;
}

/**
 *	An auxiliary template that implements sequences of unconditionally linked
 *	acceptor network nodes (also known as node chains).
 */
template <typename M_>
class NodeChain: public std::vector<anta::Node<M_> >
{
public:
	NodeChain (const unsigned int N, const int I = 1)
	{
		this -> resize(N);
		for (int n = 1; n < static_cast<int>(N); ++ n)
		{
			(*this)[n-1]. link((*this)[n], anta::unconditional<M_>(),
					anta::atSimple, I + n - 1);
		}
	}

};

#endif /* SRC_UTILS_PARSING_HPP_ */
