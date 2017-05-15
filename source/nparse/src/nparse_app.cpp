/*
 * @file $/source/nparse/src/nparse_app.cpp
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
#include <iostream>
#include <locale>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <plugin/static.hpp>
#include <util/free.hpp>
#include <nparse/_version.hpp>
#include "nparse_app.hpp"
#include "tracer_nlg.hpp"
#include "serializer.hpp"

PLUGIN_STATIC_IMPORT(nparse_script_grammar)
PLUGIN_STATIC_IMPORT(nparse_acceptor_factory)
PLUGIN_STATIC_IMPORT(nparse_runtime)

static const bool force_utf8 = true;

using namespace nparse;
namespace fs = boost::filesystem;
namespace ch = boost::chrono;

typedef anta::Processor<SG>::traced_type traced_t;

typedef ch::high_resolution_clock::time_point timepoint_t;
typedef ch::duration<double> dt_t;

/**
 *	(nparse/std exception adapter)
 */
class nparse_error: public std::runtime_error
{
	static std::string render (const ex::generic_error& a_error)
	{
		std::stringstream info;
		const std::string* str;
		str = boost::get_error_info<ex::function>(a_error);
		if (str) info << *str << ": ";
		str = boost::get_error_info<ex::message>(a_error);
		if (str) info << *str;
		str = boost::get_error_info<ex::file>(a_error);
		if (str)
		{
			const int* val;
			info << "\n\tin " << *str;
			val = boost::get_error_info<ex::line>(a_error);
			if (val) info << " on line " << *val;
			val = boost::get_error_info<ex::offset>(a_error);
			if (val) info << ", offset " << *val;
		}
		return info. str();
	}

public:
	nparse_error (const ex::generic_error& a_error):
		std::runtime_error (render(a_error))
	{
	}

};

/**
 *	Throws a syntax error exception.
 */
void rethrow (const anta::observer<SG>::type& a_observer,
		const IStaging& a_staging)
{
	std::vector<deadlock_t> dls;
	a_observer. analyze(dls);

	ex::syntax_error error;
	error << ex::message("syntax error");

	std::string file;
	int line, offset;
	for (std::vector<deadlock_t>::const_iterator i = dls. begin();
			i != dls. end(); ++ i)
	{
		if (a_staging. identify(dls. front(). first, file, line, offset))
		{
			error << ex::file(file) << ex::line(line) << ex::offset(offset);
			// @todo: implement multiple source support
			break;
		}
	}

	throw nparse_error(error);
}

/**
 *	Throws a syntax ambiguity exception.
 */
void rethrow (const traced_t& a_traced, const IStaging& a_staging)
{
	typedef const anta::State<SG>* state_t;
	typedef std::set<state_t> states_t;
	typedef std::set<anta::iterator<SG>::type> ambiguous_t;

	ambiguous_t ambiguous;
	states_t cache;
	for (traced_t::const_iterator t = a_traced. begin(); t != a_traced. end();
			++ t)
	{
		state_t s = *t;
		while (s != NULL && cache. insert(s). second)
		{
			s = s -> get_ancestor();
		}
		if (s != NULL)
		{
			ambiguous. insert(s -> get_range(). second);
		}
	}

	ex::syntax_error error;
	error << ex::message("syntax ambiguity");

	std::string file;
	int line, offset;
	for (ambiguous_t::const_iterator p = ambiguous. begin();
			p != ambiguous. end(); ++ p)
	{
		if (a_staging. identify(*p, file, line, offset))
		{
			error << ex::file(file) << ex::line(line) << ex::offset(offset);
			// @todo: implement multiple source support
			break;
		}
	}

	throw nparse_error(error);
}

/**
 *	Throws a generic error exception.
 */
void rethrow (ex::generic_error& a_error, const IStaging& a_staging)
{
	a_staging. extend(a_error);
	throw nparse_error(a_error);
}

nParseApp::nParseApp ():
	m_grammar_pool (0),
	m_input_stream (NULL),
	m_input_pool (0),
	m_input_batch (false),
	m_entry_label (1),
	m_staging_factory ("nparse.script.StagingFactory")
{
	PLUGIN_STATIC_INIT(nparse_script_grammar);
	PLUGIN_STATIC_INIT(nparse_acceptor_factory);
	PLUGIN_STATIC_INIT(nparse_runtime);
}

