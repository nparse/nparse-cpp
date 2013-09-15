/*
 * @file $/source/libnparse_factory/src/factory.cpp
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
#include <algorithm>
#include <plugin/category.hpp>
#include <nparse/nparse.hpp>
#include "static.hpp"

namespace {

using namespace nparse;

/**
 *	A helper predicate that determines whether its string argument happens to be
 *	a suffix of the given string (entry).
 */
class suffix_match
{
	const std::string& m_entry;

public:
	suffix_match (const std::string& a_entry):
		m_entry (a_entry)
	{
	}

	bool operator() (const std::string& a_suffix)
	{
		return m_entry. length() >= a_suffix. length()
			&& std::equal(a_suffix. begin(), a_suffix. end(), m_entry. begin()
					+ (m_entry. length() - a_suffix. length()));
	}

};

/**
 *	A helper comparing predicate that maps the order of the given list of
 *	suffixes to an arbitrary string list.
 */
class suffix_list_compare
{
	const std::vector<std::string>& m_suffix_list;
	bool m_make_first;

	int order (const std::string& a_entry) const
	{
		std::vector<std::string>::const_iterator found_at = std::find_if(
				m_suffix_list. begin(), m_suffix_list. end(),
				suffix_match(a_entry));
		return	(m_make_first && found_at == m_suffix_list. end())
			?	-1
			:	static_cast<int>(found_at - m_suffix_list. begin());
	}

public:
	suffix_list_compare (const std::vector<std::string>& a_suffix_list,
		const bool a_make_first):
		m_suffix_list (a_suffix_list), m_make_first (a_make_first)
	{
	}

	typedef std::pair<std::string, IAcceptorGenerator*> value_type;
	bool operator() (const value_type& u, const value_type& v) const
	{
		return order(u. first) < order(v. first);
	}

};

/*
 *	Interface implementation.
 */
class AcceptorFactory: public IAcceptorFactory
{
public:
	// Overridden from IPrioritized:
	int priority () const
	{
		return 0;
	}

	// Overridden from IAcceptorGenerator:
	bool generate (const string_t& a_definition, const IAcceptor*& a_instance,
			const std::string& a_path);

	// Overridden from IAcceptorFactory:
	int import (const std::string& a_family);
	void reorder (const std::vector<std::string>& a_suffix_list,
			const bool a_make_first);

public:
	AcceptorFactory ()
	{
	}

private:
	typedef plugin::category<IAcceptorGenerator> generators_t;
	generators_t m_generators;

};

bool AcceptorFactory::generate (const string_t& a_definition,
		const IAcceptor*& a_instance, const std::string& a_path)
{
	a_instance = NULL;
	for (generators_t::iterator i = m_generators. begin();
			i != m_generators. end(); ++ i)
	{
		if (i -> second -> generate(a_definition, a_instance, a_path))
			return true;
	}
	return false;
}

int AcceptorFactory::import (const std::string& a_family)
{
	m_generators. import(a_family);
	std::sort(m_generators. begin(), m_generators. end(), less_priority());
	return static_cast<int>(m_generators. size());
}

void AcceptorFactory::reorder (const std::vector<std::string>& a_suffix_list,
		const bool a_make_first)
{
	std::stable_sort(m_generators. begin(), m_generators. end(),
			suffix_list_compare(a_suffix_list, a_make_first));
}

} // namespace

PLUGIN_STATIC_EXPORT(
		AcceptorFactory, acceptor_factory, nparse.AcceptorFactory, 1 )
