/*
 * @file $/source/nparse-port/src/nparse-python.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@nparse.com>
 
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
#include <string>
#include <map>
#include <sstream>
#include <boost/python.hpp>
#include <encode/encode.hpp>
#include <nparse-port/parser.hpp>

using namespace boost::python;
using namespace nparse;

class Variable2Python
{
	typedef std::map<std::size_t, PyObject*> recursive_references_t;

	template <typename T_>
	static PyObject* make_ptr (const T_& a_val)
	{
		return incref(object(a_val). ptr());
	}

	static PyObject* convert (const Variable& a_var, char* a_buf,
			const unsigned int a_buf_len, recursive_references_t& a_recref)
	{
		PyObject* res = NULL;

		switch (a_var. type())
		{
		case Variable::Boolean:
			res = make_ptr(a_var. as_boolean());
			break;

		case Variable::Integer:
			res = make_ptr(a_var. as_integer());
			break;

		case Variable::Real:
			res = make_ptr(a_var. as_real());
			break;

		case Variable::String:
			res = make_ptr(static_cast<const char*>(
				a_var. as_string(a_buf, a_buf_len)
			));
			break;

		case Variable::Array:
			{
				const std::size_t id = a_var. id();
				assert(id != 0);
				recursive_references_t::iterator found_at = a_recref. find(id);
				if (found_at != a_recref. end())
				{
					res = incref(found_at -> second);
				}
				else
				{
					res = PyDict_New();
					a_recref. insert(
							recursive_references_t::value_type(id, res));
					for (VariableIterator i(a_var. begin()); ! i. end(); ++ i)
					{
						PyDict_SetItem(res, make_ptr(i -> key()),
								convert(*i, a_buf, a_buf_len, a_recref));
					}
				}
			}
			break;

		case Variable::Null:
		default:
			// @todo: is there a more sane way to create None object?
			res = incref(object(). ptr());
			break;
		}

		return res;
	}

public:
	static PyObject* convert (const Variable& a_var)
	{
		char buf[NPARSE_STRBUF_SIZE];
		recursive_references_t recref;
		return convert(a_var, buf, sizeof(buf), recref);
	}

};

class location_t
{
public:
	const char* file;
	int line;
	int offset;

	location_t ():
		file (""), line (-1), offset (-1)
	{
	}

	std::string __str__ () const
	{
		std::stringstream tmp;
		tmp << file << " : " << line << " : " << offset;
		return tmp. str();
	}

};

class PParser: public Parser
{
	std::wstring m_text;

public:
	void compile (const char* a_grammar)
	{
		Parser::load(NULL, a_grammar);
	}

	void load (const char* a_filename)
	{
		Parser::load(a_filename, NULL);
	}

	bool parse (const std::wstring& a_input)
	{
		m_text. assign(a_input);
		return Parser::parse(m_text. data(), m_text. size());
	}

	template <typename Input_>
	bool parse (Input_ a_input)
	{
		// @todo: is there anything better than just keeping a copy?
		encode::wstring(a_input). swap(m_text);
		return Parser::parse(m_text. data(), m_text. size());
	}

	Variable get_all () const
	{
		return get();
	}

	std::string node () const
	{
		std::string res(NPARSE_STRBUF_SIZE, '\x00');
		res. erase(strlen(Parser::node(&* res. begin(), res. size())));
		return res;
	}

	std::string text () const
	{
		std::string res(NPARSE_STRBUF_SIZE, '\x00');
		res. erase(strlen(Parser::text(&* res. begin(), res. size())));
		return res;
	}

	location_t get_location (const int a_index) const
	{
		location_t loc; // dumbass Python can't pass value by reference
		loc. file = Parser::get_location(a_index, &loc. line, &loc. offset);
		return loc;
	}

};

BOOST_PYTHON_MODULE(nparse)
{
	to_python_converter<Variable, Variable2Python>();

	enum_<Parser::status_t>("status")
		.value("ready",				Parser::stReady)
		.value("steady",			Parser::stSteady)
		.value("running",			Parser::stRunning)
		.value("completed",			Parser::stCompleted)
		.value("logicError",		Parser::stLogicError)
		.value("syntaxError",		Parser::stSyntaxError)
		.value("syntaxAmbiguity",	Parser::stSyntaxAmbiguity)
		.value("runtimeError",		Parser::stRuntimeError)
	;

	class_<location_t>("location")
		.def_readonly("file",		&location_t::file)
		.def_readonly("line",		&location_t::line)
		.def_readonly("offset",		&location_t::offset)
		.def("__str__",				&location_t::__str__)
	;

	class_<PParser>("Parser")
		.def("version",				&PParser::version)

		.def("status",				&PParser::status)
		.def("status_str",			&PParser::status_str)

		.def("compile",				&PParser::compile)
		.def("load",				&PParser::load)

		.def("parse_unicode",
				(bool(PParser::*)(const std::wstring&))
				&PParser::parse)

		.def("parse",
				(bool(PParser::*)(const char*))
				&PParser::parse<const char*>)

		.def("next",				&PParser::next)

		.def("step",				&PParser::step)

		.def("rewind",				&PParser::rewind)

		.def("reset",				&PParser::reset)

		.def("get",					&PParser::get)
		.def("get",					&PParser::get_all)

		.def("label",				&PParser::label)

		.def("node",				&PParser::node)

		.def("text",				&PParser::text)

		.def("shift",				&PParser::shift)

		.def("set",
				(void(PParser::*)(const char*, const bool))
				&PParser::set)
		.def("set",
				(void(PParser::*)(const char*, const int))
				&PParser::set)
		.def("set",
				(void(PParser::*)(const char*, const double))
				&PParser::set)
		.def("set",
				(void(PParser::*)(const char*, const char*))
				&PParser::set)

		.def("clear",				&PParser::clear)

		.def("get_message_count",	&PParser::get_message_count)

		.def("get_message",			&PParser::get_message)

		.def("get_location",		&PParser::get_location)

		.def("get_trace_count",		&PParser::get_trace_count)

		.def("get_iteration_count",	&PParser::get_iteration_count)

		.def("get_context_count",	&PParser::get_context_count)

		.def("get_pool_usage",		&PParser::get_pool_usage)

		.def("get_pool_capacity",	&PParser::get_pool_capacity)
	;
}
