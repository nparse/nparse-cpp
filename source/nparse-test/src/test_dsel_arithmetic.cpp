/*
 * @file $/source/nparse-test/src/test_dsel_arithmetic.cpp
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
#include <anta/dsel.hpp>
#include <anta/dsel/rt/plus.hpp>
#include <anta/dsel/rt/minus.hpp>
#include <anta/dsel/rt/multiplies.hpp>
#include <anta/dsel/rt/divides.hpp>
#include <anta/dsel/rt/modulus.hpp>
#include <gtest/gtest.h>

/**
 *	Test model definition.	
 */
namespace {

struct M2: public anta::model<anta::meta::narrow> {};

} // namespace

/**
 *	Model-specific type specializations.
 */
namespace anta {
namespace ndl {

template <>
struct context_value<M2>
{
	typedef aux::Variable<M2> type;

};

}} // namespace anta::ndl

/**
 *	Test data factory.	
 */
namespace {

using namespace anta;

// (forward declaration)
template <typename M_> class test_dsel;

/**
 *	A structure that holds test case data.
 */
template <typename M_>
struct test_case
{
	typedef M_ M;
	typedef aux::Variable<M_> variable;

	char func;
	bool throws;
	variable x, y, z;

};

/**
 *	Test fixture: instantiates common objects used for testing.
 */
template <typename M_>
class test_dsel: public test_case<M_>,
	public ::testing::TestWithParam<test_case<M_> >
{
protected:
	anta::ndl::Context<M_> Cx;

public:
	typedef bool result_type;

	template <typename V_>
	bool operator() (const V_& u, const V_& v) const
	{
		// If the type of result matches the type of expected value then it is
		// worth comparing values.
		return (u == v);
	}

	template <typename U_, typename V_>
	bool operator() (const U_&, const V_&) const
	{
		// Otherwise, if types of result and expected value do not match just
		// return false.
		return false;
	}

	bool operator() (const double u, const double v) const
	{
		// For real numbers there's a specific way of value comparison limited
		// to a certain precision.
		return ((u > v) ? u - v : v - u) < 1e-10;
	}

	bool operator() (const typename anta::ndl::context<M_>::type&,
			const typename anta::ndl::context<M_>::type&) const
	{
		throw std::logic_error("contexts (and arrays) are incomparable");
	}

};

/**
 *	Operator test fixture.
 */
class test_operator: public test_dsel<M2>
{
protected:
	// Overridden from ::testing::TestWithParam<..>
	void SetUp ()
	{
		const test_case<M>& entry = GetParam();
		func = entry. func;
		throws = entry. throws;
		variable( entry. x ). swap( x );
		variable( entry. y ). swap( y );
		variable( entry. z ). swap( z );
	}

};

} // namespace

/**
 *	This definition makes GTest able to print test entries correctly.
 *	@{ */
template <typename M_>
void PrintTo (const test_case<M_>& a_entry, ::std::ostream* a_stream)
{
	*a_stream
		<< a_entry. x. tag() << '(' << a_entry. x. as_string() << ')'
		<< ' ' << a_entry. func << ' '
		<< a_entry. y. tag() << '(' << a_entry. y. as_string() << ')'
		<< " --> ";
	if (a_entry. throws)
		*a_stream << "THROW";
	else
		*a_stream << a_entry. z. tag() << '(' << a_entry. z. as_string() << ')';
}
/**	@} */

/**
 *	Test case instantiation point.
 *	@{ */

static const anta::aux::unified_null_type null = anta::aux::unified_null_type();

