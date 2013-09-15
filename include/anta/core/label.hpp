/*
 * @file $/include/anta/core/label.hpp
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
#ifndef ANTA_CORE_LABEL_HPP_
#define ANTA_CORE_LABEL_HPP_

namespace anta {

/******************************************************************************/

/**
 *	Label. An object that is used to attach arbitrary properties and
 *	functionality to network arcs.
 */
template <typename M_>
class Label
{
public:
	/**
	 *	Must-have member functions.
	 *	@{ */

	/**
	 *	Define-by-value constructor.
	 */
	Label (const int a_value):
		m_value (a_value)
	{
	}

	/**
	 *	Actual/formal label constructor.
	 */
	Label (const bool a_actual = false):
		m_value (a_actual ? 1 : 0)
	{
	}

	/**
	 *	Get the value of the label.
	 */
	int get () const
	{
		return m_value;
	}

	/**
	 *	Check whether the label is actual.
	 */
	bool is_actual () const
	{
		return m_value != 0;
	}

	/**
	 *	Advance the value of the label.
	 */
	void advance (const Label& a_modifier)
	{
		if (is_actual())
		{
			if (a_modifier. is_actual())
				m_value *= a_modifier. get();
			else
				m_value = 0;
		}
	}

	/**	@} */

private:
	int m_value;

};

} // namespace anta

#endif /* ANTA_CORE_LABEL_HPP_ */
