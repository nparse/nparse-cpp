/*
 * @file $/include/anta/sas/check.hpp
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
#ifndef ANTA_SAS_CHECK_HPP_
#define ANTA_SAS_CHECK_HPP_

namespace anta { namespace sas {

/******************************************************************************/

/*
 *	`model_check' is an auxiliary derivative model for a separate specialization
 *	of the spectrum type exclusively for the inner needs of the Check acceptor.
 *	See the partial specialization spectum<model_check<M_> > below.
 */
template <typename M_>
struct model_check: public M_ {};

template <typename M_, template <typename Mx_> class Acceptor_>
class Check: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		typename spectrum<model_check<M_> >::type S0(E, S);
		// The fake spectrum S0 receives accepted ranges from the wrapped
		// acceptor instead of the actual spectum S. In case if the accepted
		// range covers the entire parser element E then the range is being
		// passed to the actual spectrum S; see spectrum<..>::push(..) below.
		m_acceptor. accept(E, typename range<M_>::type(E. first, E. first), S0);
	}

	Check (const Acceptor_<model_check<M_> >& a_acceptor):
		m_acceptor (a_acceptor)
	{
	}

private:
	Acceptor_<model_check<M_> > m_acceptor;

};

} // namespace sas

/*
 *	Separate specialization of the spectrum type for the auxiliary derivative
 *	model model_check<M_>.
 */
template <typename M_>
struct spectrum<sas::model_check<M_> >
{
	class type
	{
	public:
		void push (const typename iterator<M_>::type& a_from,
				const typename iterator<M_>::type& a_to)
		{
			if ((a_from == m_element. first) && (a_to == m_element. second))
				m_spectrum. push(a_from, a_to);
		}

	public:
		type (const typename range<M_>::type& a_element,
				typename spectrum<M_>::type& a_spectrum):
			m_element (a_element), m_spectrum (a_spectrum)
		{
		}

	private:
		const typename range<M_>::type& m_element;
		typename spectrum<M_>::type& m_spectrum;

	};

};

/******************************************************************************/

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename M_, template <typename Mx_> class Acceptor_>
struct eval_terminal<M_, sas::Check<M_, Acceptor_> >: eval_acceptor<M_> {};

}}
#endif

} // namespace anta

#endif /* ANTA_SAS_CHECK_HPP_ */
