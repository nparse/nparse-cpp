/*
 * @file $/include/nparse/util/hashed_string.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.6

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
#ifndef NPARSE_UTIL_HASHED_STRING_HPP_
#define NPARSE_UTIL_HASHED_STRING_HPP_

#include <string>
#include <ostream>
#include <boost/functional/hash.hpp>
#include <encode/encode.hpp>

class hashed_string
{
public:
	typedef std::size_t result_type;

	hashed_string ():
		m_hash (0)
	{
	}

	template <typename First_>
	hashed_string (const First_& a_first):
		m_hash (0)
	{
		init(encode::string(a_first));
	}

	result_type hash () const
	{
		return m_hash;
	}

	operator const std::string& () const;

	bool empty () const
	{
		return (m_hash == 0);
	}

	typedef std::string::const_iterator const_iterator;

	const_iterator begin () const
	{
		return static_cast<const std::string&>(*this). begin();
	}

	const_iterator end () const
	{
		return static_cast<const std::string&>(*this). end();
	}

	void swap (hashed_string& a_hs)
	{
		std::swap(m_hash, a_hs. m_hash);
	}

private:
	result_type m_hash;

	void init (const std::string& a_str);

};

inline bool operator< (const hashed_string& u, const hashed_string& v)
{
	return u. hash() < v. hash();
}

inline bool operator> (const hashed_string& u, const hashed_string& v)
{
	return u. hash() > v. hash();
}

inline bool operator<= (const hashed_string& u, const hashed_string& v)
{
	return u. hash() <= v. hash();
}

inline bool operator>= (const hashed_string& u, const hashed_string& v)
{
	return u. hash() >= v. hash();
}

inline bool operator== (const hashed_string& u, const hashed_string& v)
{
	return u. hash() == v. hash();
}

inline bool operator!= (const hashed_string& u, const hashed_string& v)
{
	return u. hash() != v. hash();
}

inline std::ostream& operator<< (std::ostream& a_out, const hashed_string& a_hs)
{
	return a_out << static_cast<const std::string&>(a_hs);
}

namespace boost {

template<>
class hash<hashed_string>: public std::unary_function<hashed_string, std::size_t>
{
public:
	std::size_t operator() (const hashed_string& s) const
	{
		return s.hash();
	}

};

} // namespace boost

#endif /* NPARSE_UTIL_HASHED_STRING_HPP_ */
