/*
The MIT License (MIT)
Copyright (c) 2010-2013,2015 Alex S Kudinov <alex@nparse.com>
 
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
#include <vector>
#include <map>
#include <utility/free.hpp>
#include <plugin/plugin.hpp>

#if __cplusplus < 201103L
template <typename M_>
typename M_::iterator erase_from_map(M_& a_map, typename M_::iterator& a_at) {
	typename M_::iterator next(a_at);
	++ next;
	a_map. erase(a_at);
	return next;
}
#else
template <typename M_>
typename M_::iterator erase_from_map(M_& a_map, typename M_::iterator& a_at) {
	return a_map. erase(a_at);
}
#endif

namespace plugin {

class static_manager: public manager
{
	friend class manager;

	typedef std::pair<IFactory*, bool> factory_info_t;
	typedef std::map<int, factory_info_t> version_map_t;
	typedef std::map<std::string, version_map_t> interface_map_t;

	interface_map_t m_im;

	typedef std::vector<IInterface*> singletones_t;
	singletones_t m_singletones;

	~static_manager ()
	{
		shutdown();
	}

public:
	// Overridden from plugin::manager:

	void install (IFactory* a_factory, const std::string& a_interface,
			const int a_version, const bool a_factory_owner)
	{
		// Search for interface name entry.
		interface_map_t::iterator i_at = m_im. find(a_interface);
		if (i_at == m_im. end())
		{
			// If there's no such then add a new entry.
			const std::pair<interface_map_t::iterator, bool> p = m_im. insert(
				interface_map_t::value_type(a_interface, version_map_t()));
			assert(p. second);
			i_at = p. first;
		}
		// Check whether a factory for the given interface version has been
		// registered already.
		version_map_t::iterator v_at = i_at -> second. find(a_version);
		if (v_at != i_at -> second. end())
		{
			throw interface_version_conflict();
		}
		// Bind the given factory to the specified interface version.
		const std::pair<version_map_t::iterator, bool> p =
			i_at -> second. insert(version_map_t::value_type(
						a_version, factory_info_t(a_factory, a_factory_owner)));
		assert(p. second);
	}

	void uninstall (IFactory* a_factory)
	{
		for (interface_map_t::iterator i = m_im. begin();
				i != m_im. end(); )
		{
			for (version_map_t::iterator j = i -> second. begin();
					j != i -> second. end(); )
			{
				if (j -> second. first == a_factory)
				{
					j = erase_from_map(i -> second, j);
				}
				else
				{
					++ j;
				}
			}
			if (i -> second. empty())
			{
				i = erase_from_map(m_im, i);
			}
			else
			{
				++ i;
			}
		}
	}

	bool is_provided (const std::string& a_interface, const int a_version) const
	{
		interface_map_t::const_iterator i_at = m_im. find(a_interface);
		if (i_at == m_im. end())
			return false;

		if (a_version > 0)
		{
			version_map_t::const_iterator v_at = i_at -> second. find(a_version);
			if (v_at == i_at -> second. end())
				return false;
		}

		return true;
	}

	void create (const std::string& a_interface, const int a_version,
			IInterface*& a_instance)
	{
		// Search for interface name entry.
		interface_map_t::const_iterator i_at = m_im. find(a_interface);
		if (i_at == m_im. end())
		{
			throw unknown_interface();
		}
		// Suppose the factory list is not empty.
		assert(! i_at -> second. empty());
		// First we try to find exact version match.
		version_map_t::const_iterator v_at = i_at -> second. find(a_version);
		if (v_at == i_at -> second. end())
		{
			// If threre is no exact version match then check whether any of the
			// implementation versions would fit the request.
			switch (a_version)
			{
			// Get the newest version.
			case -1:
				v_at = i_at -> second. find(i_at -> second. rbegin() -> first);
				break;

			// Get the oldest version.
			case 0:
				v_at = i_at -> second. begin();
				break;

			// Otherwise, try to find a factory that generates instances that
			// would fit any requested version.
			default:
				v_at = i_at -> second. find(0);
				if (v_at == i_at -> second. end())
				{
					throw unknown_interface_version();
				}
			}
		}
		// Let the factory create a new instance.
		if (v_at -> second. first -> create(a_instance, a_interface, a_version))
		{
			// If a brand new instance has been created then we're supposed to
			// remember the creator factory to use it afterwards for instance
			// destruction.
			// However, since the instance destruction is just a simple delete
			// operator, remembering regular instances and their factories is
			// considered to be an avoidable overhead. It's much cheaper to
			// remember singletones only, which is done in the false-branch of
			// this fork.
		}
		else
		{
			singletones_t::iterator s_at = std::lower_bound(
					m_singletones. begin(), m_singletones. end(), a_instance);
			if (s_at == m_singletones. end() || *s_at != a_instance )
				m_singletones. insert(s_at, a_instance);
		}
	}

	void destroy (IInterface* a_instance)
	{
		// Check whether the instance is a singleton and kill it otherwise.
		if (! std::binary_search(m_singletones. begin(), m_singletones. end(),
					a_instance))
		{
			delete a_instance;
		}
	}

	bool list (const callback_type& a_callback) const
	{
		for (interface_map_t::const_iterator i = m_im. begin();
				i != m_im. end(); ++ i)
		{
			if (! a_callback(i -> first))
				return false;
		}
		return true;
	}

	void shutdown ()
	{
		std::vector<IInterface*> garbage;

		for (interface_map_t::iterator i = m_im. begin(); i != m_im. end();
				++ i)
		{
			for (version_map_t::iterator v = i -> second. begin();
					v != i -> second. end(); ++ v)
			{
				if (v -> second. second) // is owner?
				{
					v -> second. first -> release();
					garbage. push_back(v -> second. first);
				}
			}
			i -> second. clear();
		}
		m_im. clear();

		utility::free_all(garbage);
	}

};

manager& manager::instance ()
{
	static static_manager s_instance;
	return s_instance;
}

install<void>::install (IFactory* a_factory, const std::string& a_interface,
		const int a_version)
{
	manager::instance(). install(a_factory, a_interface, a_version, false);
}

void eval (...)
{
}

} // namespace plugin
