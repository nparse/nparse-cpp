/*
 * @file $/include/anta/ndl/prototypes.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.8

The MIT License (MIT)
Copyright (c) 2007-2017 Alex Kudinov <alex.s.kudinov@gmail.com>

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
#ifndef ANTA_NDL_PROTOTYPES_HPP_
#define ANTA_NDL_PROTOTYPES_HPP_

namespace anta {

/******************************************************************************/

/**
 *	A specialization of the template base class for extended models that injects
 *	additional functionality into the analysis state processor.
 *
 *	1. Allow Processor to act as an instance of trace context.
 *	2. Provide member functions for handling trace variables.
 *	3. Make extended Processor a creator/holder of trace contexts.
 *
 * 	NOTE: The third template parameter which equals to meta::false_ here implies
 *		  that this base class specialization is applied to extended models.
 */
template <typename M_>
class Base<Processor<typename ndl::extend<M_>::type>, M_, meta::false_>:
	public ndl::ContextOwner<M_>
{
public:
	/**
	 *	Required type definitions that allow Processor to act as an instance of
	 *	trace context.
	 *	@{ */
	typedef typename ndl::context_key<M_>::type key_type;
	typedef typename ndl::context_value<M_>::type value_type;
	/**	@} */

	/**
	 *	Get a mutable reference to the current state.
	 */
	State<M_>& get_state () const
	{
		return *m_state;
	}

	/**
	 *	Get a mutable reference to a trace variable in the local context.
	 */
	value_type& ref (const key_type& a_key, const bool a_reset = false)
	{
		return m_state -> ref(a_key, *this, a_reset);
	}

	/**
	 *	Get a constant reference to a trace variable in the global context.
	 */
	const value_type& val (const key_type& a_key) const
	{
		return m_state -> val(a_key);
	}

	/**
	 *	List all trace variables available within the global context.
	 */
	template <typename _OutputIterator>
	void list (_OutputIterator a_out, const bool a_this_only = false) const
	{
		m_state -> list(a_out, a_this_only);
	}

protected:
	State<M_>* m_state; /**< current state pointer */

};

/**
 *	A specialization of the template base class for extended models that injects
 *	additional functionality into acceptor network Node objects.
 *
 *	1. Make Node named.
 *	2. Allow semantic actions to be attached to Node.
 *	3. Make Node a semantic action executor.
 */
template <typename M_>
class Base<Node<typename ndl::extend<M_>::type>, M_>:
	public ndl::ActionExecutor<M_>
{
public:
	/**
	 *	The polymorphic destructor.
	 */
	virtual ~Base() {}

	/**
	 *	Set node name.
	 */
	void set_name (const typename ndl::node_name<M_>::type& a_name)
	{
		m_name = a_name;
	}

	/**
	 *	Get node name.
	 */
	const typename ndl::node_name<M_>::type& get_name () const
	{
		return m_name;
	}

	/**
	 *	An operator for attaching semantic actions either to the node itself or
	 *	to the last of its outgoing arcs.
	 */
	virtual Node<M_>& operator[] (
			const typename ndl::action<M_>::type& a_action) = 0;

private:
	typename ndl::node_name<M_>::type m_name;

};

/**
 *	A specialization of the template base class for extended models that injects
 *	additional functionality into acceptor network Arc objects.
 *
 *	1. Make Arc a semantic action executor.
 */
template <typename M_>
class Base<Arc<typename ndl::extend<M_>::type>, M_>:
	public ndl::ActionExecutor<M_>
{
public:

};

/**
 *	A specialization of the template base class for extended models that injects
 *	additional functionality into analysis state objects.
 *
 *	1. Make State a trace context provider.
 *
 *	NOTE: An extended State does not necessarily provide a trace context, and
 *		  even if it does, it may not own the context instance.
 */
