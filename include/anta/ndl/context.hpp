/*
 * @file $/include/anta/ndl/context.hpp
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
#ifndef ANTA_NDL_CONTEXT_HPP_
#define ANTA_NDL_CONTEXT_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

// (forward declarations)
template <typename M_> class Context;
template <typename M_> class ContextOwner;

/**
 *	The default key type for the trace variables.
 */
template <typename M_>
struct context_key
{
	typedef std::string type;

};

/**
 *	The default value type for the trace variables.
 */
template <typename M_>
struct context_value
{
	typedef typename string<M_>::type type;

};

/**
 *	The trace context entry (a pair of key and value).
 */
template <typename M_>
struct context_entry
{
// NOTE: Usage of map<...>::value_type instead of pair<...> (see the commented
//		 out lines below) prevents from unnecessary copying of trace variables
//		 while a trace context is being listed.

//	typedef std::pair<typename context_key<M_>::type,
//			typename context_value<M_>::type> type;

	typedef typename std::map<typename context_key<M_>::type,
			typename context_value<M_>::type>::value_type type;

};

template <typename M_>
struct context
{
	typedef typename context_key<M_>::type key_type;
	typedef typename context_value<M_>::type value_type;
	typedef typename context_entry<M_>::type entry_type;

	/**
	 *	Get the default value for the trace variable.
	 */
	static const value_type& def ()
	{
		static const value_type sc_empty = value_type();
		return sc_empty;
	}

	class type
	{
	public:
		type (const Context<M_>* a_ancestor = NULL):
			m_simple_ptr (NULL), m_shared_ptr ()
		{
			if (a_ancestor != NULL)
			{
				m_simple_ptr = a_ancestor -> derive();
			}
			else
			{
				m_shared_ptr. reset(new Context<M_>());
			}
		}

		Context<M_>& operator* () const
		{
			return *(m_simple_ptr ? m_simple_ptr : m_shared_ptr. get());
		}

		Context<M_>* operator-> () const
		{
			return &**this;
		}

	private:
		Context<M_>* m_simple_ptr;
		boost::shared_ptr<Context<M_> > m_shared_ptr;

	};

};

/**
 *	The trace context (the holder object for the trace variables).
 */
template <typename M_>
class Context
{
public:
	/**
	 *	Necessary type definitions for meeting the requirements of the Context
	 *	concept.
	 *	@{ */
	typedef typename context_key<M_>::type key_type;
	typedef typename context_value<M_>::type value_type;
	/**	@} */

	/**
	 *	The constructor for an initial or a derived context.
	 */
	Context (const Context* a_ancestor = NULL,
			ContextOwner<M_>* a_owner = NULL):
		m_ancestor (a_ancestor), m_owner (a_owner)
	{
	}

	/**
	 *	Derive a new Context instance using this instance as the ancestor.
	 */
	Context* derive () const
	{
		if (m_owner == NULL)
			throw std::logic_error("derivation is impossible because this"
					" Context instance does not have an owner");
		return m_owner -> create(this);
	}

	/**
	 *	Get a mutable reference to a trace variable from the local context.
	 */
	value_type& ref (const key_type& a_key, const bool a_reset = false)
	{
		typename variables_t::iterator found_at = m_variables. find(a_key);
		if (found_at == m_variables. end())
		{
			const std::pair<typename variables_t::iterator, bool> p =
				m_variables. insert(typename variables_t::value_type(a_key,
							context<M_>::def()));
			assert(p. second);
			found_at = p. first;
			if (! a_reset)
			{
				// NOTE: Trace variable assignment operator acts NOT the same
				//		 way as the trace variable copy constructor. In general,
				//		 it might use value derivation mechanism which differs
				//		 from simple value copying.
				found_at -> second = val(a_key, m_ancestor);
			}
		}
		return found_at -> second;
	}

	/**
	 *	Get a constant reference to a trace variable from the global context.
	 */
	const value_type& val (const key_type& a_key) const
	{
		return val(a_key, this);
	}

	/**
	 *	Check whether a trace variable is defined within the global context.
	 */
	template <typename Predicate_>
	bool is_defined (const key_type& a_key, const Predicate_& a_predicate) const
	{
		for (const Context* c = this; c != NULL; c = c -> m_ancestor)
		{
			typename variables_t::const_iterator found_at =
				c -> m_variables. find(a_key);
			if (found_at != c -> m_variables. end())
				return a_predicate(found_at -> second);
		}
		return false;
	}

private:
	/**
	 *	Search for a certain variable within the global context starting form
	 *	the given ancestor.
	 */
	const value_type& val (const key_type& a_key, const Context* c) const
	{
		for ( ; c != NULL; c = c -> m_ancestor)
		{
			typename variables_t::const_iterator found_at =
				c -> m_variables. find(a_key);
			if (found_at != c -> m_variables. end())
				return found_at -> second;
		}
		return context<M_>::def();
	}

private:
	/**
	 *	A helper predicate that filters out empty and repeated variables from
	 *	the context.
	 */
	class list_filter
	{
	public:
		list_filter (std::set<key_type>& a_passed):
			m_passed (a_passed)
		{
		}

