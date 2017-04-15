/*
 * @file $/source/libnparse_script/src/observer_sg.cpp
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
#include <set>
#include <encode/encode.hpp>
#include <nparse/nparse.hpp>

namespace anta {

struct item_t
{
	const State<nparse::SG>* state;
	nparse::deadlock_t dl;

};

typedef std::vector<item_t> heap_t;

// Elements of the heap are analysis state pointers. The top element in the heap
// is always the farthest LEFT reached element from the current position. It is
// done this way because we do not want to track every reached element, but only
// a fixed amount of elements near to the current position in the input.
struct offset_order
{
	bool operator() (const item_t& u, const item_t& v) const
	{
		return u. dl. first > v. dl. first;
	}

};

class state_matcher
{
	const State<nparse::SG>* m_state;

public:
	state_matcher (const State<nparse::SG>* a_state):
		m_state (a_state)
	{
	}

	bool operator() (const item_t& a_item) const
	{
		return m_state == a_item. state;
	}

};

struct observer_sg_data_t
{
	std::size_t max_trace_count;
	std::size_t max_trace_depth;
	heap_t heap;

};

nparse::deadlock_t render_trace (const State<nparse::SG>* a_state,
		const std::size_t a_max_depth, const bool a_reverse = false)
{
	std::string tmp;

	std::size_t len = 0;
	const State<nparse::SG>* first_named = NULL;

	for (const State<nparse::SG>* p = a_state;
			p != NULL && len < a_max_depth; p = p -> get_ancestor())
	{
		// Get node name.
		const anta::ndl::node_name<nparse::SG>::type& node =
			p -> get_arc(). get_target(). get_name();

		if (! node. empty())
		{
			// Get arc label.
			const std::string& arc = p -> get_arc(). get_label(). get();

			if (a_reverse)
			{
				if (len > 0) tmp. append(" < ");
				tmp. append(encode::string(node));
				tmp. append(1, ':');
				tmp. append(arc);
			}
			else
			{
				if (len > 0) tmp. insert(0, " > ");
				tmp. insert(0, arc);
				tmp. insert(0, ':', 1);
				tmp. insert(0, encode::string(node));
			}
			++ len;
		}

		if (len == 1)
		{
			first_named = p;
		}
	}

	return nparse::deadlock_t(
			(first_named ? first_named : a_state) -> get_range(). second, tmp);
}

observer<nparse::SG>::type::type():
	m_ (new observer_sg_data_t())
{
	m_ -> max_trace_count = 1;
	m_ -> max_trace_depth = 0;
}

observer<nparse::SG>::type::~type()
{
	if (m_ != NULL)
	{
		delete m_;
		m_ = NULL;
	}
}

void observer<nparse::SG>::type::set_max_trace_count (
		const std::size_t a_max_trace_count)
{
	m_ -> max_trace_count = a_max_trace_count;
}

void observer<nparse::SG>::type::set_max_trace_depth (
		const std::size_t a_max_trace_depth)
{
	m_ -> max_trace_depth = a_max_trace_depth;
}

void observer<nparse::SG>::type::reset ()
{
	m_ -> heap. clear();
}

void observer<nparse::SG>::type::notify (const observer_event_t a_event,
		const State<nparse::SG>* a_state)
{
	if (a_event == evPUSH)
	{
		m_ -> heap. push_back(item_t());
		item_t& item = m_ -> heap. back();
		item. state = a_state;
		item. dl. first = a_state -> get_range(). first;
		std::push_heap(m_ -> heap. begin(), m_ -> heap. end(),
				offset_order());
		if (m_ -> heap. size() > m_ -> max_trace_count)
		{
			std::pop_heap(m_ -> heap. begin(), m_ -> heap. end(),
					offset_order());
			heap_t::iterator last = m_ -> heap. begin();
			std::advance(last, m_ -> heap. size() - 1);
			m_ -> heap. erase(last);
		}
	}
	else if (a_event == evEVICT)
	{
		heap_t::iterator found_at = std::find_if(
				m_ -> heap. begin(), m_ -> heap. end(), state_matcher(a_state));
		if (found_at != m_ -> heap. end())
		{
			found_at -> state = NULL;
			found_at -> dl = render_trace(a_state, m_ -> max_trace_depth);
			std::make_heap(m_ -> heap. begin(), m_ -> heap. end(),
				offset_order());
		}
	}
}

void observer<nparse::SG>::type::analyze (
		std::vector<nparse::deadlock_t>& a_dls, const bool a_reverse) const
{
	heap_t copy(m_ -> heap);
	heap_t::iterator i;

	std::sort_heap(copy. begin(), copy. end(), offset_order());
	for (i = copy. begin(); i != copy. end(); ++ i)
	{
		if (i -> state != NULL)
		{
			i -> dl = render_trace(i -> state, m_ -> max_trace_depth);
			i -> state = NULL;
		}
	}

	std::sort(copy. begin(), copy. end(), offset_order());
	for (i = copy. begin(); i != copy. end(); ++ i)
	{
		a_dls. push_back(i -> dl);
	}
}

} // namespace anta