template <typename M_>
class Base<State<typename ndl::extend<M_>::type>, M_>:
	public BaseStatic<State<typename ndl::extend<M_>::type> >
{
public:
	/**
	 *	The default constructor.
	 */
	Base ():
		m_bitpack (0)
	{
	}

	/**
	 *	Get a mutable reference to a trace variable in the local context.
	 */
	typename ndl::context_value<M_>::type& ref (
			const typename ndl::context_key<M_>::type& a_key,
			ndl::ContextOwner<M_>& a_owner, const bool a_reset = false)
	{
		return context(& a_owner) -> ref(a_key, a_reset);
	}

	/**
	 *	Get a constant reference to a trace variable in the global context.
	 */
	const typename ndl::context_value<M_>::type& val (
			const typename ndl::context_key<M_>::type& a_key) const
	{
		return context(NULL) -> val(a_key); // No Context instantiation.
	}

	/**
	 *	List all trace variables available within the global context.
	 */
	template <typename _OutputIterator>
	void list (_OutputIterator a_out, const bool a_this_only = false) const
	{
		if (a_this_only ? has_context() && is_own_context() : true)
		{
			context(NULL) -> list(a_out, a_this_only);
		}
	}

	/**
	 *	Check whether a trace variable is defined.
	 */
	template <typename Predicate_>
	bool is_defined (const typename ndl::context_key<M_>::type& a_key,
			const Predicate_& a_predicate) const
	{
		return context(NULL) -> is_defined(a_key, a_predicate);
	}

	/**
	 *	Check whether the state has a context.
	 */
	bool has_context () const
	{
		return 0 != (m_bitpack & 1UL);
	}

	/**
	 *	Check whether the state owns the context.
	 */
	bool is_own_context () const
	{
		return 0 != (m_bitpack & 2UL);
	}

	/**
	 *	Try to acquire the context somehow.
	 */
	ndl::Context<M_>* context (ndl::ContextOwner<M_>* a_owner) const
	{
		// NOTE: We are in a specialized template base class now, but we have to
		//		 use it as if it was a descendant, so we need a pointer of the
		//		 derived type.
		//		 It could also be achieved using the dynamic_cast operator.
		//		 However, since we do not need regular (dynamic) polymorphism
		//		 here, it might be somewhat cheaper to use static polymorphism.
		const State<M_>* p0 = this -> self();

		// Find the nearest ancestor that has a context.
		const State<M_>* p = p0;
		while (p != NULL && ! p -> has_context())
		{
			p = p -> get_ancestor();
		}

		if (p != p0)
		{
			// If there is such an ancestor then take its context.
			ndl::Context<M_>* context = (p != NULL) ? p -> get_context() : NULL;

			// Set the found context pointer for each of the just traversed
			// ancestors to avoid searching for it next time.
			const State<M_>* q = p0;
			while (q != p)
			{
				const_cast<State<M_>*>(q) -> set_context(context, false);
				q = q -> get_ancestor();
			}
		}

		// Make an own copy of the context if it is necessary.
		// NOTE: When a context owner is provided it means that a new context
		//		 instance is expected to be created if it does not exist.
		if (a_owner != NULL && ! is_own_context())
		{
			const_cast<Base*>(this)
				-> set_context(&* a_owner -> create(get_context()), true);
		}

		// Return the context pointer.
		return get_context();
	}

	/**
	 *	Replace current context with the given (substitute).
	 */
	void substitute (ndl::Context<M_>* a_ptr)
	{
		set_context(a_ptr, false);
	}

	/**
	 *	Get context pointer.
	 */
	ndl::Context<M_>* get_context () const
	{
		return reinterpret_cast<ndl::Context<M_>*>(m_bitpack & ~3UL);
	}

private:
	/**
	 *	Set context pointer.
	 */
	void set_context (ndl::Context<M_>* a_ptr, const bool a_own)
	{
		m_bitpack = reinterpret_cast<uint_t>(a_ptr) | (a_own ? 3UL : 1UL);
	}

	std::size_t m_bitpack; /**< context pointer stored with ownership flags */

};

/**
 *	A specialization of the analysis state entry point for extended models.
 */
template <typename M_>
struct entry_point<Processor<typename ndl::extend<M_>::type>, M_>
{
	static bool f (Processor<M_>& a_proc)
	{
		// Semantic actions are first executed for the arc, and then for the
		// corresponding target node.
		const Arc<M_>& arc = a_proc. get_state(). get_arc();
		return arc. execute(a_proc) && arc. get_target(). execute(a_proc);
	}

};

/**
 *	A specialization of the analysis state rollback mechanism for extended
 *	models.
 */
template <typename M_>
struct rollback_performer<Processor<typename ndl::extend<M_>::type>, M_>
{
	static void f (Processor<M_>& a_proc, const State<M_>* p)
	{
		while (p != NULL)
		{
			if (p -> is_own_context())
			{
				ndl::Context<M_>* ctx = p -> get_context();
				if (a_proc. evict(ctx, sizeof(*ctx)))
				{
					a_proc. destroy(ctx);
				}
			}
			if (! a_proc. evict(p, p -> size()))
			{
				break;
			}
			a_proc. get_observer(). notify(evEVICT, p);
			p = p -> get_ancestor();
		}
	}

};

/******************************************************************************/

} // namespace anta

#endif /* ANTA_NDL_PROTOTYPES_HPP_ */
