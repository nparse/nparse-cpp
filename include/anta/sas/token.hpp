/*
 * @file $/include/anta/sas/token.hpp
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
#ifndef ANTA_SAS_TOKEN_HPP_
#define ANTA_SAS_TOKEN_HPP_

namespace anta { namespace sas {

/******************************************************************************/

/**
 *	Default character class determination object.
 */
template <typename M_>
class TokenClass
{
public:
	/**
	 *	Tests whether a character is a space.
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
	 *	Tests whether a character is a quotation mark.
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
	 *	Tests whether a character is an escaping symbol.
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
	 *	Tests whether a character is a terminal.
	 */
	template <typename Char_>
	bool is_terminal (const Char_& a_char) const
	{
		return ! is_space(a_char);
	}

};

/**
 *	The token acceptor.
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
		// The code below implements trivial finite state automaton that accepts
		// either solid character sequences without spaces or quoted sequences
		// that can contain arbitrary characters including quotation marks --
		// they slould be prepended by an escaping character.
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
						return;
					p0 = p; // chop opening spaces off
					state = m_cc. is_quote(*p) ? 3 : 2;
				}
				else
				if (! m_spaces)
					return;
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
					state = 4; // accept any following character
				else
				if (*p == *p0) // test for the same quotation mark
					state = 0;
				break;

			case 4:
				state = 3;
				break;
			}
		}

		if (state == 0)
			S. push(p0, p);
	}

public:
	Token (const Class_& a_cc = Class_(), const bool a_spaces = true):
		m_cc (a_cc), m_spaces (a_spaces)
	{
	}

private:
	Class_ m_cc;
	bool m_spaces;

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
	bool spaces;

	token_config (const Class_& a_cc, const bool a_spaces):
		cc (a_cc), spaces (a_spaces)
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
		return a_rule(sas::Token<M_, Class_>(cfg. cc, cfg. spaces));
	}

};

template <typename M_, typename C_>
struct eval_terminal<M_, sas::Token<M_, C_> >: eval_acceptor<M_> {};

} // namespace detail

namespace terminals {

using namespace detail;

template <typename Class_>
typename proto::terminal<token_config<Class_> >::type token (
		const Class_& a_cc = Class_(), const bool a_spaces = true)
{
	const typename proto::terminal<token_config<Class_> >::type res =
		{ token_config<Class_>(a_cc, a_spaces) };
	return res;
}

}} // namespace ndl::terminals
#endif

/**	@} */

} // namespace anta

#endif /* ANTA_SAS_TOKEN_HPP_ */
