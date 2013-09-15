#include "test_casts.hpp"

#define D_CAST(u, v) this->d_cast(this->u, this->v)
#define P_CAST(u, v) this->p_cast(this->u, this->v)

//
//	tests for left/right directed casts
//

TYPED_TEST_P(TestCast, directed_cast_empty)
{
	EXPECT_EQ( "null",		D_CAST(n, n) );
	EXPECT_EQ( "null",		D_CAST(n, b) );
	EXPECT_EQ( "null",		D_CAST(n, i) );
	EXPECT_EQ( "null",		D_CAST(n, r) );
	EXPECT_EQ( "null",		D_CAST(n, s) );
	EXPECT_EQ( "null",		D_CAST(n, a) );
}

TYPED_TEST_P(TestCast, directed_cast_boolean)
{
	EXPECT_EQ( "boolean",	D_CAST(b, n) );
	EXPECT_EQ( "boolean",	D_CAST(b, b) );
	EXPECT_EQ( "boolean",	D_CAST(b, i) );
	EXPECT_EQ( "boolean",	D_CAST(b, r) );
	EXPECT_EQ( "boolean",	D_CAST(b, s1) );
	ASSERT_THROW( D_CAST(b, s), std::bad_cast );
	ASSERT_THROW( D_CAST(b, a), std::bad_cast );
}

TYPED_TEST_P(TestCast, directed_cast_integer)
{
	EXPECT_EQ( "integer",	D_CAST(i, n) );
	EXPECT_EQ( "integer",	D_CAST(i, b) );
	EXPECT_EQ( "integer",	D_CAST(i, i) );
	EXPECT_EQ( "integer",	D_CAST(i, r) );
	EXPECT_EQ( "integer",	D_CAST(i, s2) );
	ASSERT_THROW( D_CAST(i, s), std::bad_cast );
	ASSERT_THROW( D_CAST(i, a), std::bad_cast );
}

TYPED_TEST_P(TestCast, directed_cast_real)
{
	EXPECT_EQ( "real",		D_CAST(r, n) );
	EXPECT_EQ( "real",		D_CAST(r, b) );
	EXPECT_EQ( "real",		D_CAST(r, i) );
	EXPECT_EQ( "real",		D_CAST(r, r) );
	EXPECT_EQ( "real",		D_CAST(r, s3) );
	ASSERT_THROW( D_CAST(r, s), std::bad_cast );
	ASSERT_THROW( D_CAST(r, a), std::bad_cast );
}

TYPED_TEST_P(TestCast, directed_cast_string)
{
	EXPECT_EQ( "string",	D_CAST(s, n) );
	EXPECT_EQ( "string",	D_CAST(s, b) );
	EXPECT_EQ( "string",	D_CAST(s, i) );
	EXPECT_EQ( "string",	D_CAST(s, r) );
	EXPECT_EQ( "string",	D_CAST(s, s) );
	EXPECT_EQ( "string",	D_CAST(s, a) );
}

TYPED_TEST_P(TestCast, directed_cast_array)
{
	EXPECT_EQ( "array",		D_CAST(a, n) );
	EXPECT_EQ( "array",		D_CAST(a, b) );
	EXPECT_EQ( "array",		D_CAST(a, i) );
	EXPECT_EQ( "array",		D_CAST(a, r) );
	EXPECT_EQ( "array",		D_CAST(a, s) );
	EXPECT_EQ( "array",		D_CAST(a, a) );
}

//
//	tests for priority cast
//

TYPED_TEST_P(TestCast, priority_cast_empty)
{
	EXPECT_EQ( "null",		P_CAST(n, n) );
	EXPECT_EQ( "boolean",	P_CAST(n, b) );
	EXPECT_EQ( "integer",	P_CAST(n, i) );
	EXPECT_EQ( "real",		P_CAST(n, r) );
	EXPECT_EQ( "string",	P_CAST(n, s) );
	EXPECT_EQ( "array",		P_CAST(n, a) );
}

TYPED_TEST_P(TestCast, priority_cast_boolean)
{
	EXPECT_EQ( "boolean",	P_CAST(b, n) );
	EXPECT_EQ( "boolean",	P_CAST(b, b) );
	EXPECT_EQ( "integer",	P_CAST(b, i) );
	EXPECT_EQ( "real",		P_CAST(b, r) );
	EXPECT_EQ( "string",	P_CAST(b, s) );
	EXPECT_EQ( "array",		P_CAST(b, a) );
}