static const test_case<M2> entries[] = {
//    (f)   Throws?     u:       v:   u (f) v --> f(u, v)
	{ '+',   false,      1L,      2L,      3L },
	{ '+',   false,     "1",      2L,    "12" },
	{ '+',   false,      1L,    "+2",      3L },
	{ '+',   false,      1L,    "-2",     -1L },
	{ '+',   false,     "a",     "b",    "ab" },
	{ '+',   false,      1L,     2.5,      3L },
	{ '+',   false,     2.5,      1L,     3.5 },
	{ '+',   false,     1.7,     4.3,     6.0 },
	{ '+',   false,     3.1,"4.159e-2",3.14159},
	{ '+',    true,    null,    true,    null },
	{ '+',    true,    true,    null,    null },
	{ '+',    true,    null,   1000L,    null },
	{ '+',   false,   1000L,    null,   1000L },
	{ '+',    true,    null,    -1.1,    null },
	{ '+',   false,    -1.1,    null,    -1.1 },
	{ '+',    true,    null,    "ab",    null },
	{ '+',   false,    "ab",    null,    "ab" },
	{ '+',    true,    null,    null,    null },

	{ '-',   false,      1L,      2L,     -1L },
	{ '-',    true,     "1",      2L,    null },
	{ '-',   false,      1L,    "+2",     -1L },
	{ '-',   false,      1L,    "-2",      3L },
	{ '-',    true,     "a",     "b",    null },
	{ '-',   false,      1L,     2.5,     -1L },
	{ '-',   false,     2.5,      1L,     1.5 },
	{ '-',   false,     1.7,     4.3,    -2.6 },
	{ '-',   false,     3.1,"4.159e-2",3.05841},
	{ '-',    true,    null,    true,    null },
	{ '-',    true,    true,    null,    null },
	{ '-',    true,    null,   1000L,    null },
	{ '-',   false,   1000L,    null,   1000L },
	{ '-',    true,    null,    -1.1,    null },
	{ '-',   false,    -1.1,    null,    -1.1 },
	{ '-',    true,    null,    "ab",    null },
	{ '-',    true,    "ab",    null,    null },
	{ '-',    true,    null,    null,    null },

	{ '*',   false,      6L,      7L,     42L },
	{ '*',   false,     2.5,      2L,     5.0 },
	{ '*',   false,      2L,     2.5,     5.0 },
	{ '*',   false,     2.5,     2.5,    6.25 },
	{ '*',    true,      6L,     "7",    null },
	{ '*',    true,     2.5,     "2",    null },
	{ '*',    true,      2L,   "2.5",    null },
	{ '*',    true,     2.5,   "2.5",    null },
	{ '*',    true,     "6",      7L,    null },
	{ '*',    true,   "2.5",      2L,    null },
	{ '*',    true,     "2",     2.5,    null },
	{ '*',    true,   "2.5",     2.5,    null },
	{ '*',    true,    null,    true,    null },
	{ '*',    true,    true,    null,    null },
	{ '*',   false,    null,   1000L,      0L },
	{ '*',   false,   1000L,    null,      0L },
	{ '*',   false,    null,    -1.1,     0.0 },
	{ '*',   false,    -1.1,    null,     0.0 },
	{ '*',    true,    null,    "ab",    null },
	{ '*',    true,    "ab",    null,    null },
	{ '*',    true,    null,    null,    null },

	{ '/',   false,     10L,      4L,      2L },
	{ '/',   false,    10.0,      4L,     2.5 },
	{ '/',   false,     10L,     4.0,     2.5 },
	{ '/',   false,    10.0,     4.0,     2.5 },
	{ '/',    true,     10L,     "4",    null },
	{ '/',    true,    10.0,     "4",    null },
	{ '/',    true,     10L,   "4.0",    null },
	{ '/',    true,    10.0,   "4.0",    null },
	{ '/',    true,    "10",      4L,    null },
	{ '/',    true,  "10.0",      4L,    null },
	{ '/',    true,    "10",     4.0,    null },
	{ '/',    true,  "10.0",     4.0,    null },
	{ '/',    true,    null,    true,    null },
	{ '/',    true,    true,    null,    null },
	{ '/',   false,    null,   1000L,      0L },
	{ '/',    true,   1000L,    null,    null }, // dbz
	{ '/',   false,    null,    -1.1,     0.0 },
	{ '/',    true,    -1.1,    null,    null }, // dbz
	{ '/',    true,    null,    "ab",    null },
	{ '/',    true,    "ab",    null,    null },
	{ '/',    true,    null,    null,    null },

	{ '%',   false,     10L,      4L,      2L },
	{ '%',   false,    10.0,      4L,     2.0 },
	{ '%',   false,     10L,     4.0,     2.0 },
	{ '%',   false,    10.0,     4.0,     2.0 },
	{ '%',    true,     10L,     "4",    null },
	{ '%',    true,    10.0,     "4",    null },
	{ '%',    true,     10L,   "4.0",    null },
	{ '%',    true,    10.0,   "4.0",    null },
	{ '%',    true,    "10",      4L,    null },
	{ '%',    true,  "10.0",      4L,    null },
	{ '%',    true,    "10",     4.0,    null },
	{ '%',    true,  "10.0",     4.0,    null },
	{ '%',    true,    null,    true,    null },
	{ '%',    true,    true,    null,    null },
	{ '%',   false,    null,   1000L,      0L },
	{ '%',    true,   1000L,    null,    null }, // dbz
	{ '%',   false,    null,    -1.1,     0.0 },
	{ '%',    true,    -1.1,    null,    null }, // dbz
	{ '%',    true,    null,    "ab",    null },
	{ '%',    true,    "ab",    null,    null },
	{ '%',    true,    null,    null,    null },

};

TEST_P(test_operator, arithmetic)
{
	using namespace anta::dsel;

	variable t;
	try
	{
		switch (func)
		{
		case '+':	t = (ct<M>(x) + y). fn(Cx); break;
		case '-':	t = (ct<M>(x) - y). fn(Cx); break;
		case '*':	t = (ct<M>(x) * y). fn(Cx); break;
		case '/':	t = (ct<M>(x) / y). fn(Cx); break;
		case '%':	t = (ct<M>(x) % y). fn(Cx); break;
		default:	FAIL() << "undefined operator `" << func << '\'';
		}

		if (throws)
			ADD_FAILURE() << "should throw an exception";
	}
	catch (const std::exception& err)
	{
		if (!throws)
		{
			FAIL() << "exception: " << err. what();
		}
		else
		{
			SUCCEED();
			return;
		}
	}

	EXPECT_STREQ( z. tag(), t. tag() )
		<< "wrong result type";
	EXPECT_TRUE( z. apply(*this, t) )
		<< "wrong result: expected `"
		<< z. as_string() << "', but got `"
		<< t. as_string() << '\'';
	EXPECT_TRUE( t. apply(*this, z) )
		<< "wrong result: symmetric comparison failed";
}

INSTANTIATE_TEST_CASE_P(
	DSEL, test_operator,
	::testing::ValuesIn(&*entries, &*entries + sizeof(entries)/sizeof(*entries))
);

/**	@} */
