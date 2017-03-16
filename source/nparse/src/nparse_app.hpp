/*
 * @file $/source/nparse/src/nparse_app.hpp
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
#ifndef SRC_NPARSE_APP_HPP_
#define SRC_NPARSE_APP_HPP_

#include <string>
#include <ostream>
#include <sstream>
#include <fstream>
#include <map>
#include <boost/filesystem.hpp>
#include <plugin/plugin.hpp>
#include <nparse/nparse.hpp>

class TracerNLG;

class nParseApp
{
public:
	nParseApp ();
	~nParseApp ();

	bool configure (const int argc, char** argv);
	int execute ();

private:
	std::ostream* open_file (const std::string& a_name);
	int compile_grammar ();
	int parse_input_text ();

private:
	void print_traces (TracerNLG& a_trac);
	void print_final_states (const anta::Traveller<nparse::NLG>& a_trav);
	void print_variables (const anta::Traveller<nparse::NLG>& a_trav);
	void print_syntax_tree (TracerNLG& a_trac);
	void print_stats (const anta::Traveller<nparse::NLG>& a_trav,
			const anta::uint_t a_iteration_count);

	/**
	 *	Common application settings.
	 *	@{ */
	std::string m_grammar_file;
	long m_grammar_pool;
	std::stringstream m_input_text;
	std::ifstream m_input_file;
	std::istream* m_input_stream;
	long m_input_pool;
	bool m_input_batch;
#if defined(NPARSE_SWAP_FILE)
	std::string m_input_swap;
#endif
	std::string m_entry_point;
	int m_entry_label;
#if defined(DEBUG_PRINT)
	std::string m_debug_print;
#endif
	std::string m_trace_print;
	std::string m_trace_format;
	std::string m_state_print;
	std::string m_syntax_tree;
	std::string m_log;
	std::vector<std::string> m_out;
	/**	@} */

	/**
	 *	Initial trace variable values.
	 *	@{ */
	typedef std::map<std::string, std::string> init_t;
	init_t m_init;
	/**	@} */

	/**
	 *	List of currently open files.
	 *	@{ */
	// @todo: why not boost::ptr_map?
	typedef std::map<boost::filesystem::path, std::ostream*> files_t;
	files_t m_files;
	/**	@} */

	/**
	 *	The staging object, maintains all grammar network data.
	 */
	plugin::instance<nparse::IStagingFactory> m_staging_factory;
	boost::shared_ptr<nparse::IStaging> m_staging;

};

#endif /* SRC_NPARSE_APP_HPP_ */