		bool operator() (const typename context_entry<M_>::type& a_pair)
		{
			// Check if the key is "special".
			if (a_pair. first. empty())
				return true;
			// Check if the key has been passed already.
			if (! m_passed. insert(a_pair. first). second)
				return true;
			// If none of the conditions is met then approve copy.
			return false;
		}

	private:
		std::set<key_type>& m_passed;

	};

public:
	/**
	 *	List all trace variables that are available within the global context.
	 */
	template <typename OutputIterator_>
	uint_t list (OutputIterator_ a_out, const bool a_diff_only = false) const
	{
#if 0
		if (!this)
		{
			return 0;
		}
		else
#endif
		if (a_diff_only)
		{
			std::copy(m_variables. begin(), m_variables. end(), a_out);
			return static_cast<uint_t>(m_variables. size());
		}
		else
		{
			std::set<key_type> passed;
			for (const Context* c = this; c != NULL; c = c -> m_ancestor)
				std::remove_copy_if(c -> m_variables. begin(),
						c -> m_variables. end(), a_out, list_filter(passed));
			return static_cast<uint_t>(passed. size());
		}
	}

private:
	const Context* m_ancestor; /**< Ancestor context. */
	ContextOwner<M_>* m_owner; /**< Context's owner. */

	typedef boost::unordered_map<key_type, value_type> variables_t;
	variables_t m_variables; /**< Local trace variable map. */

#if defined(ANTA_NDL_STACKING)
private:
	static const uint_t FLAG_PUSH = 1;
	static const uint_t FLAG_POP = 2;

	typedef boost::unordered_map<key_type, int> marks_t;
	marks_t m_marks; /**< Marked trace variables. */

	/**
	 *	Mark the given variable.
	 */
	void mark (const key_type& a_key, const uint_t a_mark_flags)
	{
		typename marks_t::iterator found_at = m_marks. find(a_key);
		if (found_at == m_marks. end())
		{
			const std::pair<typename marks_t::iterator, bool> p =
				m_marks. insert(typename marks_t::value_type(a_key, 0));
			assert(p. second);
			found_at = p. first;
		}

		if ((found_at -> second & a_mark_flags) != 0)
			throw std::logic_error("the variable has been marked already");

		found_at -> second |= a_mark_flags;
	}

public:
	/**
	 *	Mark the given variable.
	 */
	bool push (const key_type& a_key)
	{
		mark(a_key, FLAG_PUSH);
		return true;
	}

	/**
	 *	Find the ancestor that has the given variable marked as pushed.
	 */
	const Context* find (const key_type& a_key) const
	{
		int level = 1;
		for (const Context* c = m_ancestor; c != NULL; c = c -> m_ancestor)
		{
			typename marks_t::const_iterator found_at = c -> m_marks. find(
					a_key);
			if (found_at != c -> m_marks. end())
			{
				if ((found_at -> second & FLAG_PUSH) != 0)
				{
					-- level;
				}

				if ((found_at -> second & FLAG_POP) != 0)
				{
					++ level;
				}

				if (level == 0)
				{
					return c;
				}
			}
		}

		return NULL;
	}

	/**
	 *	Restore the given variable to a previously marked state.
	 */
	bool pop (const key_type& a_key)
	{
		mark(a_key, FLAG_POP);

		const Context* ancestor = find(a_key);
		if (ancestor == NULL)
			throw std::logic_error("trying to pop a variable that has not been"
				" pushed");

		ref(a_key, true) = ancestor -> val(a_key);
		return true;
	}
#endif // ANTA_NDL_STACKING

#if defined(ANTA_NDL_MUTABLE_ANCESTOR) // @see nparse.script.functions.self
public:
	/**
	 *	Get the ancestor context.
	 */
	const Context* get_ancestor () const
	{
		return m_ancestor;
	}

	/**
	 *	Set the ancestor context.
	 *	NOTE:	Extremely unsafe when is not used property. Handle with care!
	 */
	void set_ancestor (const Context* a_ancestor)
	{
		m_ancestor = a_ancestor;
	}
#endif // ANTA_NDL_MUTABLE_ANCESTOR

};

/**
 *	The context owner (owns the instances of the Context class).
 */
template <typename M_>
class ContextOwner: public pool<M_>::type
{
public:
	/**
	 *	Create an instance of the Context class.
	 */
	Context<M_>* create (const Context<M_>* a_ancestor)
	{
		void* ptr = this -> allocate(sizeof(Context<M_>));
		m_instances. push_back(new(ptr) Context<M_>(a_ancestor, this));
		return m_instances. back();
	}

	/**
	 *	Destroy all the instances.
	 */
	void reset ()
	{
		if (! m_instances. empty())
		{
			for (typename instances_t::iterator i = m_instances. begin();
					i != m_instances. end(); ++ i)
			{
				(*i) -> ~Context<M_>();
			}
			m_instances. clear();
		}
	}

private:
	typedef std::vector<Context<M_>*> instances_t;
	instances_t m_instances;

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_CONTEXT_HPP_ */
