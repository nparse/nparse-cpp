/*
 * @file $/source/nparse-port/src/parser.cpp
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
#include <string.h> // for memcpy
#include <cwchar> // for wcslen
#include <map>
#include <boost/scoped_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <nparse/nparse.hpp>
#include <anta/_aux/tracer.hpp>
#include <plugin/static.hpp>
#include <nparse-port/parser.hpp>
#include "variable_data.hpp"
#include "named_property.hpp"

PLUGIN_STATIC_IMPORT(nparse_script_grammar)
PLUGIN_STATIC_IMPORT(nparse_acceptor_factory)
PLUGIN_STATIC_IMPORT(nparse_runtime)

namespace nparse {

struct ParserData
{
	// named properties
	named_property_map config;

	// settings
	plugin::instance<IStaging> staging;
	plugin::instance<IConstruct> grammar;
	int grammar_pool;
	int input_pool;
	std::string input_swap;
	std::string entry_point;
	int entry_label;

	// initial values of trace variables
	typedef std::map<
		anta::ndl::context_key<NLG>::type,
		anta::ndl::context_value<NLG>::type> init_t;
	init_t init;

	// active elements
	boost::scoped_ptr<anta::Traveller<NLG> > traveller;
	boost::scoped_ptr<anta::aux::Tracer<NLG> > tracer;

	// messages
	typedef boost::tuple<std::string, std::string, int, int> message_t;
	typedef std::vector<message_t> messages_t;
	messages_t messages;

	// stats
	anta::uint_t iteration_count;
	int shift;
	bool first_trace;
	Parser::status_t status;

	ParserData ():
		staging ("nparse.script.Staging"),
		grammar ("nparse.script.Grammar"),
		grammar_pool (16 << 10),	// [ kB ]
		input_pool (16 << 10),		// [ kB ]
		entry_point ("S"),
		entry_label (1),
		// stats
		iteration_count (0),
		shift (0),
		first_trace (false),
		status (Parser::stReady)
	{
		config
			("grammar_pool", grammar_pool)
			("input_pool", input_pool)
			("input_swap", input_swap)
			("entry_point", entry_point)
			("entry_label", entry_label)
			(init); // fallback
	}

	void activate ()
	{
		traveller. reset(new anta::Traveller<NLG>(
			staging -> cluster(entry_point), entry_label));
#if defined(NPARSE_SWAP_FILE)
		if (! input_swap. empty())
			traveller -> set_swap_file(input_swap);
#endif
		traveller -> set_capacity(input_pool << 10);
		tracer. reset(new anta::aux::Tracer<NLG>(*traveller));
	}

	bool validate (const Parser::status_t a_status)
	{
		if (status == a_status)
			return true;
		status = Parser::stLogicError;
		return false;
	}

	// syntax error
	void report (const anta::observer<SG>::type& a_observer)
	{
		std::vector<deadlock_t> dls;
		a_observer. analyze(dls);

		std::string file;
		int line, offset;

		for (std::vector<deadlock_t>::const_iterator i = dls. begin();
				i != dls. end(); ++ i)
		{
			staging -> identify(dls. front(). first, file, line, offset);
			messages. push_back(message_t(
						"syntax error", file, line, offset));
		}

		status = Parser::stSyntaxError;
	}

	typedef anta::Traveller<SG>::traced_type traced_t;

	// syntax ambiguity
	void report (const traced_t& a_traced)
	{
		typedef const anta::State<SG>* state_t;
		typedef std::set<state_t> states_t;
		typedef std::set<anta::iterator<SG>::type> ambiguous_t;

		ambiguous_t ambiguous;
		states_t cache;
		for (traced_t::const_iterator t = a_traced. begin();
				t != a_traced. end(); ++ t)
		{
			state_t s = *t;
			while (s != NULL && cache. insert(s). second)
				s = s -> get_ancestor();
			if (s != NULL)
				ambiguous. insert(s -> get_range(). second);
		}

		std::string file;
		int line, offset;

		for (ambiguous_t::const_iterator p = ambiguous. begin();
				p != ambiguous. end(); ++ p)
		{
			staging -> identify(*p, file, line, offset);
			messages. push_back(message_t(
						"syntax ambiguity", file, line, offset));
		}

		status = Parser::stSyntaxAmbiguity;
	}

	// generic error
	void report (ex::generic_error& a_error)
	{
		std::string msg, loc;
		int line, offset;
		const std::string* str;

		staging -> extend(a_error);

		str = boost::get_error_info<ex::function>(a_error);
		if (str)
		{
			msg. append(*str);
			msg. append(": ", 2);
		}

		str = boost::get_error_info<ex::message>(a_error);
		if (str)
		{
			msg. append(*str);
		}

		str = boost::get_error_info<ex::file>(a_error);
		if (str)
		{
			const int* val;
			loc. append(*str);

			val = boost::get_error_info<ex::line>(a_error);
			if (val) line = *val;

			val = boost::get_error_info<ex::offset>(a_error);
			if (val) offset = *val;
		}

		messages. push_back(message_t( msg, loc, line, offset ));

		status = Parser::stRuntimeError;
	}

	// standard exception
	void report (const std::exception& a_error)
	{
		messages. push_back(message_t( a_error. what(), "", 0, 0 ));
		status = Parser::stRuntimeError;
	}

};

Parser::Parser ():
	m_ (NULL)
{
	PLUGIN_STATIC_INIT(nparse_script_grammar);
	PLUGIN_STATIC_INIT(nparse_acceptor_factory);
	PLUGIN_STATIC_INIT(nparse_runtime);
	m_ = new ParserData();
}

Parser::~Parser ()
{
	if (m_ != NULL)
	{
		delete m_;
		m_ = NULL;
	}
}

const char* Parser::version () const
{
	return NPARSE_VERSION_STR;
}

Parser::status_t Parser::status () const
{
	return m_ -> status;
}

const char* Parser::status_str () const
{
	static const char* status_msg[] = {
		"ready", "steady", "running", "completed", "logic error",
		"syntax error", "syntax ambiguity", "runtime error" };

	switch (m_ -> status)
	{
	case nparse::Parser::stReady:
	case nparse::Parser::stSteady:
	case nparse::Parser::stRunning:
	case nparse::Parser::stCompleted:
	case nparse::Parser::stLogicError:
	case nparse::Parser::stSyntaxError:
	case nparse::Parser::stSyntaxAmbiguity:
	case nparse::Parser::stRuntimeError:
		return status_msg[ static_cast<int>(m_ -> status) ];

	default:
		break;
	}

	return "";
}

void Parser::load (const char* a_filename, const char* a_grammar)
{
	if (! m_ -> validate(stReady))
		return;

	anta::Traveller<SG> traveller(m_ -> grammar -> entry());
	anta::aux::Tracer<SG> tracer(traveller);

	traveller. set_capacity(m_ -> grammar_pool << 10);
	traveller. get_observer(). set_trace_max(3);
	traveller. get_observer(). set_queue_max(1);

	if (a_filename)
	{
		m_ -> staging -> import(a_filename, true);
	}

	bool inlined = false;
	sg_string_t grammar;
	if (a_grammar)
	{
		inlined = true;
		sg_string_t(a_grammar). swap(grammar);
	}

	try
	{
		anta::range<SG>::type src;
		while (inlined || m_ -> staging -> load(src))
		{
			if (inlined)
			{
				inlined = false;
				src. first = &* grammar. begin();
				src. second = &* grammar. end();
			}

			traveller. run(src. first, src. second);

			switch (traveller. get_traced(). size())
			{
			case 1:
				break;

			case 0:
				// syntax error
				m_ -> report(traveller. get_observer());
				return;

			default:
				// syntax ambiguity
				m_ -> report(traveller. get_traced());
				return;
			}

			try
			{
				tracer. next();
				while (tracer. step())
				{
					tracer -> get_arc(). get_label(). execute(
						*(m_ -> staging), *tracer);
				}
			}
			catch (ex::compile_error& err)
			{
				m_ -> report(err);
				return;
			}

			tracer. rewind();
			traveller. reset();

			if (m_ -> status == stReady)
			{
				m_ -> status = stSteady;
				const string_t& ns = m_ -> staging -> getNamespace();
				if (! ns. empty())
					m_ -> entry_point = encode::string(ns) + m_ -> entry_point;
			}

			m_ -> staging -> setNamespace();
		}

		m_ -> activate();
	}
	catch (const std::bad_alloc&)
	{
		m_ -> report(std::runtime_error("grammar pool overflow"));
	}
	catch (const std::exception& err)
	{
		m_ -> report(err);
	}
}

bool Parser::parse (const wchar_t* a_input, const int a_len)
{
	if (! m_ -> validate(stSteady))
		return false;

	if (m_ -> config. changed())
	{
		m_ -> config. update();
		m_ -> activate(); // reactivate on configuration change
	}

	try
	{
		m_ -> status = stRunning;
		m_ -> traveller -> init(
				a_input,
				a_input + (a_len ? a_len : wcslen(a_input)));
		for (ParserData::init_t::const_iterator i = m_ -> init. begin();
				i != m_ -> init. end(); ++ i)
		{
			m_ -> traveller -> ref(i -> first) = i -> second;
		}
		m_ -> iteration_count = m_ -> traveller -> run();
		m_ -> first_trace = m_ -> tracer -> next();
		m_ -> status = stCompleted;
		return true;
	}
	catch (ex::runtime_error& err)
	{
		m_ -> report(err);
	}
	catch (const std::bad_alloc&)
	{
		m_ -> report(std::runtime_error("input pool overflow"));
	}
	catch (const std::exception& err)
	{
		m_ -> report(err);
	}

	return false;
}

bool Parser::next ()
{
	if (! m_ -> validate(stCompleted))
		return false;

	if (m_ -> first_trace)
	{
		m_ -> first_trace = false;
		return true;
	}

	return m_ -> tracer -> next();
}

bool Parser::step ()
{
	if (! m_ -> validate(stCompleted))
		return false;

	m_ -> shift = 0;

	try
	{
		anta::aux::Tracer<NLG>& tracer = *(m_ -> tracer);
		while (tracer. step())
		{
			switch (tracer. type())
			{
			case 1:
				return true;

			case 2:
				{
					// NOTE: Scared? See implementation of Tracer::relative()
					const anta::State<NLG>* s = tracer. relative("<:");
					// Determine if the parenthesis refers to a named rule.
					if (s && !s -> get_arc(). get_target(). get_name(). empty())
						++ (m_ -> shift);
				}
				break;
			}
		}

		++ (m_ -> shift);
	}
	catch (const std::exception& err)
	{
	//	m_ -> report(err);
	}

	return false;
}

void Parser::rewind ()
{
	if (! m_ -> validate(stCompleted))
		return;

	m_ -> tracer -> rewind();
	m_ -> first_trace = m_ -> tracer -> next();
}

void Parser::reset ()
{
	switch (m_ -> status)
	{
	case stReady:
		return;

	case stRunning:
		break; // @todo: throw critical exception

	case stLogicError:
	case stSyntaxError:
	case stSyntaxAmbiguity:
	case stRuntimeError:
		m_ -> messages. clear();
		// no break;
	case stCompleted:
		m_ -> status = stSteady;
		// no break;
	case stSteady:
		if (m_ -> tracer)
		{
			m_ -> tracer -> rewind();
		}

		if (m_ -> traveller)
		{
			m_ -> traveller -> reset();
		}

		break;
	}
}

Variable Parser::get (const char* a_variable) const
{
	if (! m_ -> validate(stCompleted))
		return Variable(NULL);

	typedef anta::ndl::context_key<NLG>::type key_type;
	typedef anta::ndl::context_value<NLG>::type value_type;

	key_type key;
	value_type value;
	try
	{
		key_type(a_variable). swap(key);
		value_type((*(m_ -> tracer)) -> val(key)). swap(value);
	}
	catch (const std::exception& err)
	{
	//	m_ -> report(err);
		return Variable(NULL);
	}

	// (no throwing beyond this point)
	VariableData* data = new VariableData;
	data -> entry. first. swap(key);
	data -> entry. second. swap(value);
	return Variable(data);
}

int Parser::label () const
{
	if (! m_ -> validate(stCompleted))
		return 0;

	try
	{
		return (*(m_ -> tracer)) -> get_arc(). get_label(). get();
	}
	catch (const std::exception& err)
	{
	//	m_ -> report(err);
		return 0;
	}
}

char* Parser::node (char* a_buf, const int a_len) const
{
	if (! m_ -> validate(stCompleted) || a_len <= 0)
		return a_buf;

	try
	{
		const anta::ndl::node_name<NLG>::type& name =
			(*(m_ -> tracer)) -> get_arc(). get_target(). get_name();
		const std::string temp = encode::string(name);
		const int len = std::min<int>(a_len - 1, temp. size());
		memcpy(a_buf, temp. data(), len);
		a_buf[len] = 0;
	}
	catch (const std::exception& err)
	{
	//	m_ -> report(err);
		a_buf[0] = 0;
	}

	return a_buf;
}

char* Parser::text (char* a_buf, const int a_len) const
{
	if (! m_ -> validate(stCompleted) || a_len <= 0)
		return a_buf;

	try
	{
		const anta::range<NLG>::type& range = (*(m_ -> tracer)) -> get_range();
		const std::string temp = encode::string(range. first, range. second);
		const int len = std::min<int>(a_len - 1, temp. size());
		memcpy(a_buf, temp. data(), len);
		a_buf[len] = 0;
	}
	catch (const std::exception& err)
	{
	//	m_ -> report(err);
		a_buf[0] = 0;
	}

	return a_buf;
}

int Parser::shift () const
{
	return m_ -> validate(stCompleted) ? m_ -> shift : 0;
}

void Parser::set (const char* a_variable, const bool a_value)
{
	m_ -> config[ a_variable ] = a_value;
}

void Parser::set (const char* a_variable, const int a_value)
{
	m_ -> config[ a_variable ] = a_value;
}

void Parser::set (const char* a_variable, const double a_value)
{
	m_ -> config[ a_variable ] = a_value;
}

void Parser::set (const char* a_variable, const char* a_value)
{
	m_ -> config[ a_variable ] = a_value;
}

void Parser::clear ()
{
	m_ -> init. clear();
}

int Parser::get_message_count () const
{
	return static_cast<int>(m_ -> messages. size());
}

const char* Parser::get_message (const int a_index) const
{
	try
	{
		return boost::get<0>(m_ -> messages. at(a_index)). c_str();
	}
	catch (const std::exception& err)
	{
	//	m_ -> report(err);
		return "";
	}
}

const char* Parser::get_location (const int a_index, int* a_line, int* a_offset)
	const
{
	try
	{
		const ParserData::message_t& msg = m_ -> messages. at(a_index);
		if (a_line) *a_line = boost::get<2>(msg);
		if (a_offset) *a_offset = boost::get<3>(msg);
		return boost::get<1>(msg). c_str();
	}
	catch (const std::exception& err)
	{
	//	m_ -> report(err);
		return "";
	}
}

unsigned long Parser::get_iteration_count () const
{
	return m_ -> validate(stCompleted)
		? static_cast<unsigned long>(m_ -> iteration_count)
		: 0;
}

unsigned long Parser::get_trace_count () const
{
	return m_ -> validate(stCompleted)
		? static_cast<unsigned long>(m_ -> traveller -> get_traced(). size())
		: 0;
}

unsigned long Parser::get_pool_usage () const
{
	return m_ -> validate(stCompleted)
		? static_cast<unsigned long>(m_ -> traveller -> get_usage())
		: 0;
}

unsigned long Parser::get_pool_capacity () const
{
	return m_ -> validate(stCompleted)
		? static_cast<unsigned long>(m_ -> traveller -> get_capacity())
		: 0;
}

} // namespace nparse
