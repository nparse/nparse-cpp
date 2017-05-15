/*
 * @file $/source/nparse-test/src/test_script.hpp
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
#ifndef SRC_TEST_SCRIPT_HPP_
#define SRC_TEST_SCRIPT_HPP_

#include <string>
#include <vector>
#include <ostream>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

namespace fs = boost::filesystem;

class TestBundle: public ::testing::TestWithParam<fs::directory_entry>
{
protected:
	void SetUp ();
	void TearDown ();

	std::string m_config_name;
	YAML::Node m_config_data;
	typedef std::vector<std::string> scripts_t;
	scripts_t m_scripts;

};

/**
 *	Makes GTest able to print directory entries correctly.
 *	@{ */
namespace boost { namespace filesystem {
void PrintTo (const directory_entry& a_entry, ::std::ostream* a_stream)
{
	*a_stream << a_entry. path(). string();
}
}} // boost::filesystem
/**	@} */

#endif /* SRC_TEST_SCRIPT_HPP_ */
