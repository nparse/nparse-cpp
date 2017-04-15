/*
 * @file $/source/libnparse_factory/src/dictionary/acceptor_red.hpp
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
#ifndef SRC_DICTIONARY_ACCEPTOR_RED_HPP_
#define SRC_DICTIONARY_ACCEPTOR_RED_HPP_

// NODE: RED stands for Regular Expression Dictionary, also RED means slow!

#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <nparse/nparse.hpp>
#include "dictionary.hpp"
#include "combinator.hpp"

namespace dictionary {

namespace xp = boost::xpressive;

template <typename M_>
struct regex
{
	typedef xp::basic_regex<typename anta::iterator<M_>::type> type;

};

struct reset_adapter
{
	typedef void result_type;

	result_type operator() (Combinator& a_comb, const entry_t& a_entry) const
	{
	//	a_comb = Combinator(a_entry);
		a_comb. ~Combinator();
		new(&a_comb) Combinator(a_entry);
	}

};

template <typename M_>
typename regex<M_>::type compile_pattern (
		const Dictionary::const_iterator& a_begin,
		const Dictionary::const_iterator& a_end, Combinator& a_comb)
{
	using namespace xp;
	static const function<reset_adapter>::type reset = {{}};
#if 1 // bypassing the recursion using a deep copy trick
	typename regex<M_>::type rx;
	for (Dictionary::const_iterator i = a_begin; i != a_end; ++ i)
	{
		const typename regex<M_>::type rxi =
			regex<M_>::type::compile(
				encode::make<typename anta::string<M_>::type>::from(i -> first))
			[ reset(xp::ref(a_comb), xp::ref(*i)) ];
		rx = (i == a_begin) ? rxi : boost::proto::deep_copy( rxi | rx );
	}
	return rx;
#else
	const typename regex<M_>::type rxi =
		regex<M_>::type::compile(
			encode::make<typename anta::string<M_>::type>::from(
				a_begin -> first))
		[ reset(ref(a_comb), ref(*a_begin)) ];
	return (a_begin + 1 == a_end)
			? rxi
			: rxi | compile_pattern<M_>(a_begin + 1, a_end, a_comb);
#endif
}

template <typename M_>
class AcceptorRED: public anta::Acceptor<M_>
{
	typedef dictionary::Dictionary dict_type;

public:
	// Overridden from Acceptor<M_>:

	void accept (const typename anta::range<M_>::type& C,
			const typename anta::range<M_>::type& E,
			typename anta::spectrum<M_>::type& S) const
	{
	// NOTE: When the following call to regex_search succeeds it implicitly
	//		 links the m_combinator member instance to the matched dictionary
	//		 entry. Therefore, the m_combinator field is implicitly mutable.

		xp::match_results<typename anta::iterator<M_>::type> m;
		if (! xp::regex_search(E. second, C. second, m, m_pattern))
			return;

		anta::State<M_>* state = NULL;
		for (Combinator& i = m_combinator; ! i. end(); ++ i)
		{
			if (i. pitch())
			{
				state = S. push(m[0]. first, m[0]. second);
			}
			state -> ref(i. key(), S, true) = i. value();
		}

		if (state == NULL)
		{
			S. push(m[0]. first, m[0]. second);
		}
	}

public:
	AcceptorRED (const std::string& a_filename)
	{
		const int err_line = m_dict. load(a_filename);
		if (err_line != 0)
		{
			using namespace nparse;
			throw ex::syntax_error()
				<< ex::file(a_filename)
				<< ex::line(err_line)
				<< ex::message("invalid dictionary entry");
		}

		m_pattern = compile_pattern<M_>(m_dict. begin(), m_dict. end(),
				m_combinator);
	}

private:
	dict_type m_dict;
	xp::basic_regex<typename anta::iterator<M_>::type> m_pattern;
	mutable Combinator m_combinator;

};

} // namespace dictionary

#endif /* SRC_DICTIONARY_ACCEPTOR_RED_HPP_ */
