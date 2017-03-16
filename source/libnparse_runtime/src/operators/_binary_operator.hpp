/*
 * @file $/source/libnparse_runtime/src/operators/_binary_operator.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef SRC_BINARY_OPERATOR_HPP_
#define SRC_BINARY_OPERATOR_HPP_

#include <anta/sas/test.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>

using namespace nparse;

/**
 *	A basic class for binary operators.
 */
class BinaryOperator: public IOperator
{
public:
	// Overridden from IOperator:

	int priority () const
	{
		return m_priority;
	}

	void deploy (level_t a_current, level_t a_previous, level_t a_top) const
	{
		using namespace anta::ndl::terminals;
		switch (m_designation. size())
		{
		case 0:
			throw std::logic_error("operator designation cannot be empty");

		case 1:
			if (m_left_associative)
			{
				a_current = a_previous > *(space > m_designation[0] * M0 > space
						> a_top > pass * M1 * m_action);
			}
			else
			{
				a_current = a_previous > ~(space > m_designation[0] * M0 > space
						> a_current > pass * M1 * m_action);
			}
			break;

		default:
			if (m_left_associative)
			{
				a_current = a_previous > *(space > m_designation * M0 > space
						> a_top > pass * M1 * m_action);
			}
			else
			{
				a_current = a_previous > ~(space > m_designation * M0 > space
						> a_current > pass * M1 * m_action);
			}
			break;
		}
	}

public:
	BinaryOperator (const int a_priority, const std::string a_designation,
			const bool a_left_associative = true):
		m_priority (a_priority), m_designation (a_designation),
		m_left_associative (a_left_associative)
	{
		m_action = hnd_t(this, &BinaryOperator::push_action);
	}

protected:
	/**
	 *	Instantiates action representing a binary operator.
	 */
	virtual IAction* create_action (const anta::range<SG>::type& a_range,
			IStaging& a_staging) const = 0;

	/**
	 *	Pops two last actions from the staging stack and pushes a compound
	 *	action back.
	 */
	bool push_action (const hnd_arg_t& arg) const
	{
		arg. staging. push(create_action(get_marked_range(arg), arg. staging));
		return true;
	}

	const int m_priority;
	const std::string m_designation;
	const bool m_left_associative;
	anta::Label<SG> m_action;

};

#endif /* SRC_BINARY_OPERATOR_HPP_ */
