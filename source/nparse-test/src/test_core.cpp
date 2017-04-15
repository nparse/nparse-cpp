/*
 * @file $/source/nparse-test/src/test_core.cpp
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
#include <gtest/gtest.h>
#include <anta/core.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/end.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include "util/parsing.hpp"

/**
 *	Test model definition.	
 */
namespace {

struct M1: anta::model<> {};

} // namespace

/**
 *	Links the debug observer with the test model.
 */
#if defined(DEBUG_PRINT)
ANTA_AUX_DEBUG_PRINT_OBSERVER(M1)
#endif

/**
 *	Test case factory.
 */
namespace {

using namespace anta;

/**
 *	Test fixture: instantiates common objects used for testing.
 */
class test_core: public ::testing::Test
{
protected:
	// acceptors
	UnconditionalRepeat<M1> pass;
	sas::End<M1> end;
	sas::Symbol<M1> pound;
	sas::String<M1> alpha, omega, _12345;
	sas::Test<M1> letters, lowercase, digits, punct;

	// nodes
	Node<M1> entry, exit, term;

	// fixture constructor
	test_core ():
		pound ('#'),
		alpha ("alpha"), omega ("omega"), _12345 ("12345"),
		letters (std::isalpha, sas::tfGreedy),
		lowercase (std::islower, sas::tfGreedy),
		digits (std::isdigit, sas::tfGreedy),
		punct (std::ispunct)
	{
	}

};

} // namespace

/**
 *	Test case instantiation point.
 *	@{ */

TEST_F(test_core, transition_SS)
{
	// define network
	entry. link(exit, alpha, atSimple, 1);
	entry. link(exit, omega, atSimple, 2);
	exit. link(term, end, atSimple, 3);

	// test parsing
	EXPECT_EQ( "3 | 1 3",	parse(entry, "alpha") );
	EXPECT_EQ( "3 | 2 3",	parse(entry, "omega") );
	EXPECT_EQ( "1",			parse(entry, "gamma") );
}

TEST_F(test_core, transition_IS)
{
	// define network
	entry. link(term, punct, atInvoke, 1);
	entry. link(exit, alpha, atSimple, 2);
	exit. link(term, end, atSimple, 3);

	// test parsing
	EXPECT_EQ( "4 | 1 2 3",	parse(entry, "!alpha") );
	EXPECT_EQ( "1",			parse(entry, "alpha") );
	EXPECT_EQ( "2",			parse(entry, "?") );
}

TEST_F(test_core, transition_PSS)
{
	// define network
	entry. link(term, letters, atPositive, 1);
	entry. link(exit, alpha, atSimple, 2);
	entry. link(exit, _12345, atSimple, 3);
	exit. link(term, end, atSimple, 4);

	// test parsing
	EXPECT_EQ( "4 | 2 4",	parse(entry, "alpha") );
	EXPECT_EQ( "1",			parse(entry, "12345") );
}

TEST_F(test_core, transition_NS)
{
	// define network
	entry. link(term, alpha, atNegative, 1);
	entry. link(exit, letters, atSimple, 2);
	exit. link(term, end, atSimple, 3);

	// test parsing
	EXPECT_EQ( "2",			parse(entry, "alpha") );
	EXPECT_EQ( "3 | 2 3",	parse(entry, "gamma") );
	EXPECT_EQ( "3 | 2 3",	parse(entry, "delta") );
}

TEST_F(test_core, transition_IISS)
{
	// define network
	entry. link(term, punct, atInvoke, 1);
	entry. link(term, pound, atInvoke, 2);
	entry. link(exit, alpha, atSimple, 3);
	entry. link(exit, omega, atSimple, 4);
	exit. link(term, end, atSimple, 5);

	// test parsing
	EXPECT_EQ( "5 | 1 2 3 5",	parse(entry, "!#alpha") );
	EXPECT_EQ( "5 | 1 2 4 5",	parse(entry, "!#omega") );
	EXPECT_EQ( "2",				parse(entry, "#alpha") );
	EXPECT_EQ( "2",				parse(entry, "#omega") );
	EXPECT_EQ( "1",				parse(entry, "alpha") );
	EXPECT_EQ( "1",				parse(entry, "omega") );
	EXPECT_EQ( "2",				parse(entry, "?") );
}

TEST_F(test_core, transition_IPSS)
{
	// define network
	entry. link(term, pound, atInvoke, 1);
	entry. link(term, digits, atPositive, 2);
	entry. link(exit, alpha, atSimple, 3);
	entry. link(exit, _12345, atSimple, 4);
	exit. link(term, end, atSimple, 5);

	// test parsing
	EXPECT_EQ( "5 | 1 4 5",	parse(entry, "#12345") );
	EXPECT_EQ( "2",			parse(entry, "#alpha") );
	EXPECT_EQ( "1",			parse(entry, "12345") );
	EXPECT_EQ( "1",			parse(entry, "alpha") );
}

