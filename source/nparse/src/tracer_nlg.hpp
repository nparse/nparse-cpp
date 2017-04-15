/*
 * @file $/source/nparse/src/tracer_nlg.hpp
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
#ifndef SRC_TRACER_NLG_
#define SRC_TRACER_NLG_

#include <anta/_aux/tracer.hpp>

class TracerNLG: public anta::aux::Tracer<nparse::NLG>
{
	typedef std::string (TracerNLG::*placeholder_t)() const;
	typedef std::map<std::string, placeholder_t> placeholders_t;
	placeholders_t m_placeholders;

	std::string m_format;
	int m_tr, m_st;

public:
	TracerNLG (const anta::Processor<nparse::NLG>& a_processor):
		anta::aux::Tracer<nparse::NLG> (a_processor),
		m_tr(0), m_st(0)
	{
		m_placeholders["i"] = &TracerNLG::trace;
		m_placeholders["j"] = &TracerNLG::state;
		m_placeholders["label"] = &TracerNLG::label;
		m_placeholders["node"] = &TracerNLG::node;
		m_placeholders["text"] = &TracerNLG::text;
		m_placeholders["list"] = &TracerNLG::list;
		m_placeholders["type"] = &TracerNLG::type_str;
	}

public:
	/**
	 *	Placeholder callbacks.
	 *	@{ */

	std::string trace () const
	{
		return boost::lexical_cast<std::string>(m_tr);
	}

	std::string state () const
	{
		return boost::lexical_cast<std::string>(m_st);
	}

	std::string label () const
	{
		return boost::lexical_cast<std::string>(
				(*this) -> get_arc(). get_label(). get());
	}

	std::string node () const
	{
		return encode::string((*this) -> get_arc(). get_target(). get_name());
	}

	std::string text () const
	{
		const anta::range<nparse::NLG>::type& r = (*this) -> get_range();
		return encode::string(r. first, r. second);
	}

	std::string list () const
	{
		return anta::aux::list(**this, true);
	}

	std::string type_str () const
	{
		return boost::lexical_cast<std::string>(type());
	}

	std::string variable (const std::string a_name) const
	{
		const nparse::IEnvironment::key_type key = a_name;
		const nparse::nlg_string_t val = (*this) -> val(key). as_string();
		return encode::string(val);
	}

	/**	@} */

public:
	// Overriden from anta::aux::Tracer<...>:

	bool next ()
	{
		++ m_tr;
		m_st = 0;
		return anta::aux::Tracer<nparse::NLG>::next();
	}

	bool step ()
	{
		++ m_st;
		return anta::aux::Tracer<nparse::NLG>::step();
	}

	void rewind ()
	{
		m_tr = 0;
		m_st = 0;
		anta::aux::Tracer<nparse::NLG>::rewind();
	}

public:
	void format (const std::string& a_format)
	{
		static const boost::regex sc_escape("(\\\\t)|(\\\\n)");

		std::stringstream buf;
		std::ostream_iterator<char, char> out(buf);

		boost::regex_replace(
			out,
			a_format. begin(), a_format. end(),
			sc_escape,
			"(?1\t)(?2\n)",
			boost::match_default | boost::format_all
		);

		m_format = buf. str();
	}

	std::string print () const
	{
		static const boost::regex sc_subst("%(\\{(:?\\w+)\\}|:?\\w+)");

		std::stringstream buf;
		std::ostream_iterator<char, char> out(buf);

		boost::regex_replace(
			out,
			m_format. begin(), m_format. end(),
			sc_subst,
			*this,
			boost::match_default | boost::format_all
		);

		return buf. str();
	}

	template <typename Match_, typename Iter_>
	Iter_ operator() (const Match_& a_match, Iter_& a_iter) const
	{
		const std::string field = (*(a_match[1]. first) == '{')
			? a_match[2]. str() : a_match[1]. str();

		std::string value;
		if (! field. empty() && *field. begin() == ':')
		{
			value = variable(field. substr(1));
		}
		else
		{
			const placeholders_t::const_iterator found_at =
				m_placeholders. find(field);
			if (found_at != m_placeholders. end())
			{
				value = (this ->* (found_at -> second))();
			}
			else
			{
				value = "%{" + field + '}';
			}
		}

		std::copy(value. begin(), value. end(), a_iter);

		return a_iter;
	}

};

#endif /* SRC_TRACER_NLG_ */
