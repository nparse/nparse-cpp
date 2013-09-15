/*
 * @file $/include/anta/ndl/rule.hpp
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
#ifndef ANTA_NDL_RULE_HPP_
#define ANTA_NDL_RULE_HPP_

namespace anta { namespace ndl {

namespace proto = boost::proto;

/******************************************************************************/

/**
 *
 */
template <typename M_>
class Rule: public proto::terminal<Cluster<M_> >::type
{
	typedef typename proto::terminal<Cluster<M_> >::type base_type;

public:
	/**
	 *	The default constructor.
	 */
	Rule ():
		base_type ()
	{
	}

	/**
	 *	The naming constructor.
	 */
	Rule (const typename node_name<M_>::type& a_name):
		base_type (proto::as_expr(Cluster<M_>(a_name)))
	{
	}

	/**
	 *	The definition constructor.
	 *
	template <typename Expr_>
	Rule (const Expr_& a_expr)
	{
		*this = a_expr;
	}
	 */

public:
	/**
	 *	An auxiliary structure that makes the Rule<...> template class able to
	 *	serve as a boost::proto evaluation context.
	 */
	template <typename Expr_, typename Tag_ =
		typename proto::tag_of<Expr_>::type> struct eval;

	/**
	 *	The rule definition operator.
	 */
	template <typename Expr_>
	uint_t operator= (const Expr_& a_expr)
	{
		return (proto::value(*this) = proto::eval(a_expr, *this));
	}

	/**
	 *	Acceptor copying operator. Should not be invoked directly.
	 */
	template <typename Acceptor_>
	JointJump<M_> operator() (const Acceptor_& a_acceptor)
	{
		m_acceptors. push_back(new Acceptor_(a_acceptor));
		return JointJump<M_>(m_acceptors. back(), Label<M_>(true));
	}

	/**
	 *	Get a constant/mutable reference to the wrapped cluster.
	 *	@{ */
	Cluster<M_>& cluster ()
	{
		return proto::value(*this);
	}

	const Cluster<M_>& cluster () const
	{
		return proto::value(*this);
	}
	/**	@} */

	/**
	 *	Cast the rule as an acceptor network node.
	 *	@{ */
	operator ::anta::Node<M_>& ()
	{
		return cluster();
	}

	operator const ::anta::Node<M_>& () const
	{
		return cluster();
	}
	/**	@} */

	/**
	 *	Attach an initial semantic action to the rule.
	 */
	Rule& operator[] (const typename action<M_>::type& a_action)
	{
		cluster()[a_action];
		return *this;
	}

private:
	boost::ptr_vector<Acceptor<M_> > m_acceptors;

};

namespace detail {

/**
 *	Default evaluation of constants.
 */
template <typename M_, typename T_>
struct eval_terminal
{
	typedef const T_& result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>&) const
	{
		return static_cast<result_type>(proto::value(a_expr));
	}

};

/**
 *	Evaluation of node references.
 *	@{ */
template <typename M_>
struct eval_terminal<M_, ::anta::Node<M_> >
{
	typedef JointCall<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>&) const
	{
		return result_type(proto::value(a_expr));
	}

};

template <typename M_>
struct eval_terminal<M_, Node<M_> >: eval_terminal<M_, ::anta::Node<M_> > {};

template <typename M_>
struct eval_terminal<M_, Cluster<M_> >: eval_terminal<M_, ::anta::Node<M_> > {};

/**	@} */

/**
 *	Evaluation of acceptors.
 *	@{ */
template <typename M_>
struct eval_acceptor
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		return a_rule(proto::value(a_expr));
	}

};

template <typename M_>
struct eval_terminal<M_, UnconditionalRepeat<M_> >: eval_acceptor<M_> {};

template <typename M_>
struct eval_terminal<M_, UnconditionalShift<M_> >: eval_acceptor<M_> {};

/**	@} */

} // namespace detail

/**
 *	Rule<...>::eval<...> terminal and non-terminal specializations.
 *	@{ */

// all kinds of nonterminals
template <typename M_>
template <typename Expr_, typename Tag_>
struct Rule<M_>::eval:
	proto::default_eval<Expr_, Rule<M_> > {};

// terminals only
template <typename M_>
template <typename Expr_>
struct Rule<M_>::eval<Expr_, proto::tag::terminal>:
	detail::eval_terminal<M_, typename proto::result_of::value<Expr_>::type> {};

/**	@} */

/**
 *	Rule<...>::eval<...> specialization for semantic action attachment
 *	operators (ampersand, parentheses, subscript).
 *	@{ */

template <typename M_>
template <typename Expr_>
struct Rule<M_>::eval<Expr_, proto::tag::bitwise_and>
{
	typedef JointStmt<M_> result_type;

	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		return proto::eval(proto::left(a_expr), a_rule)
			& typename action<M_>::type(proto::right(a_expr));
	}

};

template <typename M_>
template <typename Expr_>
struct Rule<M_>::eval<Expr_, proto::tag::function>
{
	typedef JointStmt<M_> result_type;

	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		BOOST_STATIC_ASSERT( proto::arity_of<Expr_>::value == 2 );
		return proto::eval(proto::left(a_expr), a_rule)
			( typename action<M_>::type(proto::right(a_expr)) );
	}

};

template <typename M_>
template <typename Expr_>
struct Rule<M_>::eval<Expr_, proto::tag::subscript>
{
	typedef JointAlt<M_> result_type;

	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		return proto::eval(proto::left(a_expr), a_rule)
			[ typename action<M_>::type(proto::right(a_expr)) ];
	}

};

/**	@} */

/**
 *
 *	@{ */

namespace detail {

struct pass_config {};

template <typename M_>
struct eval_terminal<M_, pass_config>
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_&, Rule<M_>& a_rule) const
	{
		return a_rule(UnconditionalRepeat<M_>());
	}

};

} // namespace detail

namespace terminals {

const proto::terminal<detail::pass_config>::type pass = {{}};

} // namespace terminals

/**	@} */

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_RULE_HPP_ */
