/*
 * @file $/source/nparse-test/src/test_script.cpp
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
#include <boost/algorithm/string.hpp>
#include <encode/encode.hpp>
#include <nparse-port/parser.hpp>
#include "test_script.hpp"
#include "utils/find_files.hpp"

static const char* TAG_SCRIPT = "script";
static const char* TAG_CONFIG = "config";
static const char* TAG_INPUT = "input";
static const char* TAG_CHECK = "check";
static const char* TAG_ERROR = "error";
static const char* TAG_FOREST = "forest";

static char* static_trim (char* str)
{
	char* beg = str;
	char* end = NULL;

	while (*beg == ' ')
		++ beg;

	str = beg;

	while (*str)
	{
		if (*str != ' ')
			end = NULL;
		else
		if (!end)
			end = str;
		++ str;
	}

	if (end)
		*end = 0;

	return beg;
}

std::string enumerate (const std::vector<std::string>& a_lines,
		const char* a_prefix, const char* a_postfix = "\n")
{
	std::string result;
	for (std::vector<std::string>::const_iterator i = a_lines. begin();
			i != a_lines. end(); ++ i)
	{
		result. append(a_prefix);
		result. append(*i);
		result. append(a_postfix);
	}
	return result;
}

YAML::NodeType::value get_node_type(const YAML::Node& a_node)
{
	// YAML::Node::Type() throws InvalidNode exception for undefined nodes
	// instead of returning the Undefined node type in yaml-cpp-0.5.3. Dumb.
	return a_node. IsDefined() ? a_node. Type() : YAML::NodeType::Undefined;
}

/**
 *	Apply a single setting to the Parser object.
 */
void configure_parser (nparse::Parser& a_parser,
		const YAML::Node::const_iterator& a_setting)
{
	const std::string& key = a_setting -> first. as<std::string>();
	const std::string& val = a_setting -> second. as<std::string>();
	a_parser. set(key. c_str(), val. c_str());
	if (key == "input_swap")
	{
		ASSERT_TRUE( fs::exists(val) ) << "missing preallocated swap file `"
			<< val << '\'';
	}
}

/**
 *	Apply a series of settings to the Parser object.
 */
void configure_parser (nparse::Parser& a_parser, const YAML::Node& a_common,
		const YAML::Node& a_case)
{
	YAML::Node::const_iterator i;
	if (a_case. IsDefined())
	{
		for (i = a_common. begin(); i != a_common. end(); ++ i)
		{
			if (a_case[i -> first]. IsDefined())
				continue;
			configure_parser(a_parser, i);
		}
		for (i = a_case. begin(); i != a_case. end(); ++ i)
		{
			configure_parser(a_parser, i);
		}
	}
	else
	{
		for (i = a_common. begin(); i != a_common. end(); ++ i)
		{
			configure_parser(a_parser, i);
		}
	}
}

/**
 *	Request a variable from the Parser object.
 */
nparse::Variable request (const nparse::Parser& a_parser,
		const std::string& a_query)
{
	typedef boost::split_iterator<std::string::const_iterator> iterator;
	bool first = true;
	nparse::Variable current;
	for (iterator i(a_query. begin(), a_query. end(), boost::first_finder("."));
			! i. eof(); ++ i)
	{
		const std::string key(i -> begin(), i -> end());
		if (first)
		{
			first = false;
			current = a_parser. get(key. c_str());
		}
		else
		{
			current =* current. get(key. c_str());
		}
	}
	return current;
}

/**
 *	Validate an error report generated by the Parser object.
 */
void validate_error (const char* a_script, nparse::Parser& a_parser,
		const YAML::Node& a_error)
{
	ASSERT_EQ( a_error. size(), a_parser. get_message_count() );
	YAML::Node::const_iterator err_it = a_error. begin();
	for (int i = 0; i < a_parser. get_message_count(); ++ i)
	{
		int line = 0, offset = 0;
		const std::string& location = a_parser. get_location(i, &line, &offset);
		EXPECT_TRUE(boost::ends_with(location, a_script))
			<< "location: " << location << '\n'
			<< "a_script: " << a_script;
		switch (std::min<int>(3, err_it -> size()))
		{
		case 3: EXPECT_EQ( (*err_it)[2]. as<int>(), offset );
		case 2: EXPECT_EQ( (*err_it)[1]. as<int>(), line );
		case 1: EXPECT_STREQ(
					(*err_it)[0]. as<std::string>(). c_str(),
					a_parser. get_message(i)
				);
				break;
		case 0: EXPECT_STREQ(
					err_it -> as<std::string>(). c_str(),
					a_parser. get_message(i)
				);
				break;
		}
		++ err_it;
	}
}

