/*
 * @file $/include/anta/ndl/actions.hpp
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
#ifndef ANTA_NDL_ACTIONS_HPP_
#define ANTA_NDL_ACTIONS_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	The semantic action type.
 */
template <typename M_>
struct action
{
	typedef utility::callback<bool, Traveller<M_>&> type;

};

/**
 *	The semantic action performer.
 */
template <typename M_>
class ActionPerformer
{
public:
	/**
	 *	A container type for the list of attached semantic actions.
	 */
	typedef std::vector<typename action<M_>::type> actions_type;

	/**
	 *	Get a constant reference to the list of attached semantic actions.
	 */
	const actions_type& get_actions () const
	{
		return m_actions;
	}

	/**
	 *	Attach a semantic action to the performer.
	 */
	void attach (const typename action<M_>::type& a_action)
	{
		m_actions. push_back(a_action);
	}

	/**
	 *	Sequentially perform the attached semantic actions.
	 */
	bool entry (Traveller<M_>& a_trav) const
	{
		for (typename actions_type::const_iterator i = m_actions. begin();
				i != m_actions. end(); ++ i)
		{
			if (! (*i)(a_trav))
				return false;
		}
		return true;
	}

private:
	actions_type m_actions; /**< The list of attached semantic actions. */

};

/******************************************************************************/

namespace {

/**
 *	A predicate that always evaluates to true regardless of the argument.
 */
class pTrue
{
public:
	template <typename T_> bool operator() (T_) const { return true; }

};

/**
 *	A predicate that always evaluates to true and also saves a pointer to the
 *	value container of the matched trace variable.
 */
template <typename M_>
class pRef
{
public:
	pRef (const typename context_value<M_>::type*& a_pval):
		m_pval (a_pval)
	{
	}

	bool operator() (const typename context_value<M_>::type& a_value) const
	{
		m_pval = & a_value;
		return true;
	}

private:
	const typename context_value<M_>::type*& m_pval;

};

} // namespace

/******************************************************************************/

/**
 *	Semantic action: assigns the value of the given selector to a trace
 *	variable.
 */
template <typename M_, typename Selector_>
class action_assign
{
public:
	action_assign (const typename context_key<M_>::type& a_key,
			const Selector_& a_selector):
		m_key (a_key), m_selector (a_selector)
	{
	}

	bool operator() (Traveller<M_>& a_trav) const
	{
		a_trav. ref(m_key, true) = m_selector(a_trav); // reset = true
		return true;
	}

private:
	typename context_key<M_>::type m_key;
	Selector_ m_selector;

};

/**
 *	Semantic action: assumes that the value of a certain trace variable equals
 *	to a value which is obtained from the given selector.
 */
template <typename M_, typename Selector_>
class action_assume
{
public:
	action_assume (const typename context_key<M_>::type& a_key,
			const Selector_& a_selector):
		m_key (a_key), m_selector (a_selector)
	{
	}

	bool operator() (Traveller<M_>& a_trav) const
	{
		// Get a pointer to a trace variable identified by the key.
		const typename context_value<M_>::type* pval = NULL;
		if (a_trav. get_state(). is_defined(m_key, pRef<M_>(pval)))
		{
			// If there is such a variable then compare its value to the one
			// provided by the selector.
			return (*pval == m_selector(a_trav));
		}
		else
		{
			// Otherwise, define a new variable identified by the key and assign
			// the value provided by the selector to it.
			a_trav. ref(m_key, true) = m_selector(a_trav); // reset = true
			return true;
		}
	}

private:
	typename context_key<M_>::type m_key;
	Selector_ m_selector;

};

/**
 *	Semantic action: supposes that the value of a certain trace variable matches
 *	the given predicate.
 */
template <typename M_, typename Predicate_>
class action_suppose
{
public:
	action_suppose (const typename context_key<M_>::type& a_key,
			const Predicate_& a_predicate):
		m_key (a_key), m_predicate (a_predicate)
	{
	}

	bool operator() (Traveller<M_>& a_trav) const
	{
		// Get a pointer to a trace variable identified by the key.
		const typename context_value<M_>::type* pval = NULL;
		if (a_trav. get_state(). is_defined(m_key, pRef<M_>(pval)))
		{
			// If there is such a variable then apply the predicate to it.
			return m_predicate(a_trav, *pval);
		}
		return false;
	}

private:
	typename context_key<M_>::type m_key;
	Predicate_ m_predicate;

};

/**
 *	Semantic action: supposes that a certain trace variable has (or has not)
 *	been defined.
 */
template <typename M_>
class action_defined
{
public:
	action_defined (const typename context_key<M_>::type& a_key,
			const bool a_negate = false):
		m_key (a_key), m_negate (a_negate)
	{
	}

	bool operator() (Traveller<M_>& a_trav) const
	{
		return m_negate ^ a_trav. get_state(). is_defined(m_key, pTrue());
	}

public:
	action_defined operator! () const
	{
		return action_defined(m_key, !m_negate);
	}

private:
	typename context_key<M_>::type m_key;
	bool m_negate;

};

/**
 *	Semantic action: applies the given predicate to the length of the current
 *	element in the parser context.
 */
template <typename M_, typename Predicate_>
class action_length
{
public:
	action_length (const Predicate_& a_predicate):
		m_predicate (a_predicate)
	{
	}

	bool operator() (Traveller<M_>& a_trav) const
	{
		const typename ::anta::range<M_>::type& r =
			a_trav. get_state(). get_range();
		return m_predicate(r. second - r. first);
	}

private:
	Predicate_ m_predicate;

};

/**
 *	Semantic action: marks the given trace variable as one that should be stored
 *	in its current state.
 */
template <typename M_>
class action_push
{
public:
	action_push (const typename context_key<M_>::type a_key):
		m_key (a_key)
	{
	}

	bool operator() (Traveller<M_>& a_trav) const
	{
		return a_trav. get_state(). context(&a_trav) -> push(m_key);
	}

private:
	const typename context_key<M_>::type m_key;

};

/**
 *	Semantic action: restores the given trace variable to its previously stored
 *	state.
 */
template <typename M_>
class action_pop
{
public:
	action_pop (const typename context_key<M_>::type a_key):
		m_key (a_key)
	{
	}

	bool operator() (Traveller<M_>& a_trav) const
	{
		return a_trav. get_state(). context(&a_trav) -> pop(m_key);
	}

private:
	const typename context_key<M_>::type m_key;

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_ACTIONS_HPP_ */
