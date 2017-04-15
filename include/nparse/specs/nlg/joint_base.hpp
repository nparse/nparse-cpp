/*
 * @file $/include/nparse/specs/nlg/joint_base.hpp
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
#ifndef NPARSE_SPECS_NLG_JOINT_BASE_HPP_
#define NPARSE_SPECS_NLG_JOINT_BASE_HPP_

namespace anta {

/**
 *	Injecting additional functionality to the JointBase class:
 *	1. Making the JointBase class able to control sequencing.
 */
template <>
class Base<ndl::JointBase<nparse::NLG>, nparse::NLG>
{
public:
	/**
	 *	Set virtual flag.
	 */
	void set_virtual (const bool a_virtual)
	{
		if (a_virtual)
			m_flags |= 0x10;
		else
			m_flags &= ~static_cast<int>(0x10);
	}

	/**
	 *	Check whether the virtual flag is set.
	 */
	bool is_virtual () const
	{
		return (m_flags & 0x10) == 0x10;
	}

	/**
	 *	Set compounding flavor.
	 */
	void set_flavor (const int a_side, const int a_flavor)
	{
		assert( a_side == 0 || a_side == 1 );
		assert( (a_flavor & ~static_cast<int>(0x3)) == 0 );
		m_flags =
				(m_flags & ~static_cast<int>(a_side ? 0xC : 0x3))
			|	(a_flavor << (a_side ? 2 : 0));
	}

	/**
	 *	Get compounding flavor.
	 */
	int get_flavor (const int a_side) const
	{
		assert( a_side == 0 || a_side == 1 );
		return (m_flags >> (a_side ? 2 : 0)) & 0x3;
	}

protected:
	/**
	 *	The only constructor.
	 */
	Base ():
		m_flags (0)
	{
	}

private:
	int m_flags; /**> bits: 0-1:left flavor, 2-3:right flavor, 4:virtual flag */

};

} // namespace anta

#endif /* NPARSE_SPECS_NLG_JOINT_BASE_HPP_ */
