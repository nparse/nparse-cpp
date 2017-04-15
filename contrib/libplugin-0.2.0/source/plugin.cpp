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
#include <assert.h>
#include <utility>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <plugin/plugin.hpp>

namespace plugin {

struct dependency_t
{
	IFactory* factory;
	bool factory_ownership;
	IPluggable* instance;
	bool instance_constructed;
	bool instance_initialized;
	unsigned long reference_count;
	std::vector<std::string> dependencies;

	dependency_t (IFactory* a_factory, const bool a_ownership):
		factory (a_factory),
		factory_ownership (a_ownership),
		instance (NULL),
		instance_constructed (false),
		instance_initialized (false),
		reference_count (0)
	{
	}

};

typedef std::map<std::string, dependency_t> dependencies_t;

struct instance_pointer_comparator
{
	bool operator() (const dependency_t* p, const IPluggable* q) const
	{
		return p -> instance < q;
	}

};

class factory_pointer_matcher
{
	const IFactory* m_factory;

public:
	factory_pointer_matcher (IFactory* a_factory):
		m_factory (a_factory)
	{
	}

	bool operator() (const dependencies_t::value_type& p) const
	{
		return p. second. factory == m_factory;
	}

};

class DefaultManager: public IManager
{
	dependencies_t m_dependencies;
	std::vector<dependency_t*> m_instance_index;
	std::stack<std::vector<std::string>*> m_creation;

public:
	bool register_factory (const std::string& a_interface_tag,
			IFactory* a_factory, const bool a_transfer_ownership);

	bool unregister_factory (IFactory* a_factory);

	bool factory_exists (const std::string& a_interface_tag) const;

	IPluggable* create (const std::string& a_interface_tag);

	IPluggable* validate (const IPluggable* a_instance) const;

	bool dispose (IPluggable* a_instance);

	void list (const callback_type& a_callback) const;

	void shutdown ();

private:
	bool destroy (IPluggable* a_instance, const bool a_force);

	DefaultManager ();

	~DefaultManager ();

