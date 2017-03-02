/*
 * @file $/include/anta/_aux/lister.hpp
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
#ifndef ANTA_AUX_LISTER_HPP_
#define ANTA_AUX_LISTER_HPP_

#include <iterator>
#include <ostream>
#include <boost/config.hpp>

namespace anta { namespace aux {

// (forward declaration)
template <typename M_> class Variable;

/**
 *	An internal iterator that produces string representations of associative
 *	arrays.
 */
template <typename M_>
class lister:
	public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	template <typename V_, typename Void_ = void>
	struct as_string_functor
	{
		static std::string f (const V_& a_v)
		{
			return boost::lexical_cast<std::string>(a_v);
		}

	};

	template <typename M2_>
	struct as_string_functor<Variable<M2_>, void>
	{
		static std::string f (const Variable<M_>& a_v)
		{
			return encode::string(a_v. as_string());
		}

	};

	template <typename V_>
	static std::string as_string (const V_& a_v)
	{
		return as_string_functor<V_>::f(a_v);
	}

	std::ostream& m_out;
	bool& m_first;

public:
	lister (std::ostream& a_out, bool& a_first):
		m_out (a_out), m_first (a_first)
	{
	}

	lister& operator= (const typename ndl::context_entry<M_>::type& a_pair)
	{
		// @todo: implement field normalization
		if (! a_pair. first. empty())
		{
			if (m_first) m_first = false; else m_out << ", ";
			m_out << a_pair. first << ": " << as_string(a_pair. second);
		}
		return *this;
	}

	lister& operator* () { return *this; }
	lister& operator++ () { return *this; }
	lister& operator++ (int) { return *this; }

#if defined(BOOST_MSVC)
	const lister& operator= (const lister& a_inst)
	{
		if (this != &a_inst)
		{
			this -> ~lister();
			new(this) lister(a_inst. m_out, a_inst. m_first);
		}
		return *this;
	}
#endif
};

/**
 *	An auxiliary template function that returns a string representation of a set
 *	of context variables.
 */
template <typename Context_>
std::string list (const Context_& a_context, const bool a_diff_only)
{
	std::stringstream out;
	bool first = true;
	out << '{';
	a_context. list(
		lister<typename model_of<Context_>::type>(out, first), a_diff_only
	);
	out << '}';
	return out. str();
}

}} // namespace anta::aux

#endif /* ANTA_AUX_LISTER_HPP_ */
