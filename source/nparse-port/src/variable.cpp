/*
 * @file $/source/nparse-port/src/variable.cpp
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
#include <string.h> // for memcpy
#include <nparse/nparse.hpp>
#include <nparse-port/variable.hpp>
#include "variable_data.hpp"

namespace nparse {

// @todo: This template is generic enough to be moved somewhere outside.
template <typename Keys_>
class lister:
	public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	Keys_& m_keys;

public:
	lister (Keys_& a_keys):
		m_keys (a_keys)
	{
	}

	lister& operator= (const anta::ndl::context_entry<NLG>::type& a_pair)
	{
		if (! a_pair. first. empty())
		{
			m_keys. insert(std::lower_bound(m_keys. begin(), m_keys. end(),
						a_pair. first), a_pair. first);
		}
		return *this;
	}

	lister& operator* () { return *this; }
	lister& operator++ () { return *this; }
	lister& operator++ (int) { return *this; }

#if defined(BOOST_MSVC)
	lister& operator= (const lister& a_inst)
	{
		if (this != &a_inst)
		{
			this -> ~lister();
			new(this) lister(m_keys);
		}
		return *this;
	}
#endif

};

Variable::Variable (VariableData* a_):
	m_ (a_)
{
}

Variable::~Variable ()
{
	if (m_ != NULL)
	{
		delete m_;
		m_ = NULL;
	}
}

Variable::Variable (const Variable& a_inst)
{
	*this = a_inst;
}

const Variable& Variable::operator= (const Variable& a_inst)
{
	if (this != &a_inst)
	{
		this->~Variable();
		new(this) Variable(a_inst. m_ ? new VariableData(*a_inst. m_) : NULL);
	}
	return *this;
}

void Variable::swap (Variable& a_inst)
{
	std::swap(m_, a_inst. m_);
}

Variable::type_t Variable::type () const
{
	switch (m_ ? m_ -> entry. second. id() : anta::aux::type_id::null)
	{
	case anta::aux::type_id::boolean:
		return Boolean;
	case anta::aux::type_id::integer:
		return Integer;
	case anta::aux::type_id::real:
		return Real;
	case anta::aux::type_id::string:
		return String;
	case anta::aux::type_id::array:
		return Array;
	default:
		return Null;
	}
}

const char* Variable::tag () const
{
	return m_ ? m_ -> entry. second. tag() : "null";
}

const char* Variable::key () const
{
	// The following static cast is based on the assumption that the context key
	// type is compatible or derived from the standard c++ string.
	return m_
		? static_cast<const std::string&>(m_ -> entry. first). c_str()
		: "";
}

std::size_t Variable::id () const
{
	return (m_ && m_ -> entry. second. is_array())
		? reinterpret_cast<std::size_t>(&* m_ -> entry. second. as_array())
		: 0;
}

bool Variable::get_boolean (const bool a_def) const
{
	return (m_ && m_ -> entry. second. is_boolean())
		? m_ -> entry. second. as_boolean()
		: a_def;
}

long Variable::get_integer (const long a_def) const
{
	return (m_ && m_ -> entry. second. is_integer())
		? m_ -> entry. second. as_integer()
		: a_def;
}

double Variable::get_real (const double a_def) const
{
	return (m_ && m_ -> entry. second. is_real())
		? m_ -> entry. second. as_real()
		: a_def;
}

char* Variable::get_string (char* a_buf, const int a_len,
		const char* a_def) const
{
	std::string temp;

	if (m_ && m_ -> entry. second. is_string())
	{
		temp = m_ -> entry. second. as_string();
	}
	else
	{
		temp = a_def;
	}

	const int len = std::min<int>(a_len - 1, temp. size());

	if (len >= 0)
	{
		memcpy(a_buf, temp. data(), len);
		a_buf[len] = 0;
	}

	return a_buf;
}

bool Variable::as_boolean () const
{
	return m_ ? m_ -> entry. second. as_boolean() : false;
}

long Variable::as_integer () const
{
	return m_ ? m_ -> entry. second. as_integer() : 0L;
}

double Variable::as_real () const
{
	return m_ ? m_ -> entry. second. as_real() : 0.0;
}

char* Variable::as_string (char* a_buf, const int a_len) const
{
	if (m_ != NULL)
	{
		const std::string temp = m_ -> entry. second. as_string();
		const int len = std::min<int>(a_len - 1, temp. size());
		if (len >= 0)
		{
			memcpy(a_buf, temp. data(), len);
			a_buf[len] = 0;
		}
	}
	else if (a_len > 0)
	{
		a_buf[0] = 0;
	}
	return a_buf;
}

VariableIterator Variable::begin () const
{
	VariableIteratorData* data = NULL;
	if (size() > 0)
	{
		data = new VariableIteratorData;
		data -> context = &* m_ -> entry. second. as_array();
		data -> begin = m_ -> keys -> begin();
		data -> end = m_ -> keys -> end();
	}
	return VariableIterator(data);
}

VariableIterator Variable::end () const
{
	return VariableIterator(NULL);
}

int Variable::size () const
{
	switch (m_ ? m_ -> entry. second. id() : anta::aux::type_id::null)
	{
	case anta::aux::type_id::array:
		if (! m_ -> keys)
		{
			VariableData::keys_type* keys = new VariableData::keys_type();
			m_ -> entry. second. as_array() -> list(
					lister<VariableData::keys_type>(*keys));
			if (! keys -> empty())
			{
				m_ -> keys. reset(keys);
			}
			else
			{
				delete keys;
				keys = NULL;
			}
		}
		if (m_ -> keys)
		{
			return static_cast<int>(m_ -> keys -> size());
		}
		break;

	default:
		break;
	}

	return 0;
}

VariableIterator Variable::get (const long a_key) const
{
	VariableIterator it = begin();
	it. seek(a_key);
	return it;
}

VariableIterator Variable::get (const char* a_key) const
{
	VariableIterator it = begin();
	it. seek(a_key);
	return it;
}

} // namespace nparse
