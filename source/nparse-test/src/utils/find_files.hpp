/*
 * @file $/source/nparse-test/src/utils/find_files.hpp
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
#ifndef SRC_UTILS_FIND_FILES_HPP_
#define SRC_UTILS_FIND_FILES_HPP_

#include <boost/filesystem.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/iterator/filter_iterator.hpp>

namespace fs = boost::filesystem;
namespace xp = boost::xpressive;

class find_files
{
	class name_check
	{
	public:
		name_check ()
		{
		}

		name_check (const char* a_pattern):
			m_regex (xp::sregex::compile(a_pattern))
		{
		}

		template <typename P_>
		name_check (const P_& a_pattern):
			m_regex (a_pattern)
		{
		}

		bool operator() (const fs::directory_entry& a_entry) const
		{
			return xp::regex_match(a_entry. path(). filename(). string(),
					m_regex);
		}

	private:
		xp::sregex m_regex;

	};

public:
	typedef fs::recursive_directory_iterator base_iterator;
	typedef boost::filter_iterator<name_check, base_iterator> iterator;
	typedef iterator const_iterator;
	typedef iterator::value_type value_type;

	const_iterator begin () const
	{
		return const_iterator(m_name_check, m_base_iterator);
	}

	const_iterator end () const
	{
		return const_iterator();
	}

	template <typename P_>
	find_files (const char* a_path, const P_& a_pattern):
		m_name_check (a_pattern), m_base_iterator (a_path)
	{
	}

private:
	name_check m_name_check;
	base_iterator m_base_iterator;

};

#endif /* SRC_UTILS_FIND_FILES_HPP_ */
