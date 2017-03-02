/*
 * @file $/include/anta/ndl/prototypes.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@nparse.com>
 
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
 *	Injecting additional functionality to the Traveller class:
 *	1. Making it an owner of instances of the Context class.
 *	2. Provide additional member functions for the trace variable handling.
 *
 * 	NOTE: The third template parameter which equals to meta::false_ means that
 *		  this base class specialization is applied to extended models only.
 */
template <typename M_>
class Base<Traveller<typename ndl::extend<M_>::type>, M_, meta::false_>:
	public ndl::ContextOwner<M_>
{
public:
	/**
	 *	Necessary type definitions for meeting the requirements of the Context
	 *	concept.
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
	 *	Get a constant reference to a trace variable from the global context.
	 */
	const value_type& val (const key_type& a_key) const
	{
		return m_state -> val(a_key);
	}

	/**
	 *	List all the trace variables that are available within the global
	 *	context.
	 */
	template <typename _OutputIterator>
	void list (_OutputIterator a_out, const bool a_this_only = false) const
	{
		m_state -> list(a_out, a_this_only);
	}

protected:
	State<M_>* m_state;

};

/**
 *	Injecting additional functionality to the Node class:
 *	1. Making the Node a semantic actions performer.
 *	2. Making the Node to be able to attach the semantic actions.
 *	3. Making the Node named.
 */
template <typename M_>
class Base<Node<typename ndl::extend<M_>::type>, M_>:
	public ndl::ActionPerformer<M_>
{
public:
	/**
	 *	The polymorphic destructor.
	 */
	virtual ~Base() {}

	/**
	 *	The operator for attaching custom semantic actions to the node itself or
	 *	to the last of its arcs.
	 */
	virtual Node<M_>& operator[] (
			const typename ndl::action<M_>::type& a_action) = 0;

	/**
	 *	Set the name of the node.
	 */
	void set_name (const typename ndl::node_name<M_>::type& a_name)
	{
		m_name = a_name;
	}

	/**
	 *	Get the name of the node.
	 */
	const typename ndl::node_name<M_>::type& get_name () const
	{
		return m_name;
	}

private:
	typename ndl::node_name<M_>::type m_name;

};

/**
 *	Injecting additional functionality to the Arc class:
 *	1. Making the Arc a semantic actions performer.
 */
template <typename M_>
class Base<Arc<typename ndl::extend<M_>::type>, M_>:
	public ndl::ActionPerformer<M_>
{
public:

};

/**
 *	Injecting additional functionality to the State class:
 *	1. Making the State a trace context provider. Note that neither a state owns
 *	   the corresponding context nor there is a separate context for each state.
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
	 *	Get a mutable reference to a variable in the local context.
	 */
	typename ndl::context_value<M_>::type& ref (
			const typename ndl::context_key<M_>::type& a_key,
			ndl::ContextOwner<M_>& a_owner, const bool a_reset = false)
	{
		return context(& a_owner) -> ref(a_key, a_reset);
	}

	/**
	 *	Get a constant reference to a variable from the global context.
	 */
	const typename ndl::context_value<M_>::type& val (
			const typename ndl::context_key<M_>::type& a_key) const
	{
		return context(NULL) -> val(a_key); // No Context instantiation.
	}

	/**
	 *	Check whether a variable is defined.
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
		return 0 != (m_bitpack & 1L);
	}

	/**
	 *	Check whether the state owns the context.
	 */
	bool is_own_context () const
	{
		return 0 != (m_bitpack & 2L);
	}

	/**
	 *	List all the trace variables that are available within the global
	 *	context.
	 */
	template <typename _OutputIterator>
	void list (_OutputIterator a_out, const bool a_this_only = false) const
	{
		if (a_this_only ? has_context() && is_own_context() : true)
			context(NULL) -> list(a_out, a_this_only);
	}

	/**
	 *	Try to acquire the context somehow.
	 */
	ndl::Context<M_>* context (ndl::ContextOwner<M_>* a_owner) const
	{
		// NOTE: We're in a specialized template base class now but we have to
		//		 use it as if it was a descendant, so we need a pointer of the
		//		 derived type.
		//		 It could also be implemented using the dynamic_cast operator.
		//		 However, since we don't need dynamic polymorphism here it might
		//		 be somewhat cheaper to use so-called static polymorphism.
		const State<M_>* p0 = this -> self();

		// Find the nearest ancestor that has a context.
		const State<M_>* p = p0;
		while (p != NULL && ! p -> has_context())
			p = p -> get_ancestor();

		if (p != p0)
		{
			// If there is such an ancestor then take its context.
			ndl::Context<M_>* context = (p != NULL) ? p -> get_ptr() : NULL;

			// Set the found context pointer for each of the just traversed
			// ancestors to avoid searching for it next time.
			const State<M_>* q = p0;
			while (q != p)
			{
				const_cast<State<M_>*>(q) -> set_ptr(context, false);
				q = q -> get_ancestor();
			}
		}

		// Make an own copy of the context if it is necessary. The fact that a
		// context owner was provided means that a new context instance is
		// supposed to be created.
		if (a_owner != NULL && ! is_own_context())
		{
			const_cast<Base*>(this)
				-> set_ptr(&* a_owner -> create(get_ptr()), true);
		}

		// Return the context pointer.
		return get_ptr();
	}

	/**
	 *	Substitute given context for the current one.
	 */
	void substitute (ndl::Context<M_>* a_ptr)
	{
		set_ptr(a_ptr, false);
	}

private:
	/**
	 *	Set the context pointer.
	 */
	void set_ptr (ndl::Context<M_>* a_ptr, const bool a_own)
	{
		m_bitpack = reinterpret_cast<uint_t>(a_ptr) | (a_own ? 3L : 1L);
	}

	/**
	 *	Get the context pointer.
	 */
	ndl::Context<M_>* get_ptr () const
	{
		return reinterpret_cast<ndl::Context<M_>*>(m_bitpack & ~3L);
	}

	uint_t m_bitpack; /**< Context pointer and ownership flags. */

};

/*
 *	Specializing the semantic action provider for the model.
 */
template <typename M_>
struct entry_functor<Traveller<typename ndl::extend<M_>::type>, M_>
{
	static bool f (Traveller<M_>& a_trav)
	{
		// The semantic actions are executed first for the arc, and then for the
		// corresponding target node.
		const Arc<M_>& arc = a_trav. get_state(). get_arc();
		return arc. entry(a_trav) && arc. get_target(). entry(a_trav);
	}

};

/******************************************************************************/

} // namespace anta

#endif /* ANTA_NDL_PROTOTYPES_HPP_ */
