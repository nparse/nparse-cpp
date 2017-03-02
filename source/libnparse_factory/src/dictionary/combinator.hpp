/*
 * @file $/source/libnparse_factory/src/dictionary/combinator.hpp
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
#ifndef SRC_DICTIONARY_COMBINATOR_HPP_
#define SRC_DICTIONARY_COMBINATOR_HPP_

#include <iterator>

namespace dictionary {

typedef std::pair<std::string, std::string> key_value_t;

class Combinator: public std::iterator<std::input_iterator_tag, key_value_t,
	void, const key_value_t*, const key_value_t&>
{
	const entry_t& m_entry;
	std::vector<size_t> m_indices;
	size_t m_offset;
	value_type m_cache;

	void update_cache ()
	{
		m_cache. ~value_type();
		new (&m_cache) value_type(key(), value());
	}

	static const entry_t& dummy_entry ()
	{
		static const entry_t dummy = entry_t();
		return dummy;
	}

public:
	Combinator ():
		m_entry (dummy_entry()), m_offset (0)
	{
	}

	Combinator (const entry_t& a_entry):
		m_entry (a_entry), m_offset (0)
	{
		m_indices. resize(m_entry. second. size(), 0);
		if (! m_indices. empty())
			update_cache();
	}

	Combinator& operator++ ()
	{
		if (++ m_offset >= m_indices. size())
		{
			m_offset = 0;
			size_t i = static_cast<size_t>(-1);
			do
			{
				if (++ i >= m_indices. size())
				{
					m_indices. clear();
					return *this;
				}
				m_indices[i] = (m_indices[i] + 1)
					% m_entry. second[i]. second. size();
			} while (m_indices[i] == 0);
		}
		update_cache();
		return *this;
	}

	Combinator operator++ (int)
	{
		Combinator copy(*this);
		++ (*this);
		return copy;
	}

	bool operator== (const Combinator& a_comb) const
	{
		return m_indices. size() == a_comb. m_indices. size()
			&& m_offset == a_comb. m_offset
			&& std::equal(m_indices. begin(), m_indices. end(),
					a_comb. m_indices. begin());
	}

	bool operator!= (const Combinator& a_comb) const
	{
		return !(*this == a_comb);
	}

	reference operator* () const
	{
		return m_cache;
	}

	pointer operator-> () const
	{
		return &**this;
	}

public:
	const std::string& key () const
	{
		return m_entry. second[m_offset]. first;
	}

	const std::string& value () const
	{
		return m_entry. second[m_offset]. second[m_indices[m_offset]];
	}

	bool pitch () const
	{
		return m_offset == 0;
	}

	bool end () const
	{
		return m_offset == 0 && m_indices. empty();
	}

};

} // namespace dictionary

#endif /* SRC_DICTIONARY_COMBINATOR_HPP_ */
