/*
 * @file $/include/anta/sas/pattern.hpp
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
#ifndef ANTA_SAS_PATTERN_HPP_
#define ANTA_SAS_PATTERN_HPP_

#include <utility/pattern.hpp>

namespace anta { namespace sas {

/******************************************************************************/

template <typename M_, typename Compare_ =
	std::equal_to<typename character<M_>::type> >
class Pattern: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		typename iterator<M_>::type s = E. second;
		typename string_type::const_iterator p = m_pattern. begin();
		if (utility::pattern<typename character<M_>::type>::match(s, C. second,
					p, m_pattern. end(), m_compare, m_classifier)
				&& (p == m_pattern. end()))
		{
			S. push(E. second, s);
		}
	}

public:
	typedef typename string<M_>::type string_type;
	typedef std::map<typename character<M_>::type, string_type> classes_type;

	Pattern (const string_type& a_pattern, const classes_type& a_classes):
		m_pattern (a_pattern), m_classifier (a_classes)
	{
	}

private:
	Compare_ m_compare;
	string_type m_pattern;

	class classifier_t
	{
		const classes_type& m_classes;

	public:
		classifier_t (const classes_type& a_classes):
			m_classes (a_classes)
		{
		}

		bool operator() (const typename character<M_>::type& a_character,
				const typename character<M_>::type& a_class) const
		{
			// @todo: use custom hashmap (class, character) -> 0|1
			const typename classes_type::const_iterator class_at =
				m_classes. find(a_class);
			return (class_at != m_classes. end())
				&& std::binary_search(class_at -> second. begin(),
						class_at -> second. end(), a_character);
		}

	};

	classifier_t m_classifier;

};

} // namespace sas

/******************************************************************************/

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename M_, typename C_>
struct eval_terminal<M_, sas::Pattern<M_, C_> >: eval_acceptor<M_> {};

} // namespace detail

namespace terminals {

}} // namespace ndl::terminals
#endif

} // namespace anta

#endif /* ANTA_SAS_PATTERN_HPP_ */
