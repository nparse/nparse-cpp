/*
 * @file $/include/anta/sas/set.hpp
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
#ifndef ANTA_SAS_SET_HPP_
#define ANTA_SAS_SET_HPP_

#include <encode/encode.hpp>
#include "test.hpp"

namespace anta { namespace sas {

class SetClass
{
public:
	SetClass (const encode::wrapper<std::wstring>& a_cc):
		m_cc (a_cc)
	{
		std::sort(m_cc. begin(), m_cc. end());
		m_cc. erase(std::unique(m_cc. begin(), m_cc. end()), m_cc. end());
	}

	template <typename Char_>
	bool operator() (const Char_ a_char) const
	{
		return std::binary_search(m_cc. begin(), m_cc. end(),
				static_cast<wchar_t>(a_char));
	}

private:
	std::wstring m_cc;

};

} // namespace sas

/**
 *
 *	@{ */

#if defined(ANTA_NDL_RULE_HPP_)
namespace ndl { namespace terminals {

using namespace sas;

template <typename Data_>
inline test_functor<SetClass> set (const Data_& a_cc)
{
	const SetClass cc(a_cc);
	const test_config<SetClass> cfg(cc);
	return test_functor<SetClass>(cfg);
}

}} // namespace ndl::terminals
#endif // defined(ANTA_NDL_RULE_HPP_)

/**	@} */

} // namespace anta

#endif /* ANTA_SAS_SET_HPP_ */