nParseApp::~nParseApp ()
{
	utility::free2nds(m_files);
}

bool nParseApp::configure (const int argc, char** argv)
{
	namespace po = boost::program_options;

	// Define the list of acceptable program options.
	po::options_description desc("Generic options");
	desc. add_options()
		("help,h",				"Print this help message and exit")
		("version,v",			"Print version infomation")
		("config,c",			po::value<std::string>(),
								"Load configuration from [file]")
		("locale,L",			po::value<std::string>()
									-> default_value(""),
								"Set internal [locale]")
		("grammar-file,g",		po::value<std::string>()
									-> default_value("grammar.ng"),
								"Load grammar from [file]")
		("grammar-pool,G",		po::value<long>()
									-> default_value(16 << 10),
								"Set grammar pool size [Kb]")
		("input-text,i",		po::value<std::string>(),
								"Read input from command line")
		("input-file,f",		po::value<std::string>(),
								"Read input from [file]")
		("input-pool,I",		po::value<long>()
									-> default_value(1 << 10),
								"Set input pool size [Kb]")
#if defined(NPARSE_SWAP_FILE)
		("input-swap,w",		po::value<std::string>()
									-> default_value(""),
								"Store parser pool data in local [file]")
#endif
		("input-batch,b",		po::value<bool>()
									-> default_value(true),
								"Treat input as series of independent entries"
								" separated by new line character")
		("entry-point,e",		po::value<std::string>()
									-> default_value("S"),
								"Set grammar entry point [name]")
		("entry-label,E",		po::value<int>()
									-> default_value(1),
								"Set entry arc [label]")
		("lr-threshold,r",		po::value<long>()
									-> default_value(64),
								"Set or disable LR [threshold]")
#if defined(DEBUG_PRINT)
		("debug-print,d",		po::value<std::string>()
									-> default_value("")
									-> implicit_value("1"),
								"Print debug log to [stream]")
#endif
		("trace-print,t",		po::value<std::string>()
									-> default_value("")
									-> implicit_value("1"),
								"Print parser traces to [stream]")
		("trace-format,T",		po::value<std::string>()
									-> default_value(
								"%i\\t%j\\t%label\\t%node\\t%text\\t%list\\n"),
								"Set trace format using placeholders:\n"
								"  \\t     tab\n"
								"  \\n     line break\n"
								"  %i     trace number\n"
								"  %j     state number\n"
								"  %label transition label\n"
								"  %type  transition type\n"
								"  %node  target node name\n"
								"  %text  accepted text\n"
								"  %list  trace variable list\n"
								"  %:var  trace variable `var` value"
								)
		("state-print,s",		po::value<std::string>()
									-> default_value("")
									-> implicit_value("1"),
								"Print final trace variables to [stream]")
		("out,o",				po::value<std::vector<std::string> >()
									-> multitoken(),
								"Print specific trace variable value to STDOUT"
								" as-is (without formatting)")
		("tree,x",				po::value<std::string>()
									-> default_value("")
									-> implicit_value("1"),
								"Print syntax trees to [stream]")
		("log,l",				po::value<std::string>()
									-> default_value("")
									-> implicit_value("2"),
								"Print execution log to [stream]\n\n"
								"* An output stream can be specified as\n"
							//	"  0 or STDIN   input stream\n"
								"  1 or STDOUT  output stream\n"
								"  2 or STDERR  error stream\n"
							//	"  3 or STDLOG  logging stream\n"
								"  <path>       local file"
								);

	// Define positional program options.
	po::positional_options_description desc_p;
	desc_p
		. add("grammar-file", 1)
		. add("input-text", 1)
		. add("entry-point", 1)
		. add("input-pool", 1)
		. add("grammar-pool", 1);

	// Parse command line options.
	po::parsed_options parsed = po::command_line_parser(argc, argv)
		. options(desc)
		. positional(desc_p)
		. allow_unregistered()
		. run();

	// Store the parsed options in a variable map.
	po::variables_map vm;
	po::store(parsed, vm, force_utf8);
	po::notify(vm);

	// A flag indicating that a piece of supplementary information is requested.
	bool supplementary = false;

	// Set up internal locale.
	{
		const std::string& lname = vm["locale"]. as<std::string>();
		try
		{
			std::locale c_loc("C");
			if (lname. empty() || lname == "C")
			{
				std::locale::global(c_loc);
			}
			else
			{
				std::locale g_loc(lname. c_str());
				std::locale::global(c_loc. combine<std::ctype<char> >(g_loc));
			}
		}
		catch (const std::exception& err)
		{
			std::cerr << "warning: could not set locale `" << lname << "': "
				<< err. what() << std::endl;
			if	(	!setlocale(LC_ALL, "C")
				||	!setlocale(LC_CTYPE, lname. c_str())
				)
			{
				std::cerr << "warning: setlocale() also failed" << std::endl;
			}
		}
	}

	// Check if the version number is requested.
	if (vm. count("version"))
	{
		std::cerr << NPARSE_VERSION_STR "\n";
#if defined(DEBUG_PRINT)
#define	OBJ_SIZE(C) << sizeof(anta::C) << "\t" #C "\n"
		if (! vm["debug-print"]. as<std::string>(). empty())
		{
			std::cerr
			OBJ_SIZE(StateCommon<NLG>)
			OBJ_SIZE(StateSplit<NLG>)
			OBJ_SIZE(StateSplitShifted<NLG>)
			OBJ_SIZE(StateSplitExtended<NLG>)
			OBJ_SIZE(ndl::Context<NLG>)
			OBJ_SIZE(ndl::context<NLG>::type)
			OBJ_SIZE(aux::Variable<NLG>)
			OBJ_SIZE(ndl::Node<NLG>)
			OBJ_SIZE(Arc<NLG>)
			OBJ_SIZE(Label<NLG>);
		}
#undef	OBJ_SIZE
#endif
		supplementary = true;
	}

	// Check if the help page is requested.
	if (vm. count("help"))
	{
		std::cerr << desc << '\n';
		supplementary = true;
	}

	// If any supplementary information was requested then just exist without
	// doing anything.
	if (supplementary)
		return false;

	// Interpret unrecognized command line options as initial values for
	// the trace variables.
	{
		static const boost::regex sc_init(
			"\\A--(init\\.)?([-_\\w]+)(=(.+))?\\s*$");
		boost::smatch matches;

		const std::vector<std::string> other = po::collect_unrecognized(
				parsed. options, po::exclude_positional);

		for (std::vector<std::string>::const_iterator i = other. begin();
				i != other. end(); ++ i)
		{
			if (! boost::regex_match(*i, matches, sc_init))
			{
				throw std::runtime_error("unrecongized command line option");
			}
			m_init[matches[2]. str()] = matches[4]. str();
		}
	}

	// Check if the a config file is speficied.
	if (vm. count("config") != 0)
	{
		// Try to open the config file for reading.
		std::ifstream config_file(vm["config"]. as<std::string>(). c_str());
		if (! config_file)
		{
			throw std::runtime_error("unable to read config file");
		}

		// Read configuration options.
		parsed = po::parse_config_file(config_file, desc, true);
		po::store(parsed, vm, force_utf8);
		po::notify(vm);
		config_file. close();

		// Interpret unrecognized configuration options as initial values for
		// the trace variables.
		{
			static const boost::regex sc_init(
				"\\A(init\\.)?([-_\\w]+)\\s*$");
			boost::smatch matches;

			const std::vector<std::string> other = po::collect_unrecognized(
					parsed. options, po::exclude_positional);

			for (std::vector<std::string>::const_iterator i = other. begin();
					i != other. end(); ++ i)
			{
				if	(	(i - other. begin()) & 1
					&&	boost::regex_match(*(i - 1), matches, sc_init)
					)
				{
					const std::string& key = matches[2]. str();
					if (m_init. find(key) == m_init. end())
						m_init[key] = *i;
				}
			}
		}
	}

	// Link the input text source.
	if (vm. count("input-text"))
	{
		if (vm. count("input-file"))
		{
			throw std::runtime_error("input-text and input-file program options"
					" can not be specified at the same time");
		}
		m_input_text << vm["input-text"]. as<std::string>();
		m_input_stream =& m_input_text;
	}
	else
	if (vm. count("input-file"))
	{
		m_input_file. open(vm["input-file"]. as<std::string>(). c_str());
		if (! m_input_file)
		{
			throw std::runtime_error("unable to read input file");
		}
		m_input_stream =& m_input_file;
	}
	else
	{
		m_input_stream =& std::cin;
	}

	// Get other application settings.
	m_grammar_file = vm["grammar-file"]. as<std::string>();
	m_grammar_pool = vm["grammar-pool"]. as<long>() << 10;
	m_input_pool = vm["input-pool"]. as<long>() << 10;
	m_input_batch = vm["input-batch"]. as<bool>();
#if defined(NPARSE_SWAP_FILE)
	m_input_swap = vm["input-swap"]. as<std::string>();
#endif
	m_entry_point = vm["entry-point"]. as<std::string>();
	m_entry_label = vm["entry-label"]. as<int>();
	m_lr_threshold = vm["lr-threshold"]. as<long>();
#if defined(DEBUG_PRINT)
	m_debug_print = vm["debug-print"]. as<std::string>();
#endif
	m_trace_print = vm["trace-print"]. as<std::string>();
	m_trace_format = vm["trace-format"]. as<std::string>();
	m_state_print = vm["state-print"]. as<std::string>();
	m_syntax_tree = vm["tree"]. as<std::string>();
	m_log = vm["log"]. as<std::string>();

	if (vm. count("out"))
	{
		m_out = vm["out"]. as<std::vector<std::string> >();
	}

	return true;
}

