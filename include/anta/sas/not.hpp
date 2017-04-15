/*
 * @file $/include/anta/sas/not.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef ANTA_SAS_NOT_HPP_
#define ANTA_SAS_NOT_HPP_

namespace anta { namespace sas {

/******************************************************************************/

/*
 *	`model_not' is an auxiliary derivative model for a separate specialization
 *	of the spectrum type exclusively for the inner needs of the Not acceptor.
 *	See the partial specialization spectum<model_not<M_> > below.
 */
template <typename M_>
struct model_not: public M_ {};

template <typename M_, template <typename Mx_> class Acceptor_>
class Not: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		try
		{
			typename spectrum<model_not<M_> >::type S0;
			// If the fake spectrum (S0) receives an accepted range from the
			// wrapped acceptor (m_acceptor) it throws an exception of type
			// model_not<M_>, and therefore the actual spectum (S) does not
			// receive any range.
			m_acceptor. accept(C, E, S0);
			// Otherwise, if no exception was thrown then the actual spectum
			// receives an empty range corresponding to the end of the current
			// element of the parser context.
			S. push(E. second, E. second);
		}
		catch (const model_not<M_>&)
		{
		}
	}

	Not (const Acceptor_<model_not<M_> >& a_acceptor):
		m_acceptor (a_acceptor)
	{
	}

private:
	Acceptor_<model_not<M_> > m_acceptor;

};

} // namespace sas

/*
 *	Separate specialization of the spectrum type for the auxiliary derivative
 *	model model_not<M_>.
 */
template <typename M_>
struct spectrum<sas::model_not<M_> >
{
	class type
	{
	public:
		void push (const typename iterator<M_>::type&,
				const typename iterator<M_>::type&)
		{
			throw sas::model_not<M_>();
		}

	};

};

/******************************************************************************/

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

template <typename M_, template <typename Mx_> class Acceptor_>
struct eval_terminal<M_, sas::Not<M_, Acceptor_> >: eval_acceptor<M_> {};

}}
#endif

} // namespace anta

#endif /* ANTA_SAS_NOT_HPP_ */
