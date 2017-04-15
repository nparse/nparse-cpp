/*
 * @file $/include/anta/sas/string.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef ANTA_SAS_STRING_HPP_
#define ANTA_SAS_STRING_HPP_

#include <functional>

namespace anta { namespace sas {

/******************************************************************************/

template <typename M_, typename Compare_ =
	std::equal_to<typename character<M_>::type> >
class String: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		if	(	(! m_partial)
			&&	(m_string. size() > static_cast<typename string_type::size_type>
					(C. second - E. second))
			)
		{
			return ;
		}

		typename iterator<M_>::type i = E. second;
		typename string_type::const_iterator j = m_string. begin();

		if (m_greedy)
		{
			while ((j != m_string. end()) && m_compare(*i, *j))
			{
				++ i;
				++ j;
			}

			if	(	(	m_partial
					&&	(	m_accept_empty
						||	(j != m_string. begin())
						)
					)
				||	(j == m_string. end())
				)
			{
				S. push(E. second, i);
			}
		}
		else
		{
			if (m_partial && m_accept_empty)
			{
				S. push(E. second, i);
			}

			while ((j != m_string. end()) && m_compare(*i, *j))
			{
				++ i;
				++ j;
				S. push(E. second, i);
			}
		}
	}

public:
	typedef typename string<M_>::type string_type;

	String (const string_type& a_string, const bool a_partial = false,
			const bool a_accept_empty = false, const bool a_greedy = true):
		m_string (a_string), m_partial (a_partial),
		m_accept_empty (a_accept_empty), m_greedy (a_greedy)
	{
	}

private:
	Compare_ m_compare;
	string_type m_string;
	bool m_partial;
	bool m_accept_empty;
	bool m_greedy;

};

} // namespace sas

/******************************************************************************/

/**
 *
 *	@{ */

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename M_>
struct eval_string
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		return a_rule(sas::String<M_>(
			encode::make<typename string<M_>::type>::from(
				proto::value(a_expr))));
	}

};

template <typename M_, int N_>
struct eval_terminal<M_, char[N_]>: eval_string<M_> {};

template <typename M_, int N_>
struct eval_terminal<M_, wchar_t[N_]>: eval_string<M_> {};

template <typename M_>
struct eval_terminal<M_, std::string>: eval_string<M_> {};

template <typename M_>
struct eval_terminal<M_, std::wstring>: eval_string<M_> {};

}} // namespace ndl::detail
#endif // defined(ANTA_NDL_RULE_HPP_)

/**	@} */

} // namespace anta

#endif /* ANTA_SAS_STRING_HPP_ */
