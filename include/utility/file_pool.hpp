/*
 * @file $/include/utility/file_pool.hpp
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
#ifndef UTILITY_FILE_POOL_HPP_
#define UTILITY_FILE_POOL_HPP_

#include <stdexcept>
#include <boost/iostreams/device/mapped_file.hpp>

namespace utility {

using namespace boost::iostreams;

/**
 *	Preallocated memory byte pool.
 */
class file_pool
{
	mapped_file_params m_params;
	mapped_file m_file;
	std::size_t m_used;

public:
	/**
	 *	The only constructor.
	 */
	file_pool ():
		m_used (0)
	{
	}

	/**
	 *	Set the location of the swap file.
	 */
	void set_swap_file (const std::string& a_path)
	{
		if (m_file. is_open())
		{
			throw std::logic_error("unable to change the location of a pool"
					" file location while the pool is in use");
		}
		m_params. path = a_path;
		m_params. flags = mapped_file::readwrite;
	}

	/**
	 *	Get the capacity of the pool.
	 */
	std::size_t get_capacity () const
	{
		return static_cast<std::size_t>(m_file. size());
	}

	/**
	 *	Set the capacity of the pool.
	 */
	void set_capacity (const std::size_t a_size)
	{
		if (a_size > 0)
		{
			if (m_file. is_open())
			{
				throw std::logic_error("unable to change the capacity of a pool"
						" that is already in use");
			}
			m_params. new_file_size = a_size;
			m_file. open(m_params);
			if (! m_file. is_open())
			{
				throw std::runtime_error("unable to open pool file");
			}
		}
		else
		{
			if (m_file. is_open())
				m_file. close();
			m_used = 0;
		}
	}

	/**
	 *	Get the used size of the pool.
	 */
	std::size_t get_usage () const
	{
		return m_used;
	}

	/**
	 *	Allocate memory from the pool.
	 */
	void* allocate (const std::size_t a_size)
	{
		if (m_used + a_size > get_capacity())
		{
			throw std::bad_alloc();
		}
		return m_file. data() + ((m_used += a_size) - a_size);
	}

	/**
	 *	Clear the pool.
	 */
	void clear ()
	{
		m_used = 0;
	}

};

} // namespace utility

/**
 *	Instantiation operator that allocates memory from the byte pool.
 */
inline void* operator new (const std::size_t a_size, utility::file_pool& a_pool)
{
	return a_pool. allocate(a_size);
}

/**
 *	Disposal operator that returns previously allocated memory to the byte pool.
 */
inline void operator delete (void *, utility::file_pool&)
{
}

#endif /* UTILITY_FILE_POOL_HPP_ */
