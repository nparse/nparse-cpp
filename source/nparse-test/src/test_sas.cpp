/*
 * @file $/source/nparse-test/src/test_sas.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#include <anta/core.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/regex.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/pattern.hpp>

/**
 *	Test model definition.
 */
namespace {

struct M1: anta::model<> {};

} // namespace

/**
 *	Model-specific type specializations.
 */
namespace anta {

template <> struct spectrum<M1>
{
	class type: public std::vector<range<M1>::type>
	{
	public:
		void push (const ::anta::iterator<M1>::type& a_from,
				const ::anta::iterator<M1>::type& a_to)
		{
			push_back(value_type(a_from, a_to));
		}

	};

};

} // namespace anta

/**
 *	Test data factory.
 */
namespace {

using namespace anta;

/**
 *	Test fixture: instantiates common objects used for testing.
 */
class test_sas: public ::testing::Test
{
protected:
	std::vector<string<M1>::type> samples;
	spectrum<M1>::type spec;

	test_sas ()
	{
		samples. push_back("abcdefghijklmnopqrstuvwzyz");
		samples. push_back("19:30:00 GMT+3 6/11/1999");
		samples. push_back("ne/hi/ll");
		samples. push_back("3.14159");
	}

	range<M1>::type sample (const std::size_t a_index) const
	{
		const string<M1>::type& s = samples[a_index % samples. size()];
		return range<M1>::type(&* s. begin(), &* s. end());
	}

	range<M1>::type item (const std::size_t a_index, const std::size_t a_offset,
			const std::size_t a_length) const
	{
		range<M1>::type res = sample(a_index);
		std::advance(res. first, a_offset);
		res. second = res. first;
		std::advance(res. second, a_length);
		return res;
	}

	template <typename Acceptor_>
	void scan (const Acceptor_& a_acceptor, const std::size_t a_offset = 0,
			const std::size_t a_length = 0)
	{
		for (std::size_t index = 0; index < samples. size(); ++ index)
		{
			a_acceptor. accept(sample(index), item(index, a_offset, a_length),
					spec);
		}
	}

};

} // namespace

/**
 *	Test case instantiation point.
 *	@{ */

TEST_F(test_sas, unconditional_repeat)
{
	UnconditionalRepeat<M1> acc;
	scan(acc, 5, 3);
	ASSERT_EQ( 4, spec. size() );
	EXPECT_EQ( item(0, 5, 3), spec[0] );
	EXPECT_EQ( item(1, 5, 3), spec[1] );
	EXPECT_EQ( item(2, 5, 3), spec[2] );
	EXPECT_EQ( item(3, 5, 3), spec[3] );
}

TEST_F(test_sas, unconditional_shift)
{
	UnconditionalShift<M1> acc;
	scan(acc, 5, 3);
	ASSERT_EQ( 4, spec. size() );
	EXPECT_EQ( item(0, 8, 0), spec[0] );
	EXPECT_EQ( item(1, 8, 0), spec[1] );
	EXPECT_EQ( item(2, 8, 0), spec[2] );
	EXPECT_EQ( item(3, 8, 0), spec[3] );
}

TEST_F(test_sas, symbol1)
{
	sas::Symbol<M1> acc('1');
	scan(acc);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(1, 0, 1), spec[0] );
}

TEST_F(test_sas, string1)
{
	sas::String<M1> acc("ne");
	scan(acc);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(2, 0, 2), spec[0] );
}

TEST_F(test_sas, regex1)
{
	sas::RegEx<M1> acc("^\\w+");
	scan(acc);
	ASSERT_EQ( 4, spec. size() );
	EXPECT_EQ( item(0, 0, 26), spec[0] );
	EXPECT_EQ( item(1, 0, 2), spec[1] );
	EXPECT_EQ( item(2, 0, 2), spec[2] );
	EXPECT_EQ( item(3, 0, 1), spec[3] );
}

TEST_F(test_sas, regex2)
{
	sas::RegEx<M1> acc("(fgh|GMT|hi)");
	scan(acc);
	ASSERT_EQ( 3, spec. size() );
	EXPECT_EQ( item(0, 5, 3), spec[0] );
	EXPECT_EQ( item(1, 9, 3), spec[1] );
	EXPECT_EQ( item(2, 3, 2), spec[2] );
}

TEST_F(test_sas, ctest1)
{
	sas::Test<M1> acc(std::isalpha, sas::tfGreedy);
	scan(acc);
	ASSERT_EQ( 2, spec. size() );
	EXPECT_EQ( item(0, 0, 26), spec[0] );
	EXPECT_EQ( item(2, 0, 2),  spec[1] );
}

TEST_F(test_sas, ctest2)
{
	sas::Test<M1> acc(std::isalpha, sas::tfGreedy | sas::tfAcceptEmpty);
	scan(acc);
	ASSERT_EQ( 4, spec. size() );
	EXPECT_EQ( item(0, 0, 26), spec[0] );
	EXPECT_EQ( item(1, 0, 0),  spec[1] );
	EXPECT_EQ( item(2, 0, 2),  spec[2] );
	EXPECT_EQ( item(3, 0, 0),  spec[3] );
}

TEST_F(test_sas, pattern1)
{
	typedef sas::Pattern<M1> acceptor_type;
	acceptor_type::classes_type cc;
	cc['w'] = string<M1>::type("abcdefghijklmnopqrstuvwxyz");
	cc['W'] = string<M1>::type("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	cc['d'] = string<M1>::type("0123456789");
	cc['S'] = string<M1>::type("+-");

	acceptor_type
		a1("\\w+", cc),
		a2("\\w+\\$", cc),
		a3("abc\\w+\\$", cc),
		a4("\\d*:\\d*:\\d* \\W+\\S\\d+ \\d+/\\d+/\\d+\\$", cc),
		a5("ne/hi/ll\\$", cc),
		a6("..:..:.. ...\\S. ./../....\\$", cc),
		a7("../../..\\$", cc),
		a8("\\d+\\.\\d+\\$", cc);

	scan(a1);
	ASSERT_EQ( 2, spec. size() );
	EXPECT_EQ( item(0, 0, 26), spec[0] );
	EXPECT_EQ( item(2, 0, 2),  spec[1] );

	spec. clear();
	scan(a2);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(0, 0, 26), spec[0] );

	spec. clear();
	scan(a3);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(0, 0, 26), spec[0] );

	spec. clear();
	scan(a4);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(1, 0, 24), spec[0] );

	spec. clear();
	scan(a5);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(2, 0, 8), spec[0] );

	spec. clear();
	scan(a6);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(1, 0, 24), spec[0] );

	spec. clear();
	scan(a7);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(2, 0, 8), spec[0] );

	spec. clear();
	scan(a8);
	ASSERT_EQ( 1, spec. size() );
	EXPECT_EQ( item(3, 0, 7), spec[0] );
}

/** @} */