TYPED_TEST_P(TestCast, priority_cast_integer)
{
	EXPECT_EQ( "integer",	P_CAST(i, n) );
	EXPECT_EQ( "integer",	P_CAST(i, b) );
	EXPECT_EQ( "integer",	P_CAST(i, i) );
	EXPECT_EQ( "real",		P_CAST(i, r) );
	EXPECT_EQ( "string",	P_CAST(i, s) );
	EXPECT_EQ( "array",		P_CAST(i, a) );
}

TYPED_TEST_P(TestCast, priority_cast_real)
{
	EXPECT_EQ( "real",		P_CAST(r, n) );
	EXPECT_EQ( "real",		P_CAST(r, b) );
	EXPECT_EQ( "real",		P_CAST(r, i) );
	EXPECT_EQ( "real",		P_CAST(r, r) );
	EXPECT_EQ( "string",	P_CAST(r, s) );
	EXPECT_EQ( "array",		P_CAST(r, a) );
}

TYPED_TEST_P(TestCast, priority_cast_string)
{
	EXPECT_EQ( "string",	P_CAST(s, n) );
	EXPECT_EQ( "string",	P_CAST(s, b) );
	EXPECT_EQ( "string",	P_CAST(s, i) );
	EXPECT_EQ( "string",	P_CAST(s, r) );
	EXPECT_EQ( "string",	P_CAST(s, s) );
	EXPECT_EQ( "array",		P_CAST(s, a) );
}

TYPED_TEST_P(TestCast, priority_cast_array)
{
	EXPECT_EQ( "array",		P_CAST(a, n) );
	EXPECT_EQ( "array",		P_CAST(a, b) );
	EXPECT_EQ( "array",		P_CAST(a, i) );
	EXPECT_EQ( "array",		P_CAST(a, r) );
	EXPECT_EQ( "array",		P_CAST(a, s) );
	EXPECT_EQ( "array",		P_CAST(a, a) );
}

REGISTER_TYPED_TEST_CASE_P(TestCast,
		directed_cast_empty,
		directed_cast_boolean,
		directed_cast_integer,
		directed_cast_real,
		directed_cast_string,
		directed_cast_array,
		priority_cast_empty,
		priority_cast_boolean,
		priority_cast_integer,
		priority_cast_real,
		priority_cast_string,
		priority_cast_array
);

/**
 *	Test instantiation point.
 *	@{ */

const TestEntry TestEntry::entries[] = {
	{ true,       1,    0.0,       "these",  "true",     "1",  "2.97e8" },
	{ false,     -1,   1.00,         "are",  "true",    "-1",    "1e-0" },
	{ true,       0,  -1.00,      "random", "false",    "12",    "0e-1" },
	{ false,     42,   2.71,      "sample", "false",   "-12", "-0.3e+5" },
	{ true,     -43,  -3.14,     "strings",  "true",    "42", "-0.3e-5" },
	{ false,  70846,   1e10,         "---",  "true",   "-42", "0.25e11" },
	{ true,  -34310,  -1e10,  "should not", "false",  "1000",    "2e-6" },
	{ false,  46193,  1e-10,     "contain", "false", "-1000",    "13e6" },
	{ true,    2013, -1e-10, "convertible",  "true",  "1982",     "0.0" },
	{ false,  -1982,   9.11,      "values",  "true",  "2013",       "0" }
};

typedef ::testing::Types<
	Pair<M1, 0>, Pair<M1, 1>, Pair<M1, 2>, Pair<M1, 3>, Pair<M1, 4>,
	Pair<M1, 5>, Pair<M1, 6>, Pair<M1, 7>, Pair<M1, 8>, Pair<M1, 9>,
	Pair<M2, 0>, Pair<M2, 1>, Pair<M2, 2>, Pair<M2, 3>, Pair<M2, 4>,
	Pair<M2, 5>, Pair<M2, 6>, Pair<M2, 7>, Pair<M2, 8>, Pair<M2, 9>,
	Pair<M3, 0>, Pair<M3, 1>, Pair<M3, 2>, Pair<M3, 3>, Pair<M3, 4>,
	Pair<M3, 5>, Pair<M3, 6>, Pair<M3, 7>, Pair<M3, 8>, Pair<M3, 9>
> Models;

INSTANTIATE_TYPED_TEST_CASE_P(cast, TestCast, Models);

/**	@} */