int nParseApp::execute ()
{
	compile_grammar();
	parse_input_text();
	return 0;
}

std::ostream* nParseApp::open_file (const std::string& a_name)
{
	if (a_name == "1" || a_name == "STDOUT")
	{
		return &std::cout;
	}
	else if (a_name == "2" || a_name == "STDERR")
	{
		return &std::cerr;
	}
//	else if (a_name == "3" || a_name == "STDLOG")
//	{
//		return &std::clog;
//	}
	else
	{
		const fs::path path(a_name);
		files_t::iterator found_at = m_files. find(path);
		if (found_at == m_files. end())
		{
			const std::pair<files_t::iterator, bool> p = m_files. insert(
				files_t::value_type(path, new std::ofstream(path. c_str())));
			assert(p. second);
			found_at = p. first;
		}
		return found_at -> second;
	}
}

int nParseApp::compile_grammar ()
{
	// Instantiate script grammar object.
	plugin::instance<IConstruct> grammar("nparse.script.Grammar");

	// Create processor and link it to the script grammar's entry point.
	anta::Processor<SG> processor(grammar -> entry());
	processor. set_capacity(m_grammar_pool);
	//processor. get_observer(). set_max_trace_count(16);
	//processor. get_observer(). set_max_trace_depth(3);

	// Create tracer and link it to the processor.
	anta::aux::Tracer<SG> tracer(processor);

	// Instantiate staging object and set the first source file for import.
	m_staging = m_staging_factory -> createInstance();
	m_staging -> import(m_grammar_file, true);

	// Load and parse each source file.
	try
	{
		const timepoint_t t0 = ch::high_resolution_clock::now();

		bool first = true;
		anta::range<SG>::type src;
		while (m_staging -> load(src))
		{
			// Launch source file parsing.
			processor. run(src. first, src. second);

			// Count traces.
			int traces_count = 0;
			while (tracer. next())
			{
				++ traces_count;
			}
			tracer. rewind();

			// Report syntax error/ambiguity errors.
			switch (traces_count)
			{
			case 1:
				break;

			case 0:
				// syntax error
				rethrow(processor. get_observer(), *m_staging);
				break;

			default:
				// syntax ambiguity
				rethrow(processor. get_traced(), *m_staging);
				break;
			}

			// Execture source code (generate the acceptor network).
			tracer. next();
			while (tracer. step())
			{
				tracer -> get_arc(). get_label(). execute(*m_staging, *tracer);
			}

			// Log successul source loading.
			if (! m_log. empty())
			{
				std::string file;
				int line, offset;
				m_staging -> identify(src. first, file, line, offset);

				const timepoint_t t1 = ch::high_resolution_clock::now();
				std::ostream& log = *open_file(m_log);
				log << std::fixed
					<< std::setprecision(2)
					<< "loaded: " << file << " ("
					<< 1e2 * processor. get_usage() / processor. get_capacity();
				ch::duration_short(log)
					<< std::setprecision(4)
					<< "%, " << ch::duration_cast<dt_t>(t1 - t0) << ")\n";
			}

			// Reset intermediate objects.
			tracer. rewind();
			processor. reset();

			// Get and save the first namespace (it happens to be the last
			// namespace declared in the first imported script file).
			if (first)
			{
				const string_t& namespace_ = m_staging -> getNamespace();
				if (! namespace_. empty() &&
						m_entry_point. find('.') == m_entry_point. npos)
				{
					m_entry_point = encode::string(namespace_) + m_entry_point;
				}
				first = false;
			}

			// Reset namespace.
			m_staging -> setNamespace();
		}

		if (! m_log. empty())
		{
			const timepoint_t t1 = ch::high_resolution_clock::now();
			std::ostream& log = *open_file(m_log);
			ch::duration_short(log)
				<< std::setprecision(4)
				<< "build time: "
				<< ch::duration_cast<dt_t>(t1 - t0) << '\n';
		}
	}
	catch (const std::bad_alloc&)
	{
		throw std::runtime_error("grammar pool overflow");
	}
	catch (ex::generic_error& err)
	{
		rethrow(err, *m_staging);
	}

	return 0;
}

