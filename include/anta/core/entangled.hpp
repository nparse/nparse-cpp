/*
 * @file $/include/anta/core/entangled.hpp
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
#ifndef ANTA_CORE_ENTANGLED_HPP_
#define ANTA_CORE_ENTANGLED_HPP_

namespace anta {

/******************************************************************************/

/**
 *	Entangled. An object that provides entanglement identifier to its direct
 *	descendants.
 */
template <typename M_>
class Entangled: public Base<Entangled<M_>, M_>
{
public:
	/**
	 *	The only constructor.
	 */
	Entangled ():
		m_id (0), m_priority (0)
	{
	}

	/**
	 *	Set the object to an entangled state with the given identifier.
	 */
	void set_entanglement (const uint_t a_id)
	{
		m_id = a_id;
	}

	/**
	 *	Get entanglement identifier.
	 */
	uint_t get_entanglement () const
	{
		return m_id;
	}

	/**
	 *	Set entangled object priority.
	 */
	void set_priority (const uint_t a_priority)
	{
		m_priority = a_priority;
	}

	/**
	 *	Get entangled object priority.
	 */
	uint_t get_priority () const
	{
		return m_priority;
	}

private:
	uint_t m_id;
	uint_t m_priority;

};

/******************************************************************************/

} // namespace anta

#endif /* ANTA_CORE_ENTANGLED_HPP_ */
