/*
 * @file $/source/libnparse_script/src/tokenizer.hpp
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
#ifndef SRC_TOKENIZER_HPP_
#define SRC_TOKENIZER_HPP_

#include <nparse/nparse.hpp>
#include <anta/sas/set.hpp>
#include <anta/sas/token.hpp>

namespace {

using anta::sas::TokenClass;
using anta::sas::SetClass;
using nparse::SG;
using nparse::string_t;

// An auxliliary derivative class for the determination of the character class
// for the Token acceptor (see the grammar definition in constructor).
class Separator: public TokenClass<SG>, public SetClass
{
public:
	template <typename Init_>
	Separator (const Init_& a_init):
		TokenClass<SG> (), SetClass (a_init)
	{
	}

public:
	// NOTE: It's not an overriding of a virtual method!
	bool is_terminal (const string_t::value_type& a_char) const
	{
		// Exclude the Separator charaters from the terminal class.
		return TokenClass<SG>::is_terminal(a_char)
			&& ! SetClass::operator()(a_char);
	}

};

} // namespace

class Tokenizer
{
public:
	typedef nparse::string_t string_t;
	typedef const string_t::value_type* iterator_t;

	static string_t decode (const iterator_t& a_begin, const iterator_t& a_end);

	static string_t decode (const std::pair<iterator_t, iterator_t>& a_range)
	{
		return decode(a_range. first, a_range. second);
	}

	static string_t decode (const string_t& a_token)
	{
		return decode(
			&* a_token. begin(),
			&* a_token. begin() + a_token. size());
	}

};

#endif /* SRC_TOKENIZER_HPP_ */