/**
 *	Compare a single result from the Parser object with the given value provided
 *	in the test case.
 */
void validate_result (const nparse::Variable& a_result,
		const YAML::Node& a_expected, const char* a_info)
{
	switch (a_result. type())
	{
	case nparse::Variable::Boolean:
		EXPECT_NO_THROW({
			const bool boolean_value = a_expected. as<bool>();
			EXPECT_EQ( boolean_value, a_result. as_boolean() ) << a_info;
		}) << a_info;
		break;

	case nparse::Variable::Integer:
		EXPECT_NO_THROW({
			const int integer_value = a_expected. as<int>();
			EXPECT_EQ( integer_value, a_result. as_integer() ) << a_info;
		}) << a_info;
		break;

	case nparse::Variable::Real:
		EXPECT_NO_THROW({
			const double real_value = a_expected. as<double>();
			EXPECT_NEAR( real_value, a_result. as_real(), 1e-6 ) << a_info;
		}) << a_info;
		break;

	case nparse::Variable::Array: // highly experimental
	case nparse::Variable::String:
		EXPECT_NO_THROW({
			char buf[1024];
			const std::string& string_value = a_expected. as<std::string>();
			EXPECT_STREQ(
				string_value. c_str(),
				a_result. as_string(buf, sizeof(buf)) ) << a_info;
		}) << a_info;
		break;

	case nparse::Variable::Null:
	default:
		EXPECT_TRUE( a_expected. IsNull() ) << a_info;
		break;
	}
}

/**
 *	Validate internal parser state.
 */
void validate_state (const char* a_script, const char* a_case,
		const nparse::Parser& a_parser, const YAML::Node& a_test)
{
	static const char* sc_params[] = {
		"traces", "iterations", "contexts", "usage", NULL };

	std::stringstream info;

	for (int i = 0; sc_params[i]; ++ i)
	{
		const YAML::Node& expected = a_test[sc_params[i]];
		switch (get_node_type(expected))
		{
		case YAML::NodeType::Scalar:

			info<< "  Source: " << a_script << " @ " << a_case << '\n'
				<< "   Query: " << sc_params[i];

			// @todo: implement comparison modifiers
			switch (i) {
			case 0:
				EXPECT_EQ(
					expected. as<unsigned long>(),
					a_parser. get_trace_count() ) << info. str();
				break;
			case 1:
				EXPECT_EQ(
					expected. as<unsigned long>(),
					a_parser. get_iteration_count() ) << info. str();
				break;
			case 2:
				EXPECT_EQ(
					expected. as<unsigned long>(),
					a_parser. get_context_count() ) << info. str();
				break;
			case 3:
				EXPECT_NEAR(
					expected. as<double>(),
					100.0 * a_parser. get_pool_usage()
					      / a_parser. get_pool_capacity(),
					0.5
				) << info. str();
				break;
			}

			info. str("");
			info. clear();
			break;

		case YAML::NodeType::Undefined:
		case YAML::NodeType::Null:
			break;

		default:
			ADD_FAILURE() << "node `" << a_script << " @ " << a_case << '/'
				<< sc_params[i] << "' must be a scalar whenever specified";
			break;
		}
	}
}

/**
 *	Validate abstract sytax trees.
 */
