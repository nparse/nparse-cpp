/*
 * @file $/include/anta/dsel/reference.hpp
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
#ifndef ANTA_DSEL_REFERENCE_HPP_
#define ANTA_DSEL_REFERENCE_HPP_

namespace anta { namespace dsel {

/******************************************************************************/

/**
 *
 */
template <typename M_>
class Reference
{
public:
	typedef typename ndl::context_key<M_>::type key_type;
	typedef typename ndl::context_value<M_>::type& ref_type;
	typedef const typename ndl::context_value<M_>::type& val_type;

	Reference (const key_type& a_key):
		m_key (a_key)
	{
	}

	const key_type& key () const
	{
		return m_key;
	}

	template <typename Context_>
	ref_type ref (Context_& a_context) const
	{
		return a_context. ref(key());
	}

	template <typename Context_>
	val_type val (const Context_& a_context) const
	{
		return a_context. val(key());
	}

private:
	key_type m_key;

};

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_REFERENCE_HPP_ */