int nParseApp::parse_input_text ()
{
	// Create processor and link it to the compiled grammar's entry point.
	anta::Processor<NLG> processor(m_staging -> cluster(m_entry_point),
			m_entry_label);
#if defined(DEBUG_PRINT)
	if (! m_debug_print. empty())
	{
		processor. get_observer(). link(open_file(m_debug_print));
	}
#endif
#if defined(NPARSE_SWAP_FILE)
	if (! m_input_swap. empty())
	{
		processor. set_swap_file(m_input_swap);
	}
#endif
	processor. set_capacity(m_input_pool);
	processor. set_lr_threshold(m_lr_threshold);

	// Create tracer and link it to the processor.
	TracerNLG tracer(processor);
	tracer. format(m_trace_format);

	std::string buf;
	while (std::getline(*m_input_stream, buf, m_input_batch ? '\n' : '\x00'))
	{
		// Implicit narrow/wide string conversion may take place here.
		const nlg_string_t& line = buf;

		// Prepare input text range.
		anta::range<NLG>::type src;
		src. first = &* line. begin();
		src. second = src. first + line. size();

		// Initialize the processor.
		processor. init(src. first, src. second);

		// Assign initial values to trace variables.
		for (init_t::const_iterator i = m_init. begin(); i != m_init. end();
				++ i)
		{
			processor. ref(i -> first) = i -> second;
		}

		// Parse input text.
		dt_t parse_time = ch::seconds(0);
		anta::uint_t total_iteration_count = 0;
		try
		{
			const timepoint_t t0 = ch::high_resolution_clock::now();
			total_iteration_count = processor. run();
			parse_time = ch::high_resolution_clock::now() - t0;
		}
		catch (const std::bad_alloc&)
		{
			throw std::runtime_error("input pool overflow");
		}
		catch (ex::generic_error& err)
		{
			rethrow(err, *m_staging);
		}

		// Unroll and print traces.
		print_traces(tracer);

		// Print syntax tree.
		print_syntax_tree(tracer);

		// Print final states.
		print_final_states(processor);

		// Print explicitly specified variables.
		print_variables(processor);

		// Print statistics.
		print_stats(processor, parse_time, total_iteration_count);

		// Reset intermediate objects (not really necessary here).
		tracer. rewind();
		processor. reset();
	}

	return 0;
}

