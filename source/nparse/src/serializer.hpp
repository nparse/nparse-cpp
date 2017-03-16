/*
 * @file $/source/nparse/src/serializer.hpp
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
#ifndef SRC_SERIALIZER_HPP_
#define SRC_SERIALIZER_HPP_

#include <yaml-cpp/yaml.h>

template <typename M_>
class Serializer: public YAML::Emitter
{
	int m_level;

	class lister:
		public std::iterator<std::output_iterator_tag, void, void, void, void>
	{
		Serializer& m_serializer;

	public:
		lister (Serializer& a_serializer):
			m_serializer (a_serializer)
		{
		}

		lister& operator= (
				const typename anta::ndl::context_entry<M_>::type& a_entry)
		{
			m_serializer. WriteEntry(a_entry);
			return *this;
		}

		lister& operator* () { return *this; }
		lister& operator++ () { return *this; }
		lister& operator++ (int) { return *this; }

#if defined(BOOST_MSVC)
		const lister& operator= (const lister& a_inst)
		{
			if (this != &a_inst)
			{
				this -> ~lister();
				new(this) lister(a_inst. m_serializer);
			}
			return *this;
		}
#endif
	};

public:
	Serializer ():
		m_level (0)
	{
	}

	void WriteEntry (const typename anta::ndl::context_entry<M_>::type& a_entry)
	{
		if (a_entry. first. empty())
			return ;

		*this
			<< YAML::Key
			<< static_cast<const std::string&>(a_entry. first)
			<< YAML::Value;

		if (a_entry. second. is_boolean())
			*this << a_entry. second. as_boolean();
		else
		if (a_entry. second. is_integer())
			*this << a_entry. second. as_integer();
		else
		if (a_entry. second. is_real())
			*this << a_entry. second. as_real();
		else
		if (a_entry. second. is_string())
			*this << encode::string(a_entry. second. as_string());
		else
		if (a_entry. second. is_array())
			WriteContext(* a_entry. second. as_array());
		else
		{
			assert(a_entry. second. is_null());
			*this << YAML::Null;
		}
	}

	Serializer& WriteContext (const anta::ndl::Context<M_>& a_context)
	{
		if (m_level)
			*this << YAML::Flow;
		*this << YAML::BeginMap;
		++ m_level;
		a_context. list(lister(*this), false);
		-- m_level;
		*this << YAML::EndMap;
		return *this;
	}

};

template <typename M_>
inline Serializer<M_>& operator<< (Serializer<M_>& a_serializer,
		const anta::ndl::Context<M_>& a_context)
{
	return a_serializer. WriteContext(a_context);
}

#endif /* SRC_SERIALIZER_HPP_ */
