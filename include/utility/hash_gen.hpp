/*
 * @file $/include/utility/hash_gen.hpp
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
#ifndef UTILITY_HASH_GEN_HPP_
#define UTILITY_HASH_GEN_HPP_

namespace utility {

template <typename Hash_>
class hash_gen
{
	static Hash_ constM ()
	{
		return (static_cast<Hash_>(1) << ((sizeof(Hash_) << 3) - 1)) - 1;
	}

	static Hash_ constA ()
	{
		return 31415;
	}

	static Hash_ constB ()
	{
		return 27183;
	}

public:
	static Hash_ evolve (const Hash_& A)
	{
		return (A == 0) ? constA() : (A * constB()) % (constM() - 1);
	}

	template <typename Value_>
	static Hash_ iterate (const Value_& v, const Hash_& H, const Hash_& A)
	{
		return (A * H + static_cast<Hash_>(v)) * constM();
	}

	template <typename Iterator_>
	static Hash_ f (const Iterator_& i0, const Iterator_& iN)
	{
		Hash_ A = 0, H = 0;
		for (Iterator_ i = i0; i != iN; ++ i)
		{
			A = evolve(A);
			H = iterate(*i, H, A);
		}
		return H;
	}

public:
	hash_gen (const Hash_ a_H = 0, const Hash_ a_A = 0):
		m_H (a_H), m_A (a_A)
	{
	}

	template <typename Value_>
	const Hash_& iterate (const Value_& a_v)
	{
		m_A = evolve(m_A);
		return m_H = iterate(a_v, m_H, m_A);
	}

	template <typename Iterator_>
	const Hash_& iterate (const Iterator_& i0, const Iterator_& iN)
	{
		for (Iterator_ i = i0; i != iN; ++ i)
			iterate(*i);
		return m_H;
	}

	operator const Hash_& () const
	{
		return m_H;
	}

private:
	Hash_ m_H, m_A;

};

} // namespace utility

#endif /* UTILITY_HASH_HPP_ */
