/*
 * @file $/source/nparse-test/src/test_libencode.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#include <gtest/gtest.h>
#include <encode/encode.hpp>

#define NARROW_TEXT \
	"\x61\x6c\x70\x68\x61\x20\x62\x65\x74\x61\x20\x67\x61\x6d\x61\n"\
	"\xe0\xa4\x93\xe0\xa4\x82\x20\xe0\xa4\xae\xe0\xa4\xa3\xe0\xa4\xbf\xe0\xa4"\
		"\xaa\xe0\xa4\xa6\xe0\xa5\x8d\xe0\xa4\xae\xe0\xa5\x87\x20\xe0\xa4\xb9"\
		"\xe0\xa5\x82\xe0\xa4\x82\n"\
	"\xe0\xb9\x82\xe0\xb8\xad\xe0\xb8\xa1\xe0\xb8\xba\x20\xe0\xb8\xa1\xe0\xb8"\
		"\x93\xe0\xb8\xb4\x20\xe0\xb8\x9b\xe0\xb8\x97\xe0\xb8\xba\xe0\xb9\x80"\
		"\xe0\xb8\xa1\x20\xe0\xb8\xab\xe0\xb8\xb9\xe0\xb8\xa1\xe0\xb8\xba\n"\
	"\xe0\xbd\xa8\xe0\xbd\xbc\xe0\xbd\xbe\xe0\xbc\x8b\xe0\xbd\x98\xe0\xbc\x8b"\
		"\xe0\xbd\x8e\xe0\xbd\xb2\xe0\xbc\x8b\xe0\xbd\x94\xe0\xbc\x8b\xe0\xbd"\
		"\x91\xe0\xbe\xa8\xe0\xbd\xba\xe0\xbc\x8b\xe0\xbd\xa7\xe0\xbd\xb1\xe0"\
		"\xbd\xb4\xe0\xbe\x83\n"\
	"\xe5\x97\xa1\xe5\x98\x9b\xe5\x91\xa2\xe5\x8f\xad\xe5\x92\xa9\xe5\x90\xbd\n"\
	"\xe1\xa0\xa3\xe1\xa0\xa7\xe1\xa0\xae\n"\
	"\xe1\xa0\xae\xe1\xa0\xa0\n"\
	"\xe1\xa0\xa8\xe1\xa0\xa2\n"\
	"\xe1\xa0\xaa\xe1\xa0\xa0\xe1\xa0\xb3\n"\
	"\xe1\xa0\xae\xe1\xa0\xa1\xe1\xa0\xa2\n"\
	"\xe1\xa0\xac\xe1\xa0\xa4\xe1\xa0\xa9\n"

#define WIDE_TEXT \
	L"\x0061\x006c\x0070\x0068\x0061\x0020\x0062\x0065\x0074\x0061\x0020"\
		L"\x0067\x0061\x006d\x0061\n"\
	L"\x0913\x0902\x0020\x092e\x0923\x093f\x092a\x0926\x094d\x092e\x0947"\
		"\x0020\x0939\x0942\x0902\n"\
	L"\x0e42\x0e2d\x0e21\x0e3a\x0020\x0e21\x0e13\x0e34\x0020\x0e1b\x0e17"\
		"\x0e3a\x0e40\x0e21\x0020\x0e2b\x0e39\x0e21\x0e3a\n"\
	L"\x0f68\x0f7c\x0f7e\x0f0b\x0f58\x0f0b\x0f4e\x0f72\x0f0b\x0f54\x0f0b"\
		"\x0f51\x0fa8\x0f7a\x0f0b\x0f67\x0f71\x0f74\x0f83\n"\
	L"\x55e1\x561b\x5462\x53ed\x54a9\x543d\n"\
	L"\x1823\x1827\x182e\n"\
	L"\x182e\x1820\n"\
	L"\x1828\x1822\n"\
	L"\x182a\x1820\x1833\n"\
	L"\x182e\x1821\x1822\n"\
	L"\x182c\x1824\x1829\n"

namespace e = encode;

TEST(libencode, consistency)
{
	const char* narrow_c_string = NARROW_TEXT;
	const wchar_t* wide_c_string = WIDE_TEXT;
	const std::string narrow_stl_string (narrow_c_string);
	const std::wstring wide_stl_string (wide_c_string);

	// Check initialized strings using GTest.
	ASSERT_STREQ(narrow_c_string, narrow_stl_string. c_str());
	ASSERT_STREQ(wide_c_string, wide_stl_string. c_str());

	// Check initialized string using member equality operator.
	ASSERT_EQ(narrow_stl_string, narrow_c_string);
	ASSERT_EQ(wide_stl_string, wide_c_string);

	// Test trivial copying constructors.
	EXPECT_EQ(narrow_stl_string, e::string(narrow_c_string));
	EXPECT_EQ(wide_stl_string, e::wstring(wide_c_string));
	EXPECT_EQ(narrow_stl_string, e::string(narrow_stl_string));
	EXPECT_EQ(wide_stl_string, e::wstring(wide_stl_string));

	// Test trivial range constructors.
	EXPECT_EQ(wide_stl_string, e::make<std::wstring>::from(
				wide_stl_string. begin(), wide_stl_string. end()));
	EXPECT_EQ(narrow_stl_string, e::make<std::string>::from(
				narrow_stl_string. begin(), narrow_stl_string. end()));

	// Test narrow to wide conversion.
	EXPECT_EQ(wide_stl_string, e::wstring(narrow_c_string));
	EXPECT_EQ(wide_c_string, e::wstring(narrow_stl_string));
	EXPECT_EQ(wide_stl_string, e::make<std::wstring>::from(
				narrow_stl_string. begin(), narrow_stl_string. end()));

	// Test wide to narrow conversion.
	EXPECT_EQ(narrow_stl_string, e::string(wide_c_string));
	EXPECT_EQ(narrow_c_string, e::string(wide_stl_string));
	EXPECT_EQ(narrow_stl_string, e::make<std::string>::from(
				wide_stl_string. begin(), wide_stl_string. end()));

	// Test STL string wrappers.
	EXPECT_EQ(narrow_stl_string, e::wrapper<std::string>(wide_c_string));
	EXPECT_EQ(narrow_c_string, e::wrapper<std::string>(wide_stl_string));
	EXPECT_EQ(wide_stl_string, e::wrapper<std::wstring>(wide_c_string));
	EXPECT_EQ(wide_c_string, e::wrapper<std::wstring>(wide_stl_string));
	EXPECT_EQ(narrow_stl_string, e::wrapper<std::string>(narrow_c_string));
	EXPECT_EQ(narrow_c_string, e::wrapper<std::string>(narrow_stl_string));
	EXPECT_EQ(wide_stl_string, e::wrapper<std::wstring>(narrow_c_string));
	EXPECT_EQ(wide_c_string, e::wrapper<std::wstring>(narrow_stl_string));

	// Test single character conversions.
	EXPECT_EQ('?', e::make<char>::from(L'\x042F'));
	EXPECT_EQ('R', e::make<char>::from(L'R'));
	EXPECT_EQ(L'\x042F', e::make<wchar_t>::from("\xd0\xaf"));
	EXPECT_EQ(L'R', e::make<wchar_t>::from('R'));
}
