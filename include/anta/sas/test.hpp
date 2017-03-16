/*
 * @file $/include/anta/sas/test.hpp
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
#ifndef ANTA_SAS_TEST_HPP_
#define ANTA_SAS_TEST_HPP_

#include <cctype>

namespace anta { namespace sas {

/******************************************************************************/

enum TestFlags
{
	tfSingle = 1,
	tfGreedy = 2,
	tfAcceptEmpty = 4,
	tfNegate = 8
};

template <typename M_, typename Pred_ = int (*)(int)>
class Test: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		typename iterator<M_>::type i = E. second;
		if (m_flags & tfGreedy)
		{
			while	(	i != C. second
					&&	0 != (
							(m_pred(static_cast<int>(*i)) ? 1 : 0)
						^	((m_flags & tfNegate) ? 1 : 0)
						)
					)
			{
				++ i;
				if (m_flags & tfSingle)
					break;
			}
			if ((m_flags & tfAcceptEmpty) || (i != E. second))
				S. push(E. second, i);
		}
		else
		{
			if (m_flags & tfAcceptEmpty)
				S. push(E. second, i);
			while	(	i != C. second
					&&	0 != (
							(m_pred(static_cast<int>(*i)) ? 1 : 0)
						^ 	((m_flags & tfNegate) ? 1 : 0)
						)
					)
			{
				S. push(E. second, ++ i);
				if (m_flags & tfSingle)
					break;
			}
		}
	}

public:
	Test (const Pred_& a_pred, const int a_flags = tfSingle):
		m_pred (a_pred), m_flags (a_flags)
	{
	}

private:
	Pred_ m_pred;
	int m_flags;

};

} // namespace sas

/******************************************************************************/

/**
 *
 *	@{ */

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename Pred_ = int(*)(int)>
struct test_config
{
	Pred_ pred;
	int flags;

	test_config (const Pred_& a_pred, const int a_flags =
											sas::tfSingle | sas::tfGreedy):
		pred (a_pred), flags (a_flags)
	{
	}

};

template <typename Pred_ = int(*)(int)>
class test_functor: public proto::terminal<test_config<Pred_> >::type
{
	test_functor modf (const int a_set, const int a_clr) const
	{
		test_config<Pred_> cfg = proto::value(*this);
		cfg. flags = (cfg. flags | a_set) & ~a_clr;
		return test_functor(cfg);
	}

public:
	test_functor (const test_config<Pred_>& a_cfg):
		proto::terminal<test_config<Pred_> >::type (proto::as_expr(a_cfg))
	{
	}

	/**
	 *	Match currently allowed amount or zero characters.
	 */
	test_functor operator~ () const
	{
		return modf( sas::tfAcceptEmpty, 0 );
	}

	/**
	 *	Match at least one character.
	 */
	test_functor operator+ () const
	{
		return modf( 0, sas::tfSingle | sas::tfAcceptEmpty );
	}

	/**
	 *	Match any number of characters including zero.
	 */
	test_functor operator* () const
	{
		return modf( sas::tfAcceptEmpty, sas::tfSingle );
	}

	/**
	 *	Match any characters not of the class.
	 */
	test_functor operator! () const
	{
		return modf( sas::tfNegate, 0 );
	}

	/**
	 *	Greedy match.
	 */
	test_functor operator++ (int) const
	{
		return modf( sas::tfGreedy, 0 );
	}

	/**
	 *	Generous match.
	 */
	test_functor operator-- (int) const
	{
		return modf( 0, sas::tfGreedy );
	}

};

template <typename M_, typename Pred_>
struct eval_terminal<M_, test_config<Pred_> >
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		const test_config<Pred_>& cfg = proto::value(a_expr);
		return a_rule(sas::Test<M_, Pred_>(cfg. pred, cfg. flags));
	}

};

template <typename M_, typename P_>
struct eval_terminal<M_, sas::Test<M_, P_> >: eval_acceptor<M_> {};

} // namespace detail

namespace terminals {

using namespace detail;
using namespace sas;

const test_functor<> alnum = test_config<>(std::isalnum);
const test_functor<> alpha = test_config<>(std::isalpha);
const test_functor<> cntrl = test_config<>(std::iscntrl);
const test_functor<> digit = test_config<>(std::isdigit);
const test_functor<> graph = test_config<>(std::isgraph);
const test_functor<> lower = test_config<>(std::islower);
const test_functor<> print = test_config<>(std::isprint);
const test_functor<> punct = test_config<>(std::ispunct);
const test_functor<> space = test_config<>(std::isspace,
		tfGreedy | tfAcceptEmpty);
const test_functor<> upper = test_config<>(std::isupper);
const test_functor<> xdigit = test_config<>(std::isxdigit);

}} // namespace ndl::terminals
#endif // defined(ANTA_NDL_RULE_HPP_)

/**	@} */

} // namespace anta

#endif /* ANTA_SAS_TEST_HPP_ */
