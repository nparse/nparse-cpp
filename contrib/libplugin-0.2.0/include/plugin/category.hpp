/*
The MIT License (MIT)
Copyright (c) 2010-2017 Alex Kudinov <alex.s.kudinov@gmail.com>

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
 *	An auxiliary helper functor used for building collections of pluggable
 *	objects selected by category name, i.e. common interface name prefix.
 */
template <typename T_>
class collector
{
public:
	typedef std::vector<std::pair<std::string, T_*> > container_type;

	collector (const std::string& a_category, container_type& a_container):
		m_category (a_category),
		m_container (a_container)
	{
	}

	bool operator() (const std::string& a_interface_tag) const
	{
		// The category prefix should be shorter than interface name.
		if (m_category. size() > a_interface_tag. size())
		{
			return true; // keep going
		}

		// Check for the full or prefix match.
		std::pair<std::string::const_iterator, std::string::const_iterator> p =
			std::mismatch(m_category. begin(), m_category. end(),
					a_interface_tag. begin());
		if (p. first == m_category. end() &&
				(p. second == a_interface_tag. end() || *p. second == '.'))
		{
			// Request an instance from the manager.
			IPluggable* ptr = IManager::instance(). create(a_interface_tag);
			if (! ptr)
			{
				throw interface_not_found();
			}
			// Cast the instance to the required type.
			T_* instance = dynamic_cast<T_*>(ptr);
			if (! instance)
			{
				IManager::instance(). dispose(ptr);
				throw interface_type_conflict();
			}
			// Store the instance pointer in the external container.
			m_container. push_back(std::make_pair(a_interface_tag, instance));
		}

		return true; // keep going
	}

private:
	const std::string& m_category;
	container_type& m_container;

};

} // anonymous namespace

/**
 *	category<T_> is a template class that represents an injection point for
 *	multiple pluggable objects managed by the static manager and belonging to
 *	the same category.
 */
template <typename T_>
class category: public boost::noncopyable
{
public:
	typedef std::vector<std::pair<std::string, T_*> > container_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::difference_type difference_type;
	typedef typename container_type::iterator iterator;
	typedef typename container_type::const_iterator const_iterator;

	category (const std::string& a_category)
	{
		IManager::instance(). list(collector<T_>(a_category, m_container));
	}

	~category ()
	{
		for (iterator i = m_container. begin(); i != m_container. end(); ++ i)
		{
			IManager::instance(). dispose(i -> second);
		}
		m_container. clear();
	}

	size_type size () const
	{
		return m_container. size();
	}

	iterator begin ()
	{
		return m_container. begin();
	}

	iterator end ()
	{
		return m_container. end();
	}

	const_iterator begin () const
	{
		return m_container. begin();
	}

	const_iterator end () const
	{
		return m_container. end();
	}

private:
	container_type m_container;

};

} // namespace plugin

#endif /* PLUGIN_CATEGORY_HPP_ */
