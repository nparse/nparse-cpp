/*
 * @file $/include/anta/sas/skip.hpp
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
#ifndef ANTA_SAS_SKIP_HPP_
#define ANTA_SAS_SKIP_HPP_

#include <stdarg.h>

namespace anta { namespace sas {

/******************************************************************************/

template <typename M_>
class Skip: public Acceptor<M_>
{
public:
	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		const int tail = C. second - E. second;
		int p = 0;
		for (typename lengths_t::const_iterator i = m_lengths. begin();
				i != m_lengths. end(); ++ i)
		{
			const int q = *i;
			if (q >= 0)
			{
				if (q <= tail)
				{
					S. push(E. second, E. second + q);
				}
				p = q;
			}
			else
			while (p < -q)
			{
				++ p;
				if (p <= tail)
				{
					S. push(E. second, E. second + p);
				}
				else
					break;
			}
		}
	}

public:
	Skip& operator< (const int a_len)
	{
		m_lengths. push_back(a_len);
		return *this;
	}

	Skip ()
	{
	}

	Skip (const int N, ...)
	{
		va_list a;
		va_start(a, N);
		for (int n = 0; n < N; ++ n)
			*this < va_arg(a, int);
		va_end(a);
	}

private:
	typedef std::vector<int> lengths_t;
	lengths_t m_lengths;

};

/******************************************************************************/

}} // namespace anta::sas

#endif /* ANTA_SAS_SKIP_HPP_ */
