/*
 * @file $/include/anta/ndl.1/network.hpp
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
#ifndef ANTA_NDL_NETWORK_HPP_
#define ANTA_NDL_NETWORK_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	The Network object is used for building complete acceptor network topologies
 *	form the sets of determinative rules.
 */
template <typename M_>
class Network: public Grammar<M_>
{
public:
	/**
	 *	The only constructor.
	 */
	Network ()
	{
	}

public:
	//
	//	Quick semantic action support.
	//

#define ANTA_NDL_NETWORK_DECLARE_CUSTOM(ACTION) \
	template <typename Selector_> \
	static action_##ACTION<M_, Selector_> ACTION ( \
			const typename context_key<M_>::type& a_key, \
			const Selector_& a_selector) \
	{ \
		return action_##ACTION<M_, Selector_>(a_key, a_selector); \
	}

#define ANTA_NDL_NETWORK_DECLARE_DELTA(ACTION) \
	static action_##ACTION<M_, delta<M_> > ACTION ( \
			const typename context_key<M_>::type& a_key) \
	{ \
		return action_##ACTION<M_, delta<M_> >(a_key, delta<M_>()); \
	}

#define ANTA_NDL_NETWORK_DECLARE_VAL(ACTION) \
	static action_##ACTION<M_, value<M_> > ACTION##_val( \
			const typename context_key<M_>::type& a_key, \
			const typename context_value<M_>::type& a_value) \
	{ \
		return action_##ACTION<M_, value<M_> >(a_key, value<M_>(a_value)); \
	}

#define ANTA_NDL_NETWORK_DECLARE_REF(ACTION) \
	static action_##ACTION<M_, value_of<M_> > ACTION##_ref( \
			const typename context_key<M_>::type& a_dst, \
			const typename context_key<M_>::type& a_src) \
	{ \
		return action_##ACTION<M_, value_of<M_> >(a_dst, \
				value_of<M_>(a_src)); \
	}

	/**
	 *	Assigns the value of the given selector to a trace variable.
	 */
	ANTA_NDL_NETWORK_DECLARE_CUSTOM(assign)
	ANTA_NDL_NETWORK_DECLARE_DELTA(assign)
	ANTA_NDL_NETWORK_DECLARE_VAL(assign)
	ANTA_NDL_NETWORK_DECLARE_REF(assign)

	/**
	 *	Assumes that the value of a certain trace variable equals to a value
	 *	which is obtained from the given selector.
	 */
	ANTA_NDL_NETWORK_DECLARE_CUSTOM(assume)
	ANTA_NDL_NETWORK_DECLARE_DELTA(assume)
	ANTA_NDL_NETWORK_DECLARE_VAL(assume)
	ANTA_NDL_NETWORK_DECLARE_REF(assume)

	/**
	 *	Supposes that the value of a certain trace variable matches the given
	 *	predicate.
	 */
	ANTA_NDL_NETWORK_DECLARE_CUSTOM(suppose)
	ANTA_NDL_NETWORK_DECLARE_DELTA(suppose)
	ANTA_NDL_NETWORK_DECLARE_VAL(suppose)
	ANTA_NDL_NETWORK_DECLARE_REF(suppose)

// Removing no more necessary macroses.
#undef ANTA_NDL_NETWORK_DECLARE_CUSTOM
#undef ANTA_NDL_NETWORK_DECLARE_DELTA
#undef ANTA_NDL_NETWORK_DECLARE_VAL
#undef ANTA_NDL_NETWORK_DECLARE_REF

	/**
	 *	A shortcut for the empty value selector.
	 */
	static ::anta::ndl::empty<M_> empty ()
	{
		return ::anta::ndl::empty<M_>();
	}

	/**
	 *	Supposes that a certain trace variable has been defined.
	 */
	static action_defined<M_> defined (
			const typename context_key<M_>::type& a_key)
	{
		return action_defined<M_>(a_key);
	}

	/**
	 *	Applies the given predicate to the length of the current element in the
	 *	parser context.
	 */
	template <typename Predicate_>
	static action_length<M_, Predicate_> length (
			const Predicate_& a_predicate)
	{
		return action_length<M_, Predicate_>(a_predicate);
	}

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_NETWORK_HPP_ */
