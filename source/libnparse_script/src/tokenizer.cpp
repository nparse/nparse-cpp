/*
 * @file $/source/libnparse_script/src/tokenizer.cpp
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
#include <assert.h>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <boost/regex.hpp>
#include "tokenizer.hpp"

using namespace nparse;

namespace {

class UnSlash
{
public:
	template <typename Match_, typename Out_>
	Out_ operator() (const Match_& a_match, Out_& a_out) const
	{
		assert(a_match. size() > 1);
		assert(a_match[1]. second - a_match[1]. first > 0);
		switch (encode::make<char>::from(*a_match[1]. first))
		{
		case 'u':
			{
				int val = 0;
				std::basic_stringstream<string_t::value_type> buf;
				std::ostream_iterator<string_t::value_type,
					string_t::value_type> out(buf);
				std::copy(a_match[2]. first, a_match[2]. second, out);
				buf << std::setbase(16);
				buf >> val;
				if (buf. fail())
					throw std::runtime_error("invalid character code in a token");
				// @todo: implement multi-byte unicode character support
				*(a_out ++) = val;
				return a_out;
			}

		case '\\':
			*(a_out ++) = '\\';
			return a_out;

		case 't':
			*(a_out ++) = '\t';
			return a_out;

		case 'n':
			*(a_out ++) = '\n';
			return a_out;

		case 'r':
			*(a_out ++) = '\r';
			return a_out;

		default:
			return std::copy(a_match[1]. first, a_match[1]. second, a_out);
		}
	}

};

} // namespace

string_t Tokenizer::decode (const iterator_t& a_begin, const iterator_t& a_end)
{
	if (a_begin == a_end)
		return string_t();

	switch (encode::make<char>::from(*a_begin))
	{
	case '\'':
	case '"':
		break;
	default:
		return string_t(a_begin, a_end);
	}

	// Define a temporary string buffer.
	std::basic_stringstream<string_t::value_type> buf;

	// Prepare a regular expression object that will search for escape sequences
	// within the token definition.
	// @todo: tokenizer regex won't work if string_t is narrow
	buf << L"\\\\(\\\\|u([0-9a-f]{2,4})|[" << *a_begin << L"tnr])";
	const boost::basic_regex<string_t::value_type> escape_sq(buf. str());

	// Clean up temporary string buffer.
	buf. str(string_t(). c_str());
	buf. clear();

	// Translate escape sequences within the token definition by replacing them
	// by the corresponding characters.
	std::ostream_iterator<string_t::value_type, string_t::value_type> out(buf);
	boost::regex_replace(
			out,
			a_begin + 1,
			a_begin + (a_end - a_begin - ((*a_begin == *(a_end-1)) ? 1 : 0)),
			escape_sq,
			UnSlash(),
			boost::match_default | boost::format_all);
	if (buf. fail())
		throw std::runtime_error("token decoding error");

	// Return the decoded string.
	return buf. str();
}
