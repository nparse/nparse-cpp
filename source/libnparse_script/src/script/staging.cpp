/*
 * @file $/source/libnparse_script/src/script/staging.cpp
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
#include <assert.h>
#include <vector>
#include <map>
#include <nparse/nparse.hpp>
#include <utility/free.hpp>
#include <utility/hash_gen.hpp>
#include "../static.hpp"

// bug in boost::filesystem::current_path
#include "../source_tree.hpp"

namespace {

using namespace nparse;

class Staging: public IStaging
{
	plugin::instance<IAcceptorFactory> m_factory;
	SourceTree<string_t> m_st;

	typedef std::vector<IAcceptor*> stored_t;
	stored_t m_stored;

	typedef std::map<string_t, anta::ndl::Cluster<NLG>*> clusters_t;
	clusters_t m_clusters;

	typedef std::map<unsigned long, const IAcceptor*> acceptors_t;
	acceptors_t m_acceptors;

	typedef std::vector<action_pointer> action_stack_t;
	action_stack_t m_action_stack;

	typedef std::vector<joint_pointer> joint_stack_t;
	joint_stack_t m_joint_stack;

	typedef std::vector<plugin::IFactory*> bound_factories_t;
	bound_factories_t m_bound_factories;

	string_t m_namespace;

public:
	Staging ():
		m_factory ("nparse.AcceptorFactory")
	{
		m_factory -> import("nparse.acceptors");
	}

	~Staging ()
	{
		utility::free_all(m_stored);
		utility::free2nds(m_clusters);
		utility::free2nds(m_acceptors);
		utility::free_all(m_bound_factories);
	}

public:
	// Overridden from IStaging:

	anta::ndl::Cluster<NLG>& cluster (const string_t& a_name)
	{
		string_t name = a_name;

		clusters_t::iterator found_at = m_clusters. find(name);
		if (found_at == m_clusters. end() && ! m_namespace. empty()
				&& name. find_first_of(L'.') == name. npos)
		{
			name. insert(0, m_namespace);
			found_at = m_clusters. find(name);
		}

		if (found_at == m_clusters. end())
		{
			const std::pair<clusters_t::iterator, bool> p = m_clusters. insert(
				clusters_t::value_type(name, new anta::ndl::Cluster<NLG>(name))
			);
			assert(p. second);
			found_at = p. first;
		}

		return *(found_at -> second);
	}

	const anta::Acceptor<NLG>& acceptor (const string_t& a_def)
	{
		const std::string& path = m_st. current_path();
		const unsigned long def_hash = utility::hash_gen<unsigned long>::f(
				a_def. begin(), a_def. end());
		acceptors_t::iterator found_at = m_acceptors. find(def_hash);
		if (found_at == m_acceptors. end())
		{
			const IAcceptor* instance = NULL;
			if (! m_factory -> generate(a_def, instance, path))
			{
				// NOTE: Yes, it's a logic error!
				throw std::logic_error("unable to generate acceptor from"
						" definition");
			}
			// @todo: def_hash may depend on current path for cetrain acceptors
			const std::pair<acceptors_t::iterator, bool> p =
				m_acceptors. insert(acceptors_t::value_type(def_hash, instance));
			assert(p. second);
			found_at = p. first;
		}
		return *(found_at -> second);
	}

	const anta::Acceptor<NLG>& acceptor (IAcceptor* a_ptr)
	{
		m_stored. push_back(a_ptr);
		return *a_ptr;
	}

	void push (const action_pointer& a_ptr)
	{
		m_action_stack. push_back(a_ptr);
	}

	action_pointer pop ()
	{
		assert(! m_action_stack. empty());
		const action_pointer top = m_action_stack. back();
		m_action_stack. pop_back();
		return top;
	}

	void swap (std::vector<action_pointer>& a_av)
	{
		m_action_stack. swap(a_av);
		m_action_stack. clear();
	}

	void import (const std::string& a_path, const bool a_reset)
	{
		m_st. import(a_path, a_reset);
	}

	bool load (anta::range<SG>::type& a_range)
	{
		SourceTree<string_t>::range r;
		if (! m_st. load(r))
			return false;
		a_range. first = &* r. first;
		a_range. second = &* r. second;
		m_namespace. clear();
		return true;
	}

	bool identify (const anta::iterator<SG>::type& a_iterator,
			std::string& a_file, int& a_line, int& a_offset) const
	{
		const SourceTree<string_t>::iterator iter(a_iterator);
		return m_st. identify(iter, a_file, a_line, a_offset);
	}

	void setNamespace (const string_t& a_namespace)
	{
		m_namespace = a_namespace;
		if (! m_namespace. empty())
			m_namespace. append(1, L'.');
	}

	const string_t& getNamespace () const
	{
		return m_namespace;
	}

	void pushJoint (const joint_pointer& a_ptr)
	{
		m_joint_stack. push_back(a_ptr);
	}

	joint_pointer popJoint ()
	{
		assert(! m_joint_stack. empty());
		const joint_pointer top = m_joint_stack. back();
		m_joint_stack. pop_back();
		return top;
	}

	void bind (plugin::IFactory* a_factory)
	{
		m_bound_factories. push_back(a_factory);
	}

	void extend (ex::generic_error& a_error) const
	{
		// Check whether the exception has been bound to a file already.
		if (boost::get_error_info<ex::file>(a_error))
			return;

		// Check whether the exception has location information.
		const anta::range<SG>::type* pin =
			boost::get_error_info<ex::location>(a_error);
		if (! pin)
			return;

		// Try to identify location.
		std::string file;
		int line, offset;
		if (! identify(pin -> first, file, line, offset))
			return;

		// Add file/line/offset to the exception.
		a_error << ex::file(file) << ex::line(line) << ex::offset(offset);
	}

};

} // namespace

PLUGIN_STATIC_EXPORT(
		Staging, script_staging, nparse.script.Staging, 1 )
