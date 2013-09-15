/*
The MIT License (MIT)
Copyright (c) 2010-2013 Alex S Kudinov <alex@nparse.com>
 
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
#ifndef PLUGIN_CATEGORY_HPP_
#define PLUGIN_CATEGORY_HPP_

#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/noncopyable.hpp>
#include "plugin.hpp"

namespace plugin {

namespace {

/**
 *	A helper functor that is used for building of collections of pluggable
 *	objects that are defined by a family name.
 */
template <typename Interface_, typename Container_>
class collector
{
	// NOTE: According to the semantics of plugin::manager::list function, this
	//		 class should be easy copyable. Therefore, it better not create any
	//		 heavy member fields - just references.
	const std::string& m_family;
	Container_& m_container;

public:
	collector (const std::string& a_family, Container_& a_container):
		m_family (a_family), m_container (a_container)
	{
	}

	bool operator() (const std::string& a_interface) const
	{
		// The family prefix should be shorter than an interface name.
		if (m_family. size() > a_interface. size())
			return true;

		// Check for the full or prefix match.
		std::pair<std::string::const_iterator, std::string::const_iterator> p =
			std::mismatch(m_family. begin(), m_family. end(),
					a_interface. begin());
		if (p. first == m_family. end() && (p. second == a_interface. end()
					|| *p. second == '.'))
		{
			// Instantiate the matched interface and store the obtained pointer
			// within the result collection.
			Interface_* instance = NULL;
			plugin::manager::instance(). create(a_interface, -1, instance);
			m_container. push_back(std::make_pair(a_interface, instance));
		}

		return true;
	}

};

} // namespace

template <typename Interface_>
class category: public boost::noncopyable
{
public:
	typedef std::vector<std::pair<std::string, Interface_*> > container_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::iterator iterator;
	typedef typename container_type::const_iterator const_iterator;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::difference_type difference_type;

	const_iterator begin () const
	{
		return m_container. begin();
	}

	const_iterator end () const
	{
		return m_container. end();
	}

	iterator begin ()
	{
		return m_container. begin();
	}

	iterator end ()
	{
		return m_container. end();
	}

	size_type size () const
	{
		return m_container. size();
	}

public:
	category ()
	{
	}

	category (const std::string& a_family)
	{
		import(a_family);
	}

	~category ()
	{
		clear();
	}

	void import (const std::string& a_family)
	{
		plugin::manager::instance(). list(
				collector<Interface_, container_type>(a_family, m_container));
	}

	void clear ()
	{
		if (! m_container. empty())
		{
			for (iterator i = m_container. begin(); i != m_container. end();
					++ i)
			{
				plugin::manager::instance(). destroy(i -> second);
			}
			m_container. clear();
		}
	}

private:
	container_type m_container;

};

} // namespace plugin

#endif /* PLUGIN_CATEGORY_HPP_ */
