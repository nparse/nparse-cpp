/*
 * @file $/include/util/memory_pool.hpp
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
#ifndef UTIL_MEMORY_POOL_HPP_
#define UTIL_MEMORY_POOL_HPP_

#include <stdexcept>

namespace utility {

/**
 *	Preallocated memory byte pool.
 */
class memory_pool
{
public:
	/**
	 *	The only constructor.
	 */
	memory_pool ():
		m_bytes (NULL), m_capacity (0), m_used (0), m_peak (0), m_evicted (0)
	{
	}

	/**
	 *	The destructor.
	 */
	~memory_pool ()
	{
		if (m_bytes != NULL)
		{
			delete[] m_bytes;
			m_bytes = NULL;
		}
		m_capacity = 0;
		m_used = 0;
		m_peak = 0;
		m_evicted = 0;
	}

	/**
	 *	Get total pool capacity.
	 */
	std::size_t get_capacity () const
	{
		return m_capacity;
	}

	/**
	 *	Set total pool capacity.
	 */
	void set_capacity (const std::size_t a_size)
	{
		if (m_used > 0)
		{
			throw std::logic_error("unable to change the capacity of a pool"
					" that is already in use");
		}
		if (m_bytes != NULL)
		{
			delete[] m_bytes;
			m_bytes = NULL;
		}
		if ((m_capacity = a_size) > 0)
		{
			m_bytes = new char[m_capacity];
		}
	}

	/**
	 *	Get current pool usage.
	 */
	std::size_t get_usage () const
	{
		return m_used;
	}

	/**
	 *	Get maximum (peak) pool usage so far.
	 */
	std::size_t get_peak_usage () const
	{
		return m_peak;
	}

	/**
	 *	Get total evicted size.
	 */
	std::size_t get_evicted_size () const
	{
		return m_evicted;
	}

	/**
	 *	Allocate a segment of specified size from the pool.
	 */
	void* allocate (const std::size_t a_size)
	{
		if (m_used + a_size > m_capacity)
		{
			throw std::bad_alloc();
		}
		return m_bytes + (track_peak(m_used += a_size) - a_size);
	}

	/**
	 *	Try to evict the last allocated segment.
	 */
	bool evict (const void* a_ptr, const std::size_t a_size)
	{
		const char* probe = reinterpret_cast<const char*>(a_ptr);
		if (m_bytes + m_used - a_size == probe)
		{
			m_used -= a_size;
			m_evicted += a_size;
			return true;
		}
		return false;
	}

	/**
	 *	Clear the pool.
	 */
	void clear ()
	{
		m_used = 0;
		m_peak = 0;
		m_evicted = 0;
	}

private:
	char* m_bytes;
	std::size_t m_capacity;
	std::size_t m_used;
	std::size_t m_peak;
	std::size_t m_evicted;

	std::size_t track_peak (const std::size_t a_size)
	{
		m_peak = std::max(m_peak, a_size);
		return a_size;
	}

};

} // namespace utility

/**
 *	Instantiation operator that allocates memory from the byte pool.
 */
inline void* operator new (const std::size_t a_size, utility::memory_pool& a_pool)
{
	return a_pool. allocate(a_size);
}

/**
 *	Disposal operator that returns previously allocated memory to the byte pool.
 */
inline void operator delete (void *, utility::memory_pool&)
{
}

#endif /* UTIL_MEMORY_POOL_HPP_ */
