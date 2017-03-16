/*
 * @file $/include/anta/_aux/debug_print.hpp
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
#ifndef ANTA_AUX_DEBUG_PRINT_HPP_
#define ANTA_AUX_DEBUG_PRINT_HPP_

#include <map>
#include <ostream>
#include <iomanip>
#include <encode/encode.hpp>

#if defined(ANTA_AUX_VARIABLE_HPP_)
template <typename M_>
std::ostream& operator<< (std::ostream& a_out,
		const anta::aux::Variable<M_>& a_var)
{
	a_out << encode::string(a_var. as_string());
	return a_out;
}
#endif // defined(ANTA_AUX_VARIABLE_HPP_)

namespace {

/**
 *	An auxiliary container for unnamed object tags.
 */
typedef std::map<const void*, int> object_tags_t;

/**
 *	An auxiliary function that returns numbered tags for arbitrary objects.
 */
template <typename Object_>
std::string ntag (const Object_& a_object, object_tags_t& a_tags,
		const std::string& a_prefix = "")
{
	std::stringstream tmp;
	const void* ptr = static_cast<const void*>(&a_object);
#if 0
	tmp	<< std::setbase(16) << std::setfill('0')
		<< a_prefix << std::setw(4) << (reinterpret_cast<long>(ptr) & 0xFFFF);
#else
	object_tags_t::iterator found_at = a_tags. find(ptr);
	if (found_at == a_tags. end())
	{
		const std::pair<object_tags_t::iterator, bool> p = a_tags. insert(
				object_tags_t::value_type(ptr, a_tags. size() + 1));
		assert(p. second);
		found_at = p. first;
	}
	tmp	<< std::setbase(16) << std::setfill('0')
		<< a_prefix << std::setw(4) << found_at -> second;
#endif
	return tmp. str();
}

/**
 *	An auxiliary functor that is used to obtain string tags for node objects.
 */
template <
	typename M_,
	typename IsBasic_ = typename anta::meta::is_basic<M_>::type
>
struct node_tag_functor;

/**
 *	A specialization of the node tag functor for basic models.
 */
template <typename M_>
struct node_tag_functor<M_, anta::meta::true_>
{
	static std::string f (const anta::Node<M_>& a_node, object_tags_t& a_tags,
			const std::string& a_prefix)
	{
		return ntag(a_node, a_tags, a_prefix + "node");
	}

};

/**
 *	A specialization of the node tag functor for extended models.
 */
#if defined(ANTA_NDL_MODEL_HPP_)
// specialization for extended models
template <typename M_>
struct node_tag_functor<M_, anta::meta::false_>
{
	static std::string f (const anta::Node<M_>& a_node, object_tags_t& a_tags,
			const std::string& a_prefix)
	{
		const typename anta::ndl::node_name<M_>::type& name(a_node. get_name());
		if (name. empty())
		{
			return std::string(a_prefix) + node_tag_functor<
						M_, anta::meta::true_>::f(a_node, a_tags, a_prefix);
		}
		else
		{
			return std::string(a_prefix) + encode::string(name);
		}
	}

};
#endif // defined(ANTA_NDL_MODEL_HPP_)

/**
 *	An auxiliary function that returns the tag of the given node.
 */
template <typename M_>
std::string node_tag (const anta::Node<M_>& a_node, object_tags_t& a_tags,
		const std::string& a_prefix = "")
{
	return node_tag_functor<M_>::f(a_node, a_tags, a_prefix);
}

} // namespace

/**
 *	The debug print iterator that is used for trace variable listing.
 */
#if defined(ANTA_NDL_MODEL_HPP_)
template <typename M_>
class DebugPrintIterator: public std::iterator<std::output_iterator_tag, void,
	void, void, void>
{
	std::ostream* m_out;
	char m_ch;

public:
	DebugPrintIterator (std::ostream* a_out, const char a_ch):
		m_out (a_out), m_ch (a_ch)
	{
	}

	DebugPrintIterator& operator= (
			const typename anta::ndl::context_entry<M_>::type& a_pair)
	{
		*m_out
			<< m_ch << '\t' << a_pair. first << '\t' << a_pair. second << '\n';
		return *this;
	}

	DebugPrintIterator& operator* () { return *this; }
	DebugPrintIterator& operator++ () { return *this; }
	DebugPrintIterator& operator++ (int) { return *this; }

};
#endif // defined(ANTA_NDL_MODEL_HPP_)