void nParseApp::print_traces (TracerNLG& a_trac)
{
	if (m_trace_print. empty())
	{
		return;
	}

	std::ostream& out = *open_file(m_trace_print);

	while (a_trac. next())
	{
		while (a_trac. step())
		{
			if (a_trac. type() == 1)
			{
				out << a_trac. print();
			}
		}
	}
}

void nParseApp::print_final_states (const anta::Processor<NLG>& a_proc)
{
	typedef anta::Processor<NLG>::traced_type traced_t;
	const traced_t& traced = a_proc. get_traced();

	if (m_state_print. empty() || traced. empty())
	{
		return;
	}

	Serializer<NLG> serializer;
	serializer << YAML::BeginSeq;
	for (traced_t::const_iterator s = traced. begin(); s != traced. end(); ++ s)
	{
		serializer << *((*s) -> context(NULL));
	}
	serializer << YAML::EndSeq;

	*open_file(m_state_print) << serializer. c_str() << '\n';
}

void nParseApp::print_variables (const anta::Processor<NLG>& a_proc)
{
	typedef anta::Processor<NLG>::traced_type traced_t;
	const traced_t& traced = a_proc. get_traced();

	if (m_out. empty() || traced. empty())
	{
		return;
	}

	std::ostream& out = *open_file("STDOUT");

	for (traced_t::const_iterator s = traced. begin(); s != traced. end(); ++ s)
	{
		for (std::vector<std::string>::const_iterator
				v = m_out. begin(); v != m_out. end(); ++ v)
		{
			const IEnvironment::key_type key =
				encode::string(*v);
			out << encode::string((*s) -> val(key). as_string())
				<< '\n';
		}
	}
}

