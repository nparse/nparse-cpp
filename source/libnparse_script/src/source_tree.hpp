/*
 * @file $/source/libnparse_script/src/source_tree.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef SRC_SOURCE_TREE_HPP_
#define SRC_SOURCE_TREE_HPP_

#include <assert.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <encode/encode.hpp>

namespace fs = boost::filesystem;

template <typename String_>
class SourceTree
{
	typedef std::vector<typename String_::size_type> offsets_t;
	typedef boost::tuple<fs::path, String_, offsets_t, int> import_queue_item_t;
	typedef boost::ptr_vector<import_queue_item_t> import_queue_t;
	import_queue_t m_import_queue;
	typename import_queue_t::iterator m_current_import;
	fs::path m_current_path;

	// Quick'n'Dirty single element tuple comparison functor.
	template <typename U_, int N_ = 0>
	class compare
	{
		const U_& m_ref;

	public:
		compare (const U_& a_ref):
			m_ref (a_ref)
		{
		}

		template <typename A_>
		bool operator() (const A_& arg) const
		{
			return m_ref == boost::get<N_>(arg);
		}

	};

public:
	typedef const typename String_::value_type* iterator;
	typedef std::pair<iterator, iterator> range;

	SourceTree ():
		m_current_import (m_import_queue. end()),
		m_current_path (fs::current_path())
	{
	}

	void import (const std::string& a_path, const bool a_reset, int a_hint)
	{
#if 1
		const fs::path pp = fs::absolute(m_current_path / a_path);
#else
		fs::path pp;
		if (! a_path. empty() && a_path[0] == '/')
			pp = a_path;
		else
		if (a_path. size() >= 2 && a_path[0] == '.' && a_path[1] == '/')
			pp = fs::absolute(m_current_path / a_path. substr(2));
		else
			pp = fs::absolute(m_current_path / a_path;
#endif
		if (std::find_if(m_import_queue. begin(), m_import_queue. end(),
					compare<fs::path, 0>(pp)) == m_import_queue. end())
		{
			const typename import_queue_t::size_type offset =
				m_current_import - m_import_queue. begin();

			import_queue_item_t* ptr = new import_queue_item_t(pp, "",
				offsets_t(), a_hint);
			m_import_queue. push_back(ptr);

			m_current_import = m_import_queue. begin() + offset;
		}

		if (a_reset)
		{
			assert(! m_import_queue. empty());
			m_current_import = m_import_queue. begin();
		}
	}

	bool load (range& a_range)
	{
		if (m_current_import == m_import_queue. end())
			return false;

		const fs::path& filepath = boost::get<0>(*m_current_import);
		std::ifstream file(filepath. c_str());
		if (!file)
			throw std::runtime_error("unable to read file");

		m_current_path = filepath. parent_path();

		String_ contents;
		{
			std::string buf;
			while (std::getline(file, buf, '\x00'))
				contents += encode::make<String_>::from(buf);
			file. close();
		}

		offsets_t offsets;
		{
			typename String_::size_type nl = 0;
			while ((nl = contents. find('\n', nl)) != contents. npos)
				offsets. push_back(nl ++);
		}

		boost::get<1>(*m_current_import). swap(contents);
		boost::get<2>(*m_current_import). swap(offsets);

		a_range. first = &* boost::get<1>(*m_current_import). begin();
		a_range. second = a_range. first + boost::get<1>(*m_current_import). size();

		++ m_current_import;
		return true;
	}

	bool identify (const iterator& a_iterator, std::string& a_file, int& a_line,
			int& a_offset) const
	{
		for (typename import_queue_t::const_iterator
				i = m_import_queue. begin(); i != m_import_queue. end(); ++ i)
		{
			const String_& contents = boost::get<1>(*i);
			range r (&* contents. begin(), &* contents. begin() + contents. size());
			if (r. first <= a_iterator && a_iterator <= r. second)
			{
				const typename String_::size_type offset =
					a_iterator - r. first;

				const typename offsets_t::const_iterator begin =
					boost::get<2>(*i). begin();

				const typename offsets_t::const_iterator end =
					boost::get<2>(*i). end();

				const typename offsets_t::const_iterator offset_at =
					std::lower_bound(begin, end, offset);

				a_file = boost::get<0>(*i). string();
				a_line = offset_at - begin + 1;
				a_offset = offset -
					((begin == offset_at) ? 0 : *(offset_at - 1));

				return true;
			}
		}

		a_file. clear();
		a_line = -1;
		a_offset = -1;
		return false;
	}

	std::string current_path () const
	{
		return m_current_path. string();
	}

	int current_hint () const
	{
		return boost::get<3>(*m_current_import);
	}

};

#endif /* SRC_SOURCE_TREE_HPP_ */