/**
 *	The debug print observer that is used for logging analysis state traveller's
 *	activity to a text stream.
 */
template <typename M_>
class DebugPrintObserver
{
	std::ostream* m_out;
	object_tags_t m_nodes, m_states;

public:
	/**
	 *	The only constructor.
	 */
	DebugPrintObserver ():
		m_out (NULL)
	{
	}

	/**
	 *	Link the observer to an output stream.
	 */
	void link (std::ostream* a_out)
	{
		m_out = a_out;
	}

public:
	/**
	 *	Implementations of functions required by the observer concept.
	 *	@{ */

	void notify (const anta::observer_event_t a_event,
			const anta::State<M_>* a_state)
	{
		if (m_out == NULL)
			return;

		*m_out << a_state << ' ';

		switch (a_event)
		{
		case anta::evPUSH:	*m_out << "push "; break;
		case anta::evPULL:	*m_out << "pull "; break;
		case anta::evDENY:	*m_out << "deny "; break;
		case anta::evENTRY:	*m_out << "entry"; break;
		case anta::evTRACE:	*m_out << "trace"; break;
		case anta::evBLOCK:	*m_out << "block"; break;
		case anta::evSPLIT:	*m_out << "split"; break;
		case anta::evDEFER: *m_out << "defer"; break;
		default:
			return;
		}

		const anta::State<M_>* ancestor = a_state -> get_ancestor();
		const std::string tagA = ntag(*ancestor, m_states, "A:");
		const std::string tagC = ntag(*a_state, m_states, "C:");

		*m_out << '\t' << tagC << '\t' << tagA << '\t' << (
			(ancestor == NULL)
			? "n/a"
			: node_tag(ancestor -> get_arc(). get_target(), m_nodes). c_str()
		);

		// Print jump type.
		switch (a_state -> get_arc(). get_type())
		{
		case anta::atSimple:	*m_out << " -> "; break;
		case anta::atInvoke:	*m_out << " invokes "; break;
		case anta::atExtend:	*m_out << " extends "; break;
		case anta::atPositive:	*m_out << " asserts "; break;
		case anta::atNegative:	*m_out << " negates "; break;
		}

		// Print the target node tag.
		*m_out << node_tag(a_state -> get_arc(). get_target(), m_nodes) << '\n';

#if defined(ANTA_NDL_MODEL_HPP_)
		// List trace variables.
		if	(	(a_state != NULL)
			&&	(	(a_event == anta::evPULL)
				||	(	(a_event == anta::evENTRY)
					&&	(a_state -> is_own_context())
					)
				)
			)
		{
			a_state -> list(
				DebugPrintIterator<M_>(
					m_out, (a_event == anta::evENTRY) ? '+' : '~'),
				a_event == anta::evENTRY
			);
		}
#endif // defined(ANTA_NDL_MODEL_HPP_)
	}

	void status (
			const anta::uint_t a_iteration_count,
			const anta::uint_t a_pool_usage,
			const anta::uint_t a_queue_length,
			const anta::uint_t a_trace_count,
			const anta::uint_t a_deferred_count)
	{
	}

	void reset ()
	{
		m_nodes. clear();
		m_states. clear();
	}

	/** @} */
};

/**
 *	A macro for installing the debug observer for arbitrary models.
 */
#define ANTA_AUX_DEBUG_PRINT_OBSERVER(MODEL) \
	namespace anta { template <> struct observer<MODEL> { \
		typedef DebugPrintObserver<MODEL> type; \
	}; }

#endif /* ANTA_AUX_DEBUG_PRINT_HPP_ */
