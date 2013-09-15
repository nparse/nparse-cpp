/*
 * @file $/include/anta/dsel/lambda.hpp
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
#ifndef ANTA_DSEL_LAMBDA_HPP_
#define ANTA_DSEL_LAMBDA_HPP_

namespace anta { namespace dsel {

/******************************************************************************/

namespace proto = boost::proto;

// (forward declaration)
template <typename M_> struct lambda;

/**
 *
 */
template <typename M_>
struct domain: proto::domain<proto::generator<lambda<M_>::template type>,
	Grammar<M_> > {};

/**
 *
 */
template <typename M_>
struct lambda
{
	template <typename Expr_>
	struct type: proto::extends<
		Expr_, typename lambda<M_>::template type<Expr_>, domain<M_> >
	{
		typedef proto::extends<
			Expr_, typename lambda<M_>::template type<Expr_>, domain<M_> >
				base_type;

		BOOST_PROTO_EXTENDS_USING_ASSIGN(type)

		type (const Expr_& a_expr = Expr_()):
			base_type (a_expr)
		{
			BOOST_MPL_ASSERT((proto::matches<Expr_, Grammar<M_> >));
		}

		template <typename Context_>
		typename Context_::value_type fn (Context_& a_context) const
		{
			return proto::eval(*this, ContextReader<M_, Context_>(a_context));
		}

		template <typename Context_>
		bool operator() (Context_& a_context) const
		{
			return util::make_bool<M_, true>::from(fn(a_context));
		}

	};

};

/**
 *	An auxiliary structure that defines the type of a unitary lambda-expression
 *	containing a constant.
 */
template <typename M_>
struct constant
{
	typedef typename lambda<M_>::template type<
		typename proto::terminal<typename ndl::context_value<M_>::type>::type>
			type;

	typedef const typename ndl::context_value<M_>::type& initializer;

};

/**
 *	Transforms the given constant value to a unitary lambda-expession.
 */
template <typename M_>
inline typename constant<M_>::type ct (
		typename constant<M_>::initializer a_init)
{
	const typename proto::terminal<typename ndl::context_value<M_>::type>::type
		res = {a_init};
	return res;
}

/**
 *	An auxiliary structure that defines the type of a unitary lambda-expression
 *	containing an address reference to a variable.
 */
template <typename M_>
struct value
{
	typedef typename lambda<M_>::template type<
		typename proto::terminal<Value<M_> >::type> type;

	typedef typename Value<M_>::ref_type initializer;
};

/**
 *	Transforms the given variable reference to a unitary labda-expression.
 */
template <typename M_>
inline typename value<M_>::type val (
		typename value<M_>::initializer a_init)
{
	const typename proto::terminal<Value<M_> >::type res = {a_init};
	return res;
};

/**
 *	An auxiliary structure that defines the type of a unitary lambda-expression
 *	containing a nominal reference to a context variable.
 */
template <typename M_>
struct reference
{
	typedef typename lambda<M_>::template type<
		typename proto::terminal<Reference<M_> >::type> type;

	typedef const typename Reference<M_>::key_type& initializer;

};

/**
 *	Transforms the given context key to a unitary labda-expression.
 */
template <typename M_>
inline typename reference<M_>::type ref (
		typename reference<M_>::initializer a_init)
{
	const typename proto::terminal<Reference<M_> >::type res = {a_init};
	return res;
}

/**
 *	An auxiliary structure that defines the type of a unitary lambda-expression
 *	containing a placeholder (context dependent value or callback).
 */
template <typename M_>
struct placeholder
{
	typedef typename lambda<M_>::template type<
		typename proto::terminal<Placeholder<M_> >::type> type;

	typedef const typename Placeholder<M_>::callback_type& initializer;

};

/**
 *	Transforms the given placeholder to a unitary lambda-expression.
 */
template <typename M_>
inline typename placeholder<M_>::type ph (
		typename placeholder<M_>::initializer a_init)
{
	const typename proto::terminal<Placeholder<M_> >::type res = {a_init};
	return res;
}


/**
 *	Useful placeholder generator functions for the push/pop semantic actions.
 *	@{ */
template <typename M_>
inline typename placeholder<M_>::type push (
		const typename reference<M_>::type& a_ref)
{
	return ph<M_>(ndl::action_push<M_>(proto::value(a_ref). key()));
}

template <typename M_>
inline typename placeholder<M_>::type pop (
		const typename reference<M_>::type& a_ref)
{
	return ph<M_>(ndl::action_pop<M_>(proto::value(a_ref). key()));
}

template <typename M_>
inline typename placeholder<M_>::type delta ()
{
	return ph<M_>(ndl::delta<M_>());
}
/**	@} */

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_LAMBDA_HPP_ */