void nParseApp::print_syntax_tree (TracerNLG& a_trac)
{
	if (m_syntax_tree. empty())
	{
		return;
	}

	std::ostream& out = *open_file(m_syntax_tree);

	while (a_trac. next())
	{
		bool first = true;

		while (a_trac. step()) switch (a_trac. type())
		{
		// Regular element.
		case 1:
			{	// Determine and print the name of the target node.
				const anta::ndl::node_name<NLG>::type& name =
					a_trac -> get_arc(). get_target(). get_name();
				if (! name. empty())
				{
					if (first)
					{
						first = false;
					}
					else
					{
						out << ' ';
					}
					out << '(' << name;
				}

				// Determine and print the accepted range.
				const anta::range<NLG>::type range =
					a_trac -> get_range();
				const nlg_string_t text(
					range. first, range. second);
				if (! text. empty())
				{
					out << ' ' << text;
				}
			}
			break;

		// Close parenthesis.
		case 2:
			{
				const anta::State<NLG>* s = a_trac. relative("<:");
				// Determine if the parenthesis refers to a named rule.
				if (s && ! s -> get_arc(). get_target(). get_name(). empty())
				{
					out << ')';
				}
			}
			break;
		}

		// Final parenthesis.
		out << ")\n";
	}
}

void nParseApp::print_stats (const anta::Processor<NLG>& a_proc,
		const dt_t& a_parse_time, const anta::uint_t a_total_iteration_count)
{
	if (m_log. empty())
	{
		return;
	}

	std::ostream& log = *open_file(m_log);

	ch::duration_short(log)
		<< std::setprecision(4)
		<< "parse time: "
		<< a_parse_time
		<< std::setprecision(2)
		<< "\n"
		   "trace count: "
		<< a_proc. get_traced(). size()
		<< "\n"
		   "iteration count: "
		<< a_total_iteration_count
		<< "\n"
		   "context count: "
		<< a_proc. context_count()
		<< "\n"
		   "state pool usage at exit: "
		<< a_proc. get_usage() << " bytes ("
		<< 1e2 * a_proc. get_usage() / a_proc. get_capacity()
		<< "%)\n"
		   "state pool usage at peak: "
		<< a_proc. get_peak_usage() << " bytes ("
		<< 1e2 * a_proc. get_peak_usage() / a_proc. get_capacity()
		<< "%)\n"
		   "total evicted size: "
		<< a_proc. get_evicted_size() << " bytes ("
		<< 1e2 * a_proc. get_evicted_size() / a_proc. get_capacity()
		<< "%)\n";
}