	friend class IManager;

};

bool DefaultManager::register_factory (const std::string& a_interface_tag,
		IFactory* a_factory, const bool a_delegate_factory_ownership)
{
	dependencies_t::iterator i = m_dependencies. find(a_interface_tag);
	if (i != m_dependencies. end())
	{
		return false;
	}
	const std::pair<dependencies_t::iterator, bool> p = m_dependencies. insert(
			dependencies_t::value_type(a_interface_tag, dependency_t(a_factory,
					a_delegate_factory_ownership)));
	assert(p. second);
	return true;
}

bool DefaultManager::unregister_factory (IFactory* a_factory)
{
	dependencies_t::iterator i = std::find_if(m_dependencies. begin(),
			m_dependencies. end(), factory_pointer_matcher(a_factory));
	if (i == m_dependencies. end())
	{
		return false;
	}
	if (i -> second. instance)
	{
		destroy(i -> second. instance, true);
	}
	if (i -> second. factory_ownership)
	{
		IFactory* factory = i -> second. factory;
		i -> second. factory = NULL;
		i -> second. factory_ownership = false;
		delete factory;
	}
	m_dependencies. erase(i);
	return true;
}

bool DefaultManager::factory_exists (const std::string& a_interface_tag) const
{
	return m_dependencies. find(a_interface_tag) != m_dependencies. end();
}

IPluggable* DefaultManager::create (const std::string& a_interface_tag)
{
	// Verify that a factory producing instances of the requested type has been
	// registered.
	dependencies_t::iterator i = m_dependencies. find(a_interface_tag);
	if (i == m_dependencies. end())
	{
		return NULL;
	}

	// Increment instance reference count.
	i -> second. reference_count += 1;

	// If an instance of the requested type is already being created, simply
	// return the cached pointer.
	if (i -> second. instance)
	{
		assert(i -> second. reference_count > 1);
		return i -> second. instance;
	}

	// If this is a nested create request, add the requested type as an
	// unresolved dependency for the instance that is currently being created.
	if (! m_creation. empty())
	{
		m_creation. top() -> push_back(a_interface_tag);
	}

	// Allocate memory for the new instance.
	i -> second. instance = reinterpret_cast<IPluggable*>(
			i -> second. factory -> allocate());

	// Add instance to the index for quick search by pointer.
	std::vector<dependency_t*>::iterator j = std::lower_bound(
			m_instance_index. begin(), m_instance_index. end(),
			i -> second. instance, instance_pointer_comparator());
	if (j == m_instance_index. end() || *j != & i -> second)
	{
		m_instance_index. insert(j, & i -> second);
	}

	// Push instance dependency list to the top of creation stack.
	m_creation. push(& i -> second. dependencies);

	// Perform actual instance construction at the previously allocated memory
	// address.
	i -> second. instance = i -> second. factory -> construct(
			i -> second. instance);
	i -> second. instance_constructed = true;

	// Pop instance dependency list from the creation stack.
	m_creation. pop();

	// If the creation stack is empty, that means the current instance is not
	// some other instances's dependency and it is safe to perform the
	// post-construction instance initialization step.
	if (m_creation. empty())
	{
		// TODO: Initialize dependencies in weak topological order.
		for (dependencies_t::iterator d = m_dependencies. begin();
				d != m_dependencies. end(); ++ d)
		{
			if (! d -> second. instance_initialized && d -> second. instance)
			{
				d -> second. instance -> initialize();
				d -> second. instance_initialized = true;
				d -> second. dependencies. clear();
			}
		}
	}

	// Finally, the pointer to the constructed, but not necessarily initialized
	// instance is returned.
	return i -> second. instance;
}

IPluggable* DefaultManager::validate (const IPluggable* a_instance) const
{
	std::vector<dependency_t*>::const_iterator i = std::lower_bound(
			m_instance_index. begin(), m_instance_index. end(), a_instance,
			instance_pointer_comparator());
	if (i != m_instance_index. end() && (*i) -> instance == a_instance)
	{
		if ((*i) -> instance_constructed)
		{
			return (*i) -> instance;
		}
		else
		{
			throw using_incomplete_instance();
		}
	}
	else
	{
		throw using_invalid_instance();
	}
}

bool DefaultManager::dispose (IPluggable* a_instance)
{
	return destroy(a_instance, false);
}

void DefaultManager::list (const callback_type& a_callback) const
{
	for (dependencies_t::const_iterator i = m_dependencies. begin();
			i != m_dependencies. end(); ++ i)
	{
		if (! a_callback(i -> first))
		{
			break;
		}
	}
}

void DefaultManager::shutdown ()
{
	assert(m_creation. empty());
	while (! m_dependencies. empty())
	{
		dependencies_t::iterator i = m_dependencies. begin();
		destroy(i -> second. instance, true);
		unregister_factory(i -> second. factory);
	}
	assert(m_instance_index. empty());
}

bool DefaultManager::destroy (IPluggable* a_instance, const bool a_force)
{
	if (! a_instance)
	{
		return false;
	}

	std::vector<dependency_t*>::iterator i = std::lower_bound(
			m_instance_index. begin(), m_instance_index. end(), a_instance,
			instance_pointer_comparator());
	if (i != m_instance_index. end() && (*i) -> instance == a_instance)
	{
		dependency_t* d = *i;
		if (a_force || (d -> reference_count -= 1) <= 0)
		{
			m_instance_index. erase(i);
			IPluggable* instance = d -> instance;
			void* address = NULL;
			d -> instance = NULL;
			if (d -> instance_initialized)
			{
			//	instance = d -> factory -> deinitialize(instance);
				d -> instance_initialized = false;
			}
			if (d -> instance_constructed)
			{
				address = d -> factory -> destruct(instance);
				d -> instance_constructed = false;
			}
			d -> factory -> deallocate(address);
			d -> reference_count = 0;
			return true;
		}
	}
	return false;
}

DefaultManager::DefaultManager ()
{
}

DefaultManager::~DefaultManager ()
{
	shutdown();
}

IManager& IManager::instance ()
{
	static DefaultManager s_manager_instance;
	return s_manager_instance;
}

void eval (...)
{
}

} // namespace plugin
