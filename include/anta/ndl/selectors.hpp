/*
 * @file $/include/anta/ndl/selectors.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.6

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
#ifndef ANTA_NDL_SELECTORS_HPP_
#define ANTA_NDL_SELECTORS_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	A selector for an empty value.
 */
template <typename M_>
class empty
{
public:
	empty (const bool a_negate = false):
		m_negate (a_negate)
	{
	}

	// (selector form, e. g. assign and assume)
	const typename context_value<M_>::type& operator() (Traveller<M_>& a_trav)
		const
	{
		return context<M_>::def();
	}

	// (predicate form, e. g. suppose)
	bool operator() (Traveller<M_>& a_trav,
			const typename context_value<M_>::type& a_value) const
	{
		return m_negate ^ ((*this)(a_trav) == a_value);
	}

public:
	empty operator! () const
	{
		return empty(!m_negate);
	}

private:
	bool m_negate;

};

/**
 *	A selector for the accepted range (delta).
 */
template <typename M_>
class delta
{
	template <typename Pair_>
	static typename context_value<M_>::type convert_range (const Pair_& a_pair)
	{
		return typename context_value<M_>::type(a_pair. first, a_pair. second);
	}

public:
	// (selector form, e. g. assign and assume)
	typename context_value<M_>::type operator() (Traveller<M_>& a_trav) const
	{
		return convert_range(a_trav. get_state(). get_range());
	}

	// (predicate form, e. g. suppose)
	bool operator() (Traveller<M_>& a_trav,
			const typename context_value<M_>::type& a_value) const
	{
		return (*this)(a_trav) == a_value;
	}

};

#if 0
/**
 *	An auxiliary type evaluation metafunction that eliminates the unnecessary
 *	copy constructor invokation in case when types node_name<M_>::type and
 *	context_value<M_>::type match each other.
 *	@{ */
template <typename U_, typename V_>
struct reference_on_match
{
	typedef U_ type;

};

template <typename U_>
struct reference_on_match<U_, U_>
{
	typedef U_& type;

};
/**	@} */
#endif

// (forward declaration, defined in node.hpp)
template <typename M_> struct node_name;

/**
 *	A selector for the name of the corresponding node.
 */
template <typename M_>
class node
{
public:
#if 0
	typedef typename reference_on_match<
		const typename context_value<M_>::type,
		const typename node_name<M_>::type
	>::type selector_result_type;
#else
	typedef typename boost::mpl::if_<
			boost::is_same<
				typename context_value<M_>::type,
				typename node_name<M_>::type
			>,
			const typename context_value<M_>::type&,
			typename context_value<M_>::type
		>::type
		selector_result_type;
#endif

	// (selector form, e. g. assign and assume)
	selector_result_type operator() (Traveller<M_>& a_trav) const
	{
		// NOTE: This operator performs an implicit conversion
		// 		 from node_name<M_>::type to context_value<M_>::type in case
		// 		 they're different types, or returns a constant reference
		// 		 otherwise.
		return a_trav. get_state(). get_arc(). get_target(). get_name();
	}

	// (predicate form, e. g. suppose)
	bool operator() (Traveller<M_>& a_trav,
			const typename context_value<M_>::type& a_value) const
	{
		return (*this)(a_trav) == a_value;
	}

};

/**
 *	A selector for a constant value.
 */
template <typename M_>
class value
{
public:
	value (const typename context_value<M_>::type& a_value):
		m_value (a_value)
	{
	}

	// (selector form, e. g. assign and assume)
	const typename context_value<M_>::type& operator() (Traveller<M_>& a_trav)
		const
	{
		return m_value;
	}

	// (predicate form, e. g. suppose)
	bool operator() (Traveller<M_>& a_trav,
			const typename context_value<M_>::type& a_value) const
	{
		return (*this)(a_trav) == a_value;
	}

private:
	typename context_value<M_>::type m_value;

};

/**
 *	A selector for the value of a trace variable.
 */
template <typename M_>
class value_of
{
public:
	value_of (const typename context_key<M_>::type& a_key):
		m_key (a_key)
	{
	}

	// (selector form, e. g. assign and assume)
	const typename context_value<M_>::type& operator() (Traveller<M_>& a_trav)
		const
	{
		return a_trav. val(m_key);
	}

	// (predicate form, e. g. suppose)
	bool operator() (Traveller<M_>& a_trav,
			const typename context_value<M_>::type& a_value) const
	{
		return (*this)(a_trav) == a_value;
	}

private:
	typename context_key<M_>::type m_key;

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_SELECTORS_HPP_ */