void validate_asts (const char* a_script, const char* a_case,
		nparse::Parser& a_parser, const YAML::Node& a_test)
{
	char buf[NPARSE_STRBUF_SIZE];

	const YAML::Node& forest = a_test[TAG_FOREST];
	switch (get_node_type(forest))
	{
	case YAML::NodeType::Sequence:
		break;
	case YAML::NodeType::Map:
	case YAML::NodeType::Scalar:
		FAIL() << "node `" << a_script << " @ " << a_case << '/'
			<< TAG_FOREST << "' must be a sequence whenever specified";
		// no break;
	case YAML::NodeType::Undefined:
	case YAML::NodeType::Null:
	default:
		return;
	}

	std::string tree;
	std::vector<std::string> extra, missing;

	extra. reserve(forest. size());
	for (YAML::Node::const_iterator iter = forest. begin();
			iter != forest. end(); ++ iter)
	{
		extra. push_back(iter -> as<std::string>());
	}
	std::sort(extra. begin(), extra. end());

	a_parser. rewind();
	while (a_parser. next())
	{
		// build a tree corresponding to the current trace
		bool first = true;
		while (a_parser. step())
		{
			tree. append(a_parser. shift(), ')');
			if (0 !=* a_parser. node(buf, sizeof(buf)))
			{
				if (first) first = false;
				else tree. append(1, ' ');
				tree. append(1, '(');
				tree. append(buf);
			}
			if (0 !=* a_parser. text(buf, sizeof(buf)))
			{
				tree. append(1, ' ');
				tree. append(static_trim(buf));
			}
		}
		tree. append(a_parser. shift(), ')');

		// look for the thee in the expected list
		std::vector<std::string>::iterator found_at = std::lower_bound(
				extra. begin(), extra. end(), tree);
		if (found_at != extra. end() && *found_at == tree)
		{
			// if the tree has been found then remove it from the list
			extra. erase(found_at);
		}
		else
		{
			// otherwise, add the tree to the missing list
			missing. push_back(tree);
		}

		// cleanup
		tree. clear();
	}

	// Report unexpected observations.
	EXPECT_TRUE( extra. empty() )
		<< "  Source: " << a_script << " @ " << a_case << '\n'
		<< "  Reason: found " << extra. size() << " extra trace(s)\n"
		<< enumerate(extra, "   |----: ", "\n");

	// Report missing opservations.
	EXPECT_TRUE( missing. empty() )
		<< "  Source: " << a_script << " @ " << a_case << '\n'
		<< "  Reason: not found " << missing. size() << " trace(s)\n"
		<< enumerate(missing, "   |----: ", "\n");

	// Print difference.
	std::vector<std::string>::iterator
		i = extra. begin(),		i_end = extra. end(),
		j = missing. begin(),	j_end = missing. end();
	while (true)
	{
		int mode = -1;

		if (i != i_end)
		{
			if (j != j_end)
			{
				if (*i < *j)
				{
					mode = 0;
				}
				else
				{
					mode = 1;
				}
			}
			else
			{
				mode = 0;
			}
		}
		else
		{
			if (j != j_end)
			{
				mode = 1;
			}
			else
			{
				break;
			}
		}

		if (mode == 0)
		{
			std::cout << "--- " << *i << '\n';
			++ i;
		}
		else
		if (mode == 1)
		{
			std::cout << "+++ " << *j << '\n';
			++ j;
		}
	}
}

/**
 *	Perform checks associated with the given test case on the given script.
 */
void process_check (const char* a_script, const char* a_case,
		const char* a_input, nparse::Parser& a_parser,
		const YAML::Node& a_check)
{
	char buf[NPARSE_STRBUF_SIZE];
	std::stringstream info;

	// Step into first trace.
	a_parser.rewind();
	ASSERT_TRUE( a_parser. next() )
		<< "  Source: " << a_script << " @ " << a_case << '\n'
		<< "  Reason: expected at least one trace to analyze";

	// Run through expected values.
	for (YAML::Node::const_iterator check_it = a_check. begin();
			check_it != a_check. end(); ++ check_it)
	{
		const std::string query = check_it -> first. as<std::string>();
		const nparse::Variable& result = request(a_parser, query);

		info<< "  Source: " << a_script << " @ " << a_case << '\n'
			<< "   Input: " << a_input << '\n'
			<< "   Query: " << query << '\n'
			<< "  Target: " << check_it -> second << '\n'
			<< "  Result: " << result. tag() << " `"
				<< result. as_string(buf, sizeof(buf)) << '\'';

		validate_result(
			result,
			check_it -> second,
			info. str(). c_str()
		);

		info. str("");
		info. clear();
	}
}

/**
 *	Run a single test case on the given script.
 */
void process_case (const char* a_script, const char* a_case,
		nparse::Parser& a_parser, const YAML::Node& a_common,
		const YAML::Node& a_test)
{
	std::stringstream info;
	info << "  Source: " << a_script << " @ " << a_case;

	ASSERT_EQ( nparse::Parser::stSteady, a_parser. status() ) << info. str();

	// Configure parser/set initial trace variables.
	configure_parser(a_parser, a_common, a_test[TAG_CONFIG]);

	// Prepare input text range.
	const std::wstring input = encode::wstring(
			a_test[TAG_INPUT]. as<std::string>());

	// Parse input text.
	a_parser. parse(input. data(), input. size());

	// Validate runtime-error report.
	const YAML::Node& error = a_test[TAG_ERROR];
	if (error. IsDefined())
	{
		info << "\n  Reason: emulating a runtime error";
		ASSERT_NE( nparse::Parser::stCompleted, a_parser. status() )
			<< info. str();
		EXPECT_EQ( nparse::Parser::stRuntimeError, a_parser. status() )
			<< info. str();
		validate_error(a_script, a_parser, error);
		return;
	}

	info << "\n  Reason: emulating normal workflow";
	if (a_parser. status() != nparse::Parser::stCompleted)
	{
		for (int i = 0; i < a_parser. get_message_count(); ++ i)
		{
			int line = -1, offset = -1;
			info<< "\n Message (" << i << "): " << a_parser. get_message(i)
				<< "\nLocation (" << i << "): " << a_parser. get_location(
					i, &line, &offset) << " : " << line << ", offset "
						<< offset;
		}
		FAIL() << info. str();
	}

	// Validate parser state.
	validate_state(a_script, a_case, a_parser, a_test);

	// Validate abstract syntax trees.
	validate_asts(a_script, a_case, a_parser, a_test);

	// Run checks associated with the test case.
	const YAML::Node& check = a_test[TAG_CHECK];
	switch (get_node_type(check))
	{
	case YAML::NodeType::Map:
		process_check(
			a_script,
			a_case,
			a_test[TAG_INPUT]. as<std::string>(). c_str(),
			a_parser,
			check
		);
		break;

	case YAML::NodeType::Undefined:
	case YAML::NodeType::Null:
		break;

	default:
		ADD_FAILURE() << "node `" << a_script << " @ " << a_case << '/'
			<< TAG_CHECK << "' must be a map whenever specified";
		break;
	}
}

