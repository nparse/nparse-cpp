/*
 * @file $/include/anta/dsel/placeholder.hpp
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
#ifndef ANTA_DSEL_PLACEHOLDER_HPP_
#define ANTA_DSEL_PLACEHOLDER_HPP_

namespace anta { namespace dsel {

/******************************************************************************/

/**
 *
 */
template <typename M_>
class Placeholder
{
public:
	typedef utility::callback<typename ndl::context_value<M_>::type,
			Traveller<M_>&> callback_type;

	Placeholder (const callback_type& a_callback):
		m_callback (a_callback)
	{
	}

	template <typename Agent_>
	typename ndl::context_value<M_>::type operator() (Agent_& a_agent) const
	{
		return m_callback(a_agent);
	}

private:
	callback_type m_callback;

};

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_PLACEHOLDER_HPP_ */
