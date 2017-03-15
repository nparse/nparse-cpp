/*
 * @file $/include/nparse/specs/nlg/pool.hpp
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
#ifndef NPARSE_SPECS_NLG_POOL_HPP_
#define NPARSE_SPECS_NLG_POOL_HPP_

#include <utility/file_pool.hpp>

namespace anta {

template <>
struct pool<nparse::NLG>
{
	class type
	{
		bool m_use_ramd;
		utility::memory_pool m_ramd;
		utility::file_pool m_file;

	public:
		/**
		 *	The only constructor.
		 */
		type ():
			m_use_ramd (true)
		{
		}

		/**
		 *	Specify pool file.
		 */
		void set_swap_file (const std::string& a_path)
		{
			m_use_ramd = false;
			m_file. set_swap_file(a_path);
		}

		/**
		 *	Get the capacity of the pool.
		 */
		std::size_t get_capacity () const
		{
			return m_use_ramd
				? m_ramd. get_capacity()
				: m_file. get_capacity();
		}

		/**
		 *	Set the capacity of the pool.
		 */
		void set_capacity (const std::size_t a_size)
		{
			return m_use_ramd
				? m_ramd. set_capacity(a_size)
				: m_file. set_capacity(a_size);
		}

		/**
		 *	Get the used size of the pool.
		 */
		std::size_t get_usage () const
		{
			return m_use_ramd
				? m_ramd. get_usage()
				: m_file. get_usage();
		}

		/**
		 *	Allocate memory from the pool.
		 */
		void* allocate (const std::size_t a_size)
		{
			return m_use_ramd
				? m_ramd. allocate(a_size)
				: m_file. allocate(a_size);
		}

		/**
		 *	Clear the pool.
		 */
		void clear ()
		{
			return m_use_ramd
				? m_ramd. clear()
				: m_file. clear();
		}

	};

};

} // namespace anta

inline void* operator new (const std::size_t a_size,
		anta::pool<nparse::NLG>::type& a_pool)
{
	return a_pool. allocate(a_size);
}

inline void operator delete(void *, anta::pool<nparse::NLG>::type&)
{
}

#endif /* NPARSE_SPECS_NLG_POOL_HPP_ */
