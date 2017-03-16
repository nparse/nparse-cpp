/*
 * @file $/include/anta/sas/prefix.hpp
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
#ifndef ANTA_SAS_PREFIX_HPP_
#define ANTA_SAS_PREFIX_HPP_

#include <utility/hash_gen.hpp>

namespace anta { namespace sas {

/******************************************************************************/

template <typename M_, typename Hash_ = uint_t>
class Prefix: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		typename iterator<M_>::type i = E. second;
		const typename iterator<M_>::type i_max =
			i + std::min<typename string_type::size_type>(C. second - E. second,
					m_max_length);

		bool found = false;
		typename iterator<M_>::type found_i = i;

		utility::hash_gen<Hash_> h;
		typename string_type::size_type length = 0;

		while (i != i_max)
		{
			h. iterate(*i);
			++ i;

			if (++ length < m_min_length)
			{
				continue;
			}

			typename patterns_t::const_iterator found_at = m_patterns. find(h);
			if (found_at != m_patterns. end())
			{
				if (found_at -> second. size() != length)
				{
					continue;
				}

				if (! std::equal(found_at -> second. begin(),
							found_at -> second. end(), E. second))
				{
					continue;
				}

				// S. push(E. second, i);
				found = true;
				found_i = i;
			}
		}

		if (found)
			S. push(E. second, found_i);
	}

public:
	typedef typename string<M_>::type string_type;

	Prefix& operator< (const string_type& a_pattern)
	{
		if (a_pattern. empty())
			throw std::logic_error("empty patterns are not allowed");
		const Hash_ h = utility::hash_gen<Hash_>::f(
				a_pattern. begin(), a_pattern. end());
		typename patterns_t::iterator found_at = m_patterns. find(h);
		if (found_at != m_patterns. end())
		{
			if (found_at -> second != a_pattern)
				throw std::logic_error("hash collision detected");
		}
		else
		{
			const std::pair<typename patterns_t::iterator, bool> p =
				m_patterns. insert(
					typename patterns_t::value_type(h, a_pattern));
			assert(p. second);
			const typename string_type::size_type length = a_pattern. size();
			if (m_max_length < length)
				m_max_length = length;
			if ((m_min_length > length) || (m_min_length == 0))
				m_min_length = length;
		}
		return *this;
	}

	Prefix ():
		m_max_length (0), m_min_length (0)
	{
	}

private:
	typedef std::map<Hash_, string_type> patterns_t;
	patterns_t m_patterns;
	typename string_type::size_type m_max_length;
	typename string_type::size_type m_min_length;

};

} // namespace sas

/******************************************************************************/

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename M_, typename H_>
struct eval_terminal<M_, sas::Prefix<M_, H_> >: eval_acceptor<M_> {};

}}
#endif

} // namespace anta

#endif /* ANTA_SAS_PREFIX_HPP_ */