TEST_F(test_core, transition_INS)
{
	// define network
	entry. link(term, pound, atInvoke, 1);
	entry. link(term, omega, atNegative, 2);
	entry. link(exit, letters, atSimple, 3);
	exit. link(term, end, atSimple, 4);

	// test parsing
	EXPECT_EQ( "4 | 1 3 4",	parse(entry, "#alpha") );
	EXPECT_EQ( "3",			parse(entry, "#omega") );
	EXPECT_EQ( "1",			parse(entry, "alpha") );
	EXPECT_EQ( "1",			parse(entry, "omega") );
}

TEST_F(test_core, transition_IPPSS)
{
	// define network
	entry. link(term, pound, atInvoke, 1);
	entry. link(term, letters, atPositive, 2);
	entry. link(term, lowercase, atPositive, 3);
	entry. link(exit, alpha, atSimple, 4);
	entry. link(exit, omega, atSimple, 5);
	exit. link(term, end, atSimple, 6);

	// test parsing
	EXPECT_EQ( "1",			parse(entry, "alpha") );
	EXPECT_EQ( "2",			parse(entry, "#12345") );
	EXPECT_EQ( "3",			parse(entry, "#Alpha") );
	EXPECT_EQ( "4",			parse(entry, "#alPHA") );
	EXPECT_EQ( "6 | 1 4 6",	parse(entry, "#alpha") );
	EXPECT_EQ( "6 | 1 5 6",	parse(entry, "#omega") );
}

TEST_F(test_core, transition_IPNSS)
{
	sas::String<M1> alpha1("ALPHA"), omega1("OMEGA");

	// define network
	entry. link(term, pound, atInvoke, 1);
	entry. link(term, letters, atPositive, 2);
	entry. link(term, lowercase, atNegative, 3);
	entry. link(exit, alpha1, atSimple, 4);
	entry. link(exit, omega1, atSimple, 5);
	exit. link(term, end, atSimple, 6);

	// test parsing
	EXPECT_EQ( "3",			parse(entry, "#Alpha") );
	EXPECT_EQ( "3",			parse(entry, "#Omega") );
	EXPECT_EQ( "4",			parse(entry, "#alpha") );
	EXPECT_EQ( "4",			parse(entry, "#omega") );
	EXPECT_EQ( "5 | 1 4 6",	parse(entry, "#ALPHA") );
	EXPECT_EQ( "5 | 1 5 6",	parse(entry, "#OMEGA") );
}

TEST_F(test_core, transition_INPSS)
{
	sas::String<M1> alpha1("ALPHA"), omega1("OMEGA");

	// define network
	entry. link(term, pound, atInvoke, 1);
	entry. link(term, lowercase, atNegative, 3);
	entry. link(term, letters, atPositive, 2);
	entry. link(exit, alpha1, atSimple, 4);
	entry. link(exit, omega1, atSimple, 5);
	exit. link(term, end, atSimple, 6);

	// test parsing
	EXPECT_EQ( "3",			parse(entry, "#Alpha") );
	EXPECT_EQ( "3",			parse(entry, "#Omega") );
	EXPECT_EQ( "3",			parse(entry, "#alpha") );
	EXPECT_EQ( "3",			parse(entry, "#omega") );
	EXPECT_EQ( "5 | 1 4 6",	parse(entry, "#ALPHA") );
	EXPECT_EQ( "5 | 1 5 6",	parse(entry, "#OMEGA") );
}

TEST_F(test_core, transition_NNSSS)
{
	// define network
	entry. link(term, digits, atNegative, 1);
	entry. link(term, letters, atNegative, 2);
	entry. link(exit, alpha, atSimple, 3);
	entry. link(exit, _12345, atSimple, 4);
	entry. link(exit, pound, atSimple, 5);
	exit. link(term, end, atSimple, 6);

	// test parsing
	EXPECT_EQ( "2",			parse(entry, "alpha") );
	EXPECT_EQ( "2",			parse(entry, "12345") );
	EXPECT_EQ( "3 | 5 6",	parse(entry, "#") );
}

TEST_F(test_core, transition_NNIS)
{
	// define additional node chains
	NodeChain<M1> ch1(4, 100), ch2(6, 200), ch3(2, 300);

	// define network
	entry. link(ch1[0], pass, atNegative, 1);		// N
	ch1. back(). link(term, digits, atSimple, -1);
	entry. link(ch2[0], pass, atNegative, 2);		// N
	ch2. back(). link(term, letters, atSimple, -2);
	entry. link(ch3[0], pass, atInvoke, 3);			// I
	ch3. back(). link(exit, alpha, atSimple, 4);
	ch3. back(). link(exit, _12345, atSimple, 5);
	ch3. back(). link(exit, pound, atSimple, 6);
	entry. link(exit, pass, atSimple, 7);			// S
	exit. link(term, end, atSimple, 8);

	// test parsing
	EXPECT_EQ( 0,	trace_count(entry, "alpha", 2048) );
	EXPECT_EQ( 0,	trace_count(entry, "12345", 2048) );
	EXPECT_EQ( 1,	trace_count(entry, "#", 2048) );
}

/**	@} */
