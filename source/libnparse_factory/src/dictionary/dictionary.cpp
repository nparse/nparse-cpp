/*
 * @file $/source/libnparse_factory/src/dictionary/dictionary.cpp
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
#include <fstream>
#define BOOST_SPIRIT_UNICODE
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/boost_tuple.hpp> // MUST!
#include "dictionary.hpp"

typedef boost::tuple<std::string, std::vector<std::string> > category_data_t;

typedef boost::tuple<std::string, std::vector<category_data_t> > entry_data_t;

namespace qi = boost::spirit::qi;
namespace ce = boost::spirit::unicode;

template <typename I_ = std::string::const_iterator>
class GComment: public qi::grammar<I_>
{
	qi::rule<I_> m_comment;

public:
	GComment ():
		GComment::base_type (m_comment)
	{
		using ce::space;
		using ce::char_;
		using qi::eol;
		m_comment %=
				space
			|	("//" > *(char_ - eol) > eol)
			|	("/*" > *(char_ - "*/") > "*/");
	}

};

template <typename I_ = std::string::const_iterator,
		 typename S_ = GComment<I_> >
class GEntry: public qi::grammar<I_, S_, entry_data_t()>
{
	qi::rule<I_, S_, entry_data_t()> m_entry;
	qi::rule<I_, S_, category_data_t()> m_category;
	qi::rule<I_, S_, std::string()> m_prefix;
	qi::rule<I_, S_, std::string()> m_name;
	qi::rule<I_, S_, std::string()> m_item;

public:
	GEntry ():
		GEntry::base_type (m_entry)
	{
		using ce::space;
		using ce::char_;
		using qi::lexeme;
		using qi::eoi;
		m_entry %= m_prefix > -(m_category % ',') > ';';
		m_category %= m_name > '=' > '{' > (m_item % ',') > '}';
		m_prefix %= lexeme[ +(char_ - space) ];
		m_name %= lexeme[ char_("a-zA-Z") > *char_("a-zA-Z0-9") ];
		m_item %= lexeme[
				'"' > *("\\\"" > qi::attr('"') | char_ - '"') > '"'
			|	+(char_ - char_(" {},\"")) ];
	}

};

namespace dictionary {

bool Dictionary::parse_entry (std::string::const_iterator& a_begin,
		const std::string::const_iterator& a_end)
{
	static const GEntry<> sc_entry;
	static const GComment<> sc_comment;
	entry_data_t ed;
	try
	{
		if (! qi::phrase_parse(a_begin, a_end, sc_entry, sc_comment, ed))
			return false;
	}
	catch (const qi::expectation_failure<std::string::const_iterator>&)
	{
		return false;
	}
	push_back(entry_t());
	entry_t& entry = back();
	entry. first. swap(boost::get<0>(ed));
	BOOST_FOREACH (category_data_t& cd, boost::get<1>(ed))
	{
		entry. second. push_back(category_t());
		entry. second. back(). first. swap(boost::get<0>(cd));
		entry. second. back(). second. swap(boost::get<1>(cd));
	}
	return true;
}

bool Dictionary::parse_entry (const std::string& a_definition)
{
	std::string::const_iterator b = a_definition. begin(),
		e = a_definition. end();
	if (parse_entry(b, e))
	{
		if (b == e)
			return true;
		erase(begin() + (size() - 1));
	}
	return false;
}

int Dictionary::load (const std::string& a_filename)
{
	std::ifstream file(a_filename. c_str());
	if (! file)
		return -1;

	// @todo: line-by-line loading doesn't work with comments, etc.
	int line_number = 0;
	std::string line;
	while (std::getline(file, line))
	{
		++ line_number;
		if (! parse_entry(line))
		{
			file. close();
			return line_number;
		}
	}

	file. close();
	return 0;
}

} // namespace dictionary
