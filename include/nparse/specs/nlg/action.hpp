/*
 * @file $/include/nparse/specs/nlg/action.hpp
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
#ifndef NPARSE_SPECS_NLG_ACTION_HPP_
#define NPARSE_SPECS_NLG_ACTION_HPP_

namespace nparse {

/**
 *	Instances of the flow_control class are being thrown as meta exceptions in
 *	order to allow (or deny) transitions.
 */
class flow_control
{
	bool m_flag;

public:
	flow_control (const bool a_flag):
		m_flag (a_flag)
	{
	}

	operator bool () const
	{
		return m_flag;
	}

};

} // namespace nparse

namespace anta { namespace ndl {
using namespace nparse;

template <>
struct action<NLG>
{
	class type: public action_pointer
	{
	public:
		type (IAction* a_ptr):
			action_pointer (a_ptr)
		{
			assert( a_ptr != NULL );
		}

		type (const action_pointer& a_ptr):
			action_pointer (a_ptr)
		{
			assert( a_ptr. get() != NULL );
		}

		bool operator() (Processor<NLG>& a_proc) const
		{
			try
			{
				IEnvironment env(a_proc, false);
				(*this) -> evalVal(env);
				return true;
			}
			catch (nparse::flow_control& fc)
			{
				return (bool) fc;
			}
		}

	};

};

}} // namespace anta::ndl

#endif /* NPARSE_SPECS_NLG_ACTION_HPP_ */
