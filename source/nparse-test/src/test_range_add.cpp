/*
 * @file $/source/nparse-test/src/test_range_add.cpp
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
#include <cstdlib>
#include <string>
#include <algorithm>
#include <gtest/gtest.h>
#include <util/range_add.hpp>

namespace {

class RangeTester
{
	std::vector<int> m_boundaries;
	std::string m_buffer;

	int min (const int a, const int b) const
	{
		return std::max(std::min(a, b), 0);
	}

	int max (const int a, const int b) const
	{
		return std::min(std::max(a, b), static_cast<int>(m_buffer. size()));
	}

public:
	RangeTester (const int a_buffer_size):
		m_buffer (static_cast<std::size_t>(std::max(a_buffer_size, 0)), '-')
	{
	}

	void add (const int a_from, const int a_to)
	{
		utility::range_add(m_boundaries, a_from, a_to);
		for (int i = min(a_from, a_to), j = max(a_from, a_to); i < j; ++ i)
		{
			m_buffer[ static_cast<std::size_t>(i) ] = '|';
		}
	}

	const std::string& buffered () const
	{
		return m_buffer;
	}

	const int boundary_count () const
	{
		return static_cast<int>(m_boundaries. size());
	}

	const int size () const
	{
		return static_cast<int>(m_buffer. size());
	}

	const std::string generated () const
	{
		std::string generated(m_buffer. size(), '-');
		std::vector<int>::const_iterator b = m_boundaries. begin();
		while (b != m_boundaries. end())
		{
			int i = *b;
			++ b;
			const int j = *b;
			++ b;
			for ( ; i < j; ++ i)
			{
				generated[ static_cast<std::size_t>(i) ] = '|';
			}
		}
		return generated;
	}

	const int generated_boundary_count () const
	{
		int count = 0;
		const std::string temp = generated();
		for (std::string::const_iterator i = temp. begin(); i != temp. end();
				++ i)
		{
			if (*i == '|' && (i == temp. begin() || *(i - 1) == '-'))
			{
				++ count;
			}
		}
		return 2 * count;
	}

};

} // namespace

TEST(range_add, correctness101)
{
	RangeTester t(20);
	t.add(5, 15);
	EXPECT_EQ( t. buffered(), "-----||||||||||-----" );
	EXPECT_EQ( t. buffered(), t. generated() );
	EXPECT_EQ( t. boundary_count(), 2 );
}

TEST(range_add, correctness102)
{
	RangeTester t(20);
	t.add(15, 5);
	EXPECT_EQ( t. buffered(), "-----||||||||||-----" );
	EXPECT_EQ( t. buffered(), t. generated() );
	EXPECT_EQ( t. boundary_count(), 2 );
}

TEST(range_add, correctness103)
{
	RangeTester t(20);
	t.add(5, 10);
	t.add(10, 15);
	EXPECT_EQ( t. buffered(), "-----||||||||||-----" );
	EXPECT_EQ( t. buffered(), t. generated() );
	EXPECT_EQ( t. boundary_count(), 2 );
}

TEST(range_add, correctness104)
{
	RangeTester t(20);
	t.add(10, 15);
	t.add(5, 10);
	EXPECT_EQ( t. buffered(), "-----||||||||||-----" );
	EXPECT_EQ( t. buffered(), t. generated() );
	EXPECT_EQ( t. boundary_count(), 2 );
}

TEST(range_add, correctness105)
{
	RangeTester t(20);
	t.add(5, 9);
	t.add(10, 15);
	EXPECT_EQ( t. buffered(), "-----||||-|||||-----" );
	EXPECT_EQ( t. buffered(), t. generated() );
	EXPECT_EQ( t. boundary_count(), 4 );
}

TEST(range_add, correctness106)
{
	RangeTester t(20);
	t.add(10, 15);
	t.add(5, 9);
	EXPECT_EQ( t. buffered(), "-----||||-|||||-----" );
	EXPECT_EQ( t. buffered(), t. generated() );
	EXPECT_EQ( t. boundary_count(), 4 );
}

namespace {

class TestCase: public ::testing::TestWithParam<std::pair<int, int> >
{
protected:
	void SetUp ();
	void TearDown ();

	int m_max_range;
	int m_max_steps;
	int m_max_addition;
	int m_generator_seed;

};

void TestCase::SetUp ()
{
	const std::pair<int, int> param = GetParam();
	m_max_range = 1024; // [ 0 .. 1023 ]
	m_max_steps = 100;
	m_max_addition = param. first;
	m_generator_seed = param. second;
}

void TestCase::TearDown ()
{
}

} // namespace

TEST_P(TestCase, random_fill)
{
	srand(m_generator_seed);
	RangeTester t(m_max_range);
	for (int step = 0; step < m_max_steps; ++ step)
	{
		const int gap = rand() % m_max_addition;
		const int from = rand() % (t. size() - gap);
		const int to = from + gap;
		if (step & 1)
		{
			t. add(from, to);
		}
		else
		{
			t. add(to, from);
		}
		EXPECT_EQ( t. buffered(), t. generated() );
		EXPECT_EQ( t. boundary_count(), t. generated_boundary_count() );
	}
}

INSTANTIATE_TEST_CASE_P(
	range_add,
	TestCase,
	::testing::Values(
		std::make_pair( 10, 10001),
		std::make_pair( 20, 10002),
		std::make_pair( 30, 10003),
		std::make_pair( 50, 10004),
		std::make_pair( 60, 10005),
		std::make_pair( 70, 10006),
		std::make_pair( 80, 10007),
		std::make_pair( 90, 10008),
		std::make_pair(100, 10009)
	)
);
