/*
 * @file $/source/nparse-test/src/test_ndl.cpp
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
#include <anta/ndl.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/end.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/prefix.hpp>
#include "utils/parsing.hpp"

/**
 *	Test model definition.	
 */
namespace {

struct M1: public anta::ndl::extend<anta::model<> >::type {};

} // namespace

/**
 *	Links the debug observer with the test model.
 */
#if defined(DEBUG_PRINT)
ANTA_AUX_DEBUG_PRINT_OBSERVER(M1)
#endif

using namespace anta;
using namespace ndl;
using namespace ndl::terminals;
using boost::proto::lit;

/**
 *	Test case instantiation point.
 *	@{ */

TEST(test_ndl, negation1)
{
	ndl::Rule<M1> rule;
	rule = !lit("gamma") > +alpha > end;

	EXPECT_EQ( 1,	trace_count(rule. cluster(), "alpha") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "gamma") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "gammaTest") );
}

TEST(test_ndl, negation2)
{
	sas::Prefix<M1, unsigned int> prefix;
	prefix < "alpha" < "beta" < "gamma";

	ndl::Rule<M1> rule;
	rule = !(prefix > !alnum) > +alnum > '.' > end;

	EXPECT_EQ( 0,	trace_count(rule. cluster(), "alpha.") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "beta.") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "gamma.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "alphaTest.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "betaTest.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "gammaTest.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "delta.") );
}

TEST(test_ndl, negation3)
{
	sas::Prefix<M1, unsigned int> prefix;
	prefix < "alpha" < "beta" < "gamma";

	// WARNING: Enclosed negations are not allowed.
	ndl::Rule<M1> rule;
	rule = !(prefix > !(alnum > pass)) > +alnum > '.' > end;

	EXPECT_EQ( 0,	trace_count(rule. cluster(), "alpha.", 2048) );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "beta.", 2048) );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "gamma.", 2048) );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "alphaTest.", 2048) );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "betaTest.", 2048) );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "gammaTest.", 2048) );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "delta.", 2048) );
}

TEST(test_ndl, permutation1)
{
	ndl::Rule<M1> rule;
	rule = (lit("alpha") > "beta" | lit("beta") > "alpha") > '.' > end;

	EXPECT_EQ( 1,	trace_count(rule. cluster(), "alphabeta.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "betaalpha.") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "alphabeta") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "betaalpha") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "delta.") );
}

TEST(test_ndl, permutation2)
{
	ndl::Rule<M1> rule;
	rule =
		(	"alpha" > *space > "beta" 
		|	"beta" > *space > "alpha"
		) > '.' > end;

	EXPECT_EQ( 1,	trace_count(rule. cluster(), "alphabeta.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "betaalpha.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "alpha beta.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "beta alpha.") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "delta.") );
}

TEST(test_ndl, permutation3)
{
	ndl::Rule<M1> rule;
	rule = (lit("alpha") ^ lit("beta")) > '.' > end;

	EXPECT_EQ( 1,	trace_count(rule. cluster(), "alphabeta.") );
	EXPECT_EQ( 1,	trace_count(rule. cluster(), "betaalpha.") );
	EXPECT_EQ( 0,	trace_count(rule. cluster(), "delta.") );
}

/**	@} */
