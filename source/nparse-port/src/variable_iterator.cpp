/*
 * @file $/source/nparse-port/src/variable_iterator.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.3

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
#include <cstdio> // for sprintf
#include <nparse/nparse.hpp>
#include <nparse-port/variable.hpp>
#include "variable_data.hpp"

namespace nparse {

VariableIterator::VariableIterator (VariableIteratorData* a_data):
	m_ (a_data)
{
}

VariableIterator::~VariableIterator ()
{
	if (m_ != NULL)
	{
		delete m_;
		m_ = NULL;
	}
}

VariableIterator::VariableIterator (const VariableIterator& a_iter):
	m_ (NULL)
{
	*this = a_iter;
}

const VariableIterator& VariableIterator::operator= (
		const VariableIterator& a_iter)
{
	if (this != &a_iter)
	{
		this -> ~VariableIterator();
		if (a_iter. m_ != NULL)
		{
			new(this) VariableIterator(new VariableIteratorData());
			m_ -> context = a_iter. m_ -> context;
			m_ -> begin = a_iter. m_ -> begin;
			m_ -> end = a_iter. m_ -> end;
		}
	}
	return *this;
}

bool VariableIterator::operator== (const VariableIterator& a_iter) const
{
	if (m_ == NULL)
	{
		return a_iter. m_ == NULL || a_iter. m_ -> begin == a_iter. m_ -> end;
	}
	else
	if (a_iter. m_ == NULL)
	{
		return m_ -> begin == m_ -> end;
	}
	else
	{
		return m_ -> begin == a_iter. m_ -> begin;
	}
}

const Variable& VariableIterator::operator* () const
{
	if (m_ == NULL)
	{
		static const Variable sc_dummy;
		return sc_dummy;
	}

	anta::ndl::context_key<NLG>::type key(*m_ -> begin);
	anta::ndl::context_value<NLG>::type value(m_ -> context -> val(key));
	// (no throwing beyond this point)
	VariableData* data = new VariableData;
	data -> entry. first. swap(key);
	data -> entry. second. swap(value);
	m_ -> current. ~Variable();
	new(& m_ -> current) Variable(data);
	return m_ -> current;
}

const VariableIterator& VariableIterator::operator++ ()
{
	if (m_ != NULL)
	{
		++ (m_ -> begin);
		if (m_ -> begin == m_ -> end)
		{
			this -> ~VariableIterator();
		}
	}
	return *this;
}

VariableIterator& VariableIterator::get (const int a_key)
{
	char buf[32];
	std::sprintf(buf, "%d", a_key);
	return get(&*buf);
}

VariableIterator& VariableIterator::get (const char* a_key)
{
	if (m_ != NULL)
	{
		const anta::ndl::context_key<NLG>::type key(a_key);
		VariableData::keys_type::const_iterator lower_at =
			std::lower_bound(m_ -> begin, m_ -> end, key);
		if (lower_at == m_ -> end || *lower_at != key)
		{
			this -> ~VariableIterator();
		}
		else
		{
			m_ -> begin = lower_at;
		}
	}
	else
	{
		this -> ~VariableIterator();
	}
	return *this;
}

} // namespace nparse
