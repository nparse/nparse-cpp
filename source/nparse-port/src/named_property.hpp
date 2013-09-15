/*
 * @file $/source/nparse-port/src/named_property.hpp
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
#ifndef SRC_NAMED_PROPERTY_HPP_
#define SRC_NAMED_PROPERTY_HPP_

#include <string>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <encode/encode.hpp>

template <typename T_>
class named_property;

class named_property_interface
{
public:
	virtual ~named_property_interface () {}

	virtual void set (const bool&) = 0;
	virtual void set (const int&) = 0;
	virtual void set (const double&) = 0;
	virtual void set (const std::string&) = 0;

	virtual named_property_interface& lock (const std::string&)
	{
		return *this;
	}

public:
	void set (const char* a_nstr)
	{
		set(encode::string(a_nstr));
	}

	void set (const wchar_t* a_wstr)
	{
		set(encode::string(a_wstr));
	}

	void set (const std::wstring& a_str)
	{
		set(encode::string(a_str));
	}

	template <typename V_>
	const V_& operator= (const V_& a_val)
	{
		this -> set(a_val);
		return a_val;
	}

};

template <typename T_>
class named_property_reference: public named_property_interface
{
public:
	named_property_reference ():
		m_ptr (NULL)
	{
	}

	void bind (T_& a_var)
	{
		m_ptr =& a_var;
	}

	named_property_interface& self ()
	{
		return *this;
	}

public:
	// Overridden from named_property_interface:
	void set (const T_& a_val)
	{
		*m_ptr = a_val;
	}

protected:
	T_* m_ptr;

};

template <>
class named_property<bool>: public named_property_reference<bool>
{
public:
	void set (const int& a_val)
	{
		self(). set( (a_val != 0) ? true : false );
	}

	void set (const double& a_val)
	{
		self(). set( (a_val != 0.0) ? true : false );
	}

	void set (const std::string& a_val)
	{
		if (a_val == "true")
			self(). set(true);
		else
		if (a_val == "false")
			self(). set(false);
		else
			throw std::bad_cast();
	}

};

template <>
class named_property<int>: public named_property_reference<int>
{
public:
	void set (const bool& a_val)
	{
		self(). set( static_cast<int>(a_val ? 1 : 0) );
	}

	void set (const double& a_val)
	{
		self(). set( static_cast<int>(a_val) );
	}

	void set (const std::string& a_val)
	{
		self(). set( boost::lexical_cast<int>(a_val) );
	}

};

template <>
class named_property<double>: public named_property_reference<double>
{
public:
	void set (const bool& a_val)
	{
		self(). set( static_cast<double>(a_val ? 1.0 : 0.0) );
	}

	void set (const int& a_val)
	{
		self(). set( static_cast<double>(a_val) );
	}

	void set (const std::string& a_val)
	{
		self(). set( boost::lexical_cast<double>(a_val) );
	}

};

template <>
class named_property<std::string>: public named_property_reference<std::string>
{
public:
	void set (const bool& a_val)
	{
		self(). set( a_val ? "true" : "false" );
	}

	void set (const int& a_val)
	{
		self(). set( boost::lexical_cast<std::string>(a_val) );
	}

	void set (const double& a_val)
	{
		self(). set( boost::lexical_cast<std::string>(a_val) );
	}

};

template <typename K_, typename T_, typename C_, typename A_>
class named_property<std::map<K_, T_, C_, A_> >:
	public named_property_reference<std::map<K_, T_, C_, A_> >
{
	const std::string* m_name_ptr;

	template <typename V_>
	void deliver (const V_& a_val)
	{
		// implicit V_ -> T_ conversion
		(*(this -> m_ptr))[*m_name_ptr] = a_val;
	}

public:
	named_property ():
		m_name_ptr (NULL)
	{
	}

public:
	// Overridden from named_property_interface:
	named_property_interface& lock (const std::string& a_name)
	{
		m_name_ptr =& a_name;
		return *this;
	}

public:
	// Overridden from named_property_reference:
	void set (const bool& a_val)
	{
		deliver(a_val);
	}

	void set (const int& a_val)
	{
		deliver(a_val);
	}

	void set (const double& a_val)
	{
		deliver(a_val);
	}

	void set (const std::string& a_val)
	{
		deliver(a_val);
	}

};

class named_property_map
{
	typedef boost::ptr_map<std::string, named_property_interface> map_type;
	map_type m_map;
	std::auto_ptr<named_property_interface> m_fallback;
	bool m_changed;

public:
	named_property_map ():
		m_changed (false)
	{
	}

	template <typename T_>
	named_property_map& operator() (const std::string& a_name, T_& a_ref)
	{
		named_property<T_>* pp = new named_property<T_>();
		pp -> bind(a_ref);
#if 1	// std::auto_ptr is a workaround for
		// https://svn.boost.org/trac/boost/ticket/6089
		m_map. insert(a_name, std::auto_ptr<named_property_interface>(pp));
#else
		m_map. insert(a_name, pp);
#endif
		return *this;
	}

	template <typename T_>
	named_property_map& operator() (T_& a_ref)
	{
		named_property<T_>* pp = new named_property<T_>();
		pp -> bind(a_ref);
		m_fallback. reset(pp);
		return *this;
	}

	named_property_interface& operator[] (const std::string& a_name)
	{
		map_type::iterator found_at = m_map. find(a_name);
		if (found_at != m_map. end())
		{
			m_changed = true;
			return found_at -> second -> lock(a_name);
		}
		else
		{
			return m_fallback -> lock(a_name);
		}
	}

	bool changed () const
	{
		return m_changed;
	}

	void update ()
	{
		m_changed = false;
	}

};

#endif /* SRC_NAMED_PROPERTY_HPP_ */
