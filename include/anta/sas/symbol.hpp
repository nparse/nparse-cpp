/*
 * @file $/include/anta/sas/symbol.hpp
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
#ifndef ANTA_SAS_SYMBOL_HPP_
#define ANTA_SAS_SYMBOL_HPP_

#include <encode/encode.hpp>

namespace anta { namespace sas {

/******************************************************************************/

template <typename M_>
class Symbol: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		if ((E. second != C. second) && (*E. second == m_symbol))
			S. push(E. second, E. second + 1);
	}

public:
	Symbol (const typename character<M_>::type& a_symbol):
		m_symbol (a_symbol)
	{
	}

private:
	typename character<M_>::type m_symbol;

};

} // namespace sas

/******************************************************************************/

/**
 *
 *	@{ */

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename M_>
struct eval_character
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		return a_rule(sas::Symbol<M_>(
			encode::make<typename character<M_>::type>::from(
				proto::value(a_expr))));
	}

};


template <typename M_>
struct eval_terminal<M_, char>: eval_character<M_> {};

template <typename M_>
struct eval_terminal<M_, wchar_t>: eval_character<M_> {};

}} // namespace ndl::detail
#endif // defined(ANTA_NDL_RULE_HPP_)

/**	@} */

} // namespace anta

#endif /* ANTA_SAS_SYMBOL_HPP_ */
