/*
 * @file $/include/anta/sas/token.hpp
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
#ifndef ANTA_SAS_TOKEN_HPP_
#define ANTA_SAS_TOKEN_HPP_

namespace anta { namespace sas {

// (forward declaration)
template <typename M_> class TokenClass;

/******************************************************************************/

/**
 *	The token acceptor.
 *
 *	The code in accept(C,E,S) member function implements a trivial finite state
 *	automaton that consumes character sequences without spaces or quoted
 *	character sequences that can contain spaces.
 *
 *	Quotation marks that do not designate token boundaries must be screened,
 *	that is, prepended with an escape character. An escape character itself can
 *	be screened too (with another escape character), in which case it is
 *	consumed as a regular (terminal) character.
 *
 *	Example:
 *		token_without_spaces
 *		"token with spaces"
 *		"quoted token containing (\") quotation mark"
 *		"quoted token containing (\\) escape character"
 *
 * 	A character class, namely Terminals, Spaces, Quotation Marks or Escape
 * 	Characters, is determined for each consumed character by means of provided
 * 	TokenClass<M_> helper specified as the second template parameter.
 */
template <typename M_, typename Class_ = TokenClass<M_> >
class Token: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		typename iterator<M_>::type p0 = E. second, p = p0;
		int state;
		for (state = 1; p != C. second && state != 0; ++ p)
		{
			switch (state)
			{
			case 1:
				if (! m_cc. is_space(*p))
				{
					if (! m_cc. is_terminal(*p))
					{
						return;
					}
					p0 = p; // chop off opening spaces
					state = m_cc. is_quote(*p) ? 3 : 2;
				}
				else if (! m_skip_spaces)
				{
					return;
				}
				break;

			case 2:
				if (! m_cc. is_terminal(*p))
				{
					-- p; // as opposed to the closing quotation mark the
						  // trailing non-terminal is not counted
					state = 0;
				}
				break;

			case 3:
				if (m_cc. is_escape(*p))
				{
					state = 4; // accept any following character
				}
				else if (*p == *p0) // test for the same quotation mark
				{
					state = 0;
				}
				break;

			case 4:
				state = 3;
				break;
			}
		}

		if (state == 0)
		{
			S. push(p0, p);
		}
	}

public:
	Token (const Class_& a_cc = Class_(), const bool a_skip_spaces = true):
		m_cc (a_cc), m_skip_spaces (a_skip_spaces)
	{
	}

private:
	Class_ m_cc;
	bool m_skip_spaces;

};

/**
 *	TokenClass<M_> is the default model-specific character class deremination
 *	helper used by the token acceptor.
 */
template <typename M_>
class TokenClass
{
public:
	/**
	 *	Test whether the given character is a space.
	 */
	template <typename Char_>
	bool is_space (const Char_& a_char) const
	{
		switch (static_cast<wchar_t>(a_char))
		{
		case L'\x20':
		case L'\t':
		case L'\r':
		case L'\n':
			return true;
		}
		return false;
	}

	/**
	 *	Test whether the given character is a quotation mark.
	 */
	template <typename Char_>
	bool is_quote (const Char_& a_char) const
	{
		switch (static_cast<wchar_t>(a_char))
		{
		case L'\'':
		case L'"':
			return true;
		}
		return false;
	}

	/**
	 *	Test whether the given character is an escape character.
	 */
	template <typename Char_>
	bool is_escape (const Char_& a_char) const
	{
		switch (static_cast<wchar_t>(a_char))
		{
		case L'\\':
			return true;
		}
		return false;
	}

	/**
	 *	Test whether the given character is a terminal.
	 */
	template <typename Char_>
	bool is_terminal (const Char_& a_char) const
	{
		return ! is_space(a_char);
	}

};

} // namespace sas

/******************************************************************************/

/**
 *
 *	@{ */

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename Class_>
struct token_config
{
	const Class_& cc;
	bool skip_spaces;

	token_config (const Class_& a_cc, const bool a_skip_spaces):
		cc (a_cc), skip_spaces (a_skip_spaces)
	{
	}

};

template <typename M_, typename Class_>
struct eval_terminal<M_, token_config<Class_> >
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		const token_config<Class_>& cfg = proto::value(a_expr);
		return a_rule(sas::Token<M_, Class_>(cfg. cc, cfg. skip_spaces));
	}

};

template <typename M_, typename C_>
struct eval_terminal<M_, sas::Token<M_, C_> >: eval_acceptor<M_> {};

} // namespace detail

namespace terminals {

using namespace detail;

template <typename Class_>
typename proto::terminal<token_config<Class_> >::type token (
		const Class_& a_cc = Class_(), const bool a_skip_spaces = true)
{
	const typename proto::terminal<token_config<Class_> >::type res =
		{ token_config<Class_>(a_cc, a_skip_spaces) };
	return res;
}

}} // namespace ndl::terminals
#endif

/**	@} */

} // namespace anta

#endif /* ANTA_SAS_TOKEN_HPP_ */
