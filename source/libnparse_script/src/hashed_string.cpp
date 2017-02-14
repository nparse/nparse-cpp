/*
 * @file $/source/libnparse_script/src/hashed_string.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.6

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
#include <assert.h>
#include <stdexcept>
#include <boost/unordered_map.hpp>
#include <nparse/util/hashed_string.hpp>
#include <utility/hash_gen.hpp>

typedef boost::unordered_map<hashed_string::result_type, std::string> hashes_t;
hashes_t g_hashes;

void hashed_string::init (const std::string& a_str)
{
	if (a_str. empty())
	{
		m_hash = 0;
		return;
	}

	const hashed_string::result_type h =
		utility::hash_gen<hashed_string::result_type>::f(a_str. begin(), a_str. end())
			<< 1;

	if (*a_str. begin() == '_')
	{
		m_hash = h | 1;
	}
	else
	{
		m_hash = h & ~static_cast<hashed_string::result_type>(1);
	}

	hashes_t::const_iterator found_at;
	while	(	(found_at = g_hashes. find(m_hash)) != g_hashes. end()
			&&	found_at -> second != a_str
			)
	{
		m_hash += 2;
	}

	if (found_at == g_hashes. end())
	{
		const std::pair<hashes_t::iterator, bool> p = g_hashes. insert(
				hashes_t::value_type(m_hash, a_str));
		assert(p. second);
	}
}

hashed_string::operator const std::string& () const
{
	if (m_hash != 0)
	{
		hashes_t::const_iterator found_at = g_hashes. find(m_hash);
		if (found_at == g_hashes. end())
			throw std::logic_error("hashed_string: inconsistency detected");
		return found_at -> second;
	}
	else
	{
		static const std::string sc_empty = std::string();
		return sc_empty;
	}
}
