/*
 * @file $/include/nparse/specs/sg/label.hpp
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
#ifndef NPARSE_SPECS_SG_LABEL_HPP_
#define NPARSE_SPECS_SG_LABEL_HPP_

namespace nparse {

// (forward declaration, see interfaces.hpp for the definition)
class IStaging;

/**
 *	An auxiliary structure for passing arbitrary data to the semantic handlers
 *	at invocation points.
 */
struct hnd_arg_t
{
	IStaging& staging;
	const anta::State<SG>& state;

private:
	// NOTE: By making the only constructor explicit and private we make sure
	//		 that the class can be instantiated only in specificly allowed
	//		 locations in the source code (see the friend list below).
	explicit hnd_arg_t (IStaging& a_staging, const anta::State<SG>& a_state):
		staging (a_staging), state (a_state)
	{
	}

	friend class anta::Label<SG>;

};

/**
 *	The semantic handler type.
 */
typedef utility::callback<bool, const hnd_arg_t&> hnd_t;

} // namespace nparse

/**
 *	The anta::Label<...> template specialization for the script grammar model
 *	makes it possible to attach the deferred semantic actions to the elements of
 *	the script grammar.
 */
namespace anta {

template <>
class Label<nparse::SG>
{
public:
	/**
	 *	Must-have member functions.
	 *	@{ */

	Label (const char* a_str, const int a_int = 0):
		m_str (a_str), m_int (a_int)
	{
	}

	Label (const std::string& a_str, const int a_int = 0):
		m_str (a_str), m_int (a_int)
	{
	}

	Label (const int a_int):
		m_int (a_int)
	{
	}

	Label (const nparse::hnd_t& a_hnd):
		m_int (0), m_hnd (a_hnd)
	{
	}

	Label (const bool a_actual = false):
		m_str (a_actual ? "." : ""), m_int (0)
	{
	}

	const std::string& get () const
	{
		return m_str;
	}

	bool is_actual () const
	{
		return !(m_str. empty() && m_int == 0 && m_hnd. empty());
	}

	void advance (const Label& a_modifier)
	{
		// Never advance void labels.
		if (! is_actual())
		{
			return ;
		}

		// If the given modifier is void then void the label.
		if (! a_modifier. is_actual())
		{
			m_str. clear();
			m_int = 0;
			m_hnd. clear();
			return ;
		}

		// Advancing the string value of the label.
		if (! a_modifier. m_str. empty())
		{
			if (m_str == ".")
			{
				m_str = a_modifier. m_str;
			}
			else
			{
				m_str = a_modifier. m_str + "." + m_str;
			}
		}

		// Advancing the integer value of the label.
		if (m_int == 0 && a_modifier. m_int != 0)
		{
			m_int = a_modifier. m_int;
		}

		// Advancing the attached callback of the label.
		if (m_hnd. empty() && ! a_modifier. m_hnd. empty())
		{
			m_hnd = a_modifier. m_hnd;
		}
	}

	/**	@} */

public:
	int get_int () const
	{
		return m_int;
	}

	bool execute (nparse::IStaging& a_staging, const State<nparse::SG>& a_state)
		const
	{
		return m_hnd. empty()
			? true
			: m_hnd(nparse::hnd_arg_t(a_staging, a_state));
	}

private:
	std::string m_str;
	int m_int;
	nparse::hnd_t m_hnd;

};

} // namespace anta

#endif /* NPARSE_SPECS_SG_LABEL_HPP_ */