/**
 *	Run a series of test cases on the given script.
 */
void process_script (const char* a_script, const YAML::Node& a_config)
{
	std::stringstream info;
	info << "  Source: " << a_script;

	// Instantiate parser.
	nparse::Parser parser;
	ASSERT_EQ( nparse::Parser::stReady, parser. status() ) << info. str();

	// Load and compile grammar script.
	parser. load(a_script);

	// Validate syntax error report.
	const YAML::Node& error = a_config[TAG_ERROR];
	if (error. IsDefined())
	{
		info << "\n  Reason: emulating a syntax error";
		ASSERT_NE( nparse::Parser::stSteady, parser. status() ) << info. str();
		validate_error(a_script, parser, error);
		return;
	}

	info << "\n  Reason: emulating normal workflow";
	for (int i = 0; i < parser. get_message_count(); ++ i)
	{
		int line = -1, offset = -1;
		info<< "\n Message (" << i << "): " << parser. get_message(i)
			<< "\nLocation (" << i << "): " << parser. get_location(
				i, &line, &offset) << " : " << line << ", offset " << offset;
	}

	ASSERT_EQ( nparse::Parser::stSteady, parser. status() ) << info. str();

	// Run through all test cases.
	for (YAML::Node::const_iterator test_it = a_config. begin();
			test_it != a_config. end(); ++ test_it)
	{
		const std::string& case_ = test_it -> first. as<std::string>();
		if (case_ == TAG_SCRIPT || case_ == TAG_CONFIG)
			continue;

		process_case(
			a_script,
			case_. c_str(),
			parser,
			a_config[TAG_CONFIG],
			test_it -> second
		);

		// Clear initial trace variables and reset internal parser state.
		parser. clear();
		parser. reset();
	}
}

/*****************************************************************************/

void TestBundle::SetUp ()
{
	const std::string config_name = GetParam(). path(). string();

	m_config_data = YAML::LoadFile(config_name);
	ASSERT_EQ( YAML::NodeType::Map, get_node_type(m_config_data) );

	const YAML::Node& script = m_config_data[TAG_SCRIPT];
	switch (get_node_type(script))
	{
	case YAML::NodeType::Scalar:
		m_scripts. push_back(script. as<std::string>());
		break;

	case YAML::NodeType::Sequence:
		m_scripts. reserve(script. size());
		{
			int count = 0;
			for (YAML::Node::const_iterator script_it = script. begin();
					script_it != script. end(); ++ script_it)
			{
				ASSERT_EQ( YAML::NodeType::Scalar, script_it -> Type() )
					<< "node `" << config_name << ':' << TAG_SCRIPT << '['
					<< count++ << "]' does not represent a scalar value";
				m_scripts. push_back(script_it -> as<std::string>());
			}
		}
		break;

	default:
		FAIL() << "node `" << config_name << ':' << TAG_SCRIPT << "' must be"
			" either a scalar value or a list";
	}
}

void TestBundle::TearDown ()
{
	m_config_data. reset();
	m_scripts. clear();
}

TEST_P(TestBundle, execute)
{
	// Run through all listed scripts.
	for (scripts_t::const_iterator script_it = m_scripts. begin();
			script_it != m_scripts. end(); ++ script_it)
	{
		process_script(
			(GetParam(). path(). parent_path() / *script_it). c_str(),
			m_config_data
		);
	}
}

using namespace boost::xpressive;

INSTANTIATE_TEST_CASE_P(
	scripts,
	TestBundle,
	::testing::ValuesIn(find_files(
	/* where */ "./",
	/* what  */ bos >> +_ >> ".yml" >> eos // ^.+\.yml$
	))
);
