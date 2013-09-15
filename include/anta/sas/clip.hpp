/*
 * @file $/include/anta/sas/clip.hpp
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
#ifndef ANTA_SAS_CLIP_HPP_
#define ANTA_SAS_CLIP_HPP_

#include <stdarg.h>

namespace anta { namespace sas {

/******************************************************************************/

template <typename M_>
class Clip: public Acceptor<M_>, public std::vector<int>
{
	typedef typename std::vector<int>::const_iterator const_iterator;

public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		const int tail = E. second - E. first;
		int p = 0;
		for (const_iterator i = this -> begin(); i != this -> end(); ++ i)
		{
			const int q = *i;
			if (q >= 0)
			{
				if (q <= tail)
				{
					if (m_head)
						S. push(E. first, E. first + q);
					else
						S. push(E. second - q, E. second);
				}
				p = q;
			}
			else
			while (p < -q)
			{
				++ p;
				if (p <= tail)
				{
					if (m_head)
						S. push(E. first, E. first + p);
					else
						S. push(E. second - p, E. second);
				}
				else
					break;
			}
		}
	}

	Clip& operator< (const int a_len)
	{
		this -> push_back(a_len);
		return *this;
	}

	Clip ():
		m_head (true)
	{
	}

	Clip (const int N, ...):
		m_head (true)
	{
		va_list arg;
		va_start(arg, N);
		for (int n = 0; n < N; ++ n)
			*this < va_arg(arg, int);
		va_end(arg);
	}

private:
	bool m_head;

};

} // namespace sas

/******************************************************************************/

/**
 *
 *	@{ */

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace detail {

struct clip_config: public std::vector<int> {};

template <typename M_>
struct eval_terminal<M_, clip_config>
{
	typedef JointJump<M_> result_type;

	template <typename Expr_>
	result_type operator() (const Expr_& a_expr, Rule<M_>& a_rule) const
	{
		sas::Clip<M_> instance;
		const clip_config& cfg = proto::value(a_expr);
		instance. swap(const_cast<clip_config&>(cfg));
		return a_rule(instance);
	}

};

template <typename M_>
struct eval_terminal<M_, sas::Clip<M_> >: eval_acceptor<M_> {};

} // namespace detail

namespace terminals {

using namespace detail;

proto::terminal<clip_config>::type clip (const int N, ...)
{
	clip_config cfg;
	va_list arg;
	va_start(arg, N);
	for (int n = 0; n < N; ++ n)
		cfg. push_back(va_arg(arg, int));
	va_end(arg);
	const proto::terminal<clip_config>::type res = {cfg};
	return res;
}

}} // namespace ndl::terminals
#endif

/**	@} */

} // namespace anta

#endif /* ANTA_SAS_CLIP_HPP_ */
