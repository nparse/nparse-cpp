/*
 * @file $/include/anta/sas/regex.hpp
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
#ifndef ANTA_SAS_REGEX_HPP_
#define ANTA_SAS_REGEX_HPP_

#include <assert.h>

#if defined(NPARSE_REGEX_XPRESSIVE)
#	include <boost/xpressive/xpressive.hpp>
#else
#	include <boost/regex.hpp>
#endif

namespace anta { namespace sas {

#if defined(NPARSE_REGEX_XPRESSIVE)
namespace rx = boost::xpressive;
#else
namespace rx = boost;
#endif

/******************************************************************************/

template <typename M_>
class RegEx: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		if (m_match)
		{
			if (rx::regex_match(E. first, E. second, m_regex))
				S. push(E. first, E. second);
		}
		else
		{
			rx::match_results<typename iterator<M_>::type> m;
			if (rx::regex_search(E. second, C. second, m, m_regex))
			{
			//	assert( m[0]. first == E. second );
				S. push(m[0]. first, m[0]. second);
			}
		}
	}

public:
#if defined(NPARSE_REGEX_XPRESSIVE)
	typedef rx::basic_regex<typename iterator<M_>::type> regex_type;
#else
	typedef rx::basic_regex<typename character<M_>::type> regex_type;
#endif
	typedef typename string<M_>::type string_type;

	RegEx (const string_type& a_pattern, const bool a_match = false):
		m_match (a_match),
#if defined(NPARSE_REGEX_XPRESSIVE)
		m_regex (regex_type::compile(a_pattern))
#else
		m_regex (a_pattern)
#endif
	{
	}

private:
	bool m_match;
	regex_type m_regex;

};

} // namespace sas

/******************************************************************************/

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename Def_>
struct regex_config
{
	const Def_& def;

	regex_config (const Def_& a_def):
		def (a_def)
	{
	}

};

template <typename M_, typename Def_>
struct eval_terminal<M_, regex_config<Def_> >
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		const regex_config<Def_>& cfg = proto::value(a_expr);
		return a_rule(sas::RegEx<M_>(cfg. def));
	}

};

template <typename M_>
struct eval_terminal<M_, sas::RegEx<M_> >: eval_acceptor<M_> {};

} // namespace detail

namespace terminals {

using namespace detail;
using namespace sas;

template <typename Def_>
typename proto::terminal<regex_config<Def_> >::type regex (const Def_& a_def)
{
	const typename proto::terminal<regex_config<Def_> >::type res =
		{ regex_config<Def_>(a_def) };
	return res;
}

}} // namespace ndl::terminals
#endif // defined(ANTA_NDL_RULE_HPP_)

} // namespace anta

#endif /* ANTA_SAS_REGEX_HPP_ */
