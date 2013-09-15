/*
 * @file $/source/libnparse_script/src/observer_sg.cpp
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
#include <set>
#include <encode/encode.hpp>
#include <nparse/nparse.hpp>

namespace anta {

void observer<nparse::SG>::type::analyze (
		std::vector<nparse::deadlock_t>& a_dls, const bool a_reverse) const
{
	std::set<item_t> tails;
	std::set<item_t>::iterator i;

	// Prepare the set of tail states.
	{
		queue_t queue_copy(m_queue);
		while (! queue_copy. empty())
		{
			tails. insert(queue_copy. top());
			queue_copy. pop();
		}
	}

	// If there are no tails then just leave.
	if (tails. empty())
		return ;

	// Filter out enclosures.
	for (i = tails. begin(); i != tails. end(); ++ i)
	{
		item_t p = *i;
		while (p != NULL)
		{
			p = p -> get_ancestor();
			std::set<item_t>::iterator found_at = tails. find(p);
			if (found_at != tails. end())
				tails. erase(found_at);
		}
	}

	// The filtering out of the enclosures cannot possibly lead to complete
	// emptying of the set of tails.
	assert(! tails. empty());

	// Trace back the tails.
	for (i = tails. begin(); i != tails. end(); ++ i)
	{
		// (A temporary string and the series of front insertions represent a
		//  lazy approach, though in this particular case it's not critical).
		item_t loc = NULL;
		std::string tmp;

		unsigned int len = 0;
		for (item_t p = *i; p != NULL && len < m_trace_max;
				p = p -> get_ancestor())
		{
			// Obtain the name of the target node and the corresponding arc.
			const anta::ndl::node_name<nparse::SG>::type& node =
				p -> get_arc(). get_target(). get_name();
			const std::string& arc = p -> get_arc(). get_label(). get();

			if (! node. empty())
			{
				if (a_reverse)
				{
					if (len > 0)
						tmp. append(" < ");
					tmp. append(encode::string(node));
					tmp. append(1, ':');
					tmp. append(arc);
				}
				else
				{
					if (len > 0)
						tmp. insert(0, " > ");
					tmp. insert(0, arc);
					tmp. insert(0, ':', 1);
					tmp. insert(0, encode::string(node));
				}
				++ len;
			}
			if (len == 1)
				loc = p;
		}

		a_dls. push_back(std::make_pair(
			(loc ? loc : *i) -> get_range(). second, tmp
		));
	}
}

} // namespace anta
