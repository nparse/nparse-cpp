/*
 * @file $/source/nparse-test/src/test_dsel_casts.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.6

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex@nparse.com>
 
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
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/if.hpp>
#include <anta/dsel.hpp>
#include <gtest/gtest.h>

/**
 *	Test model definition.	
 */
namespace {

// NOTE: Since it is impossible to obtain the originating model for the native
//		 types, all relevant test cases MUST use the default ANTA model.
typedef anta::model<> M1;

struct M2: public anta::model<anta::meta::narrow> {};

struct M3: public anta::model<anta::meta::wide> {};

} // namespace

/**
 *	Model-specific type specializations.
 */
namespace anta {

template <>
struct string<M3>
{
	typedef encode::wrapper<std::basic_string<character<M3>::type> > type;

};

namespace ndl {

template <>
struct context_value<M2>
{
	typedef aux::Variable<M2> type;

};

template <>
struct context_value<M3>
{
	typedef aux::Variable<M3> type;

};

} // namespace ndl

template <>
struct model_of<ndl::context<M2>::type> { typedef M2 type; };

template <>
struct model_of<ndl::context<M3>::type> { typedef M3 type; };

} // namespace anta

/**
 *	Test data factory.	
 */
namespace {

/**
 *	A structure that holds test case data.
 */
struct test_case
{
	bool			b;
	int				i;
	double			r;
	const char*		s;
	const char*		s1;
	const char*		s2;
	const char*		s3;

	static const test_case entries[10];

};

/**
 *	A structure that ties models and particular test cases.
 */
template <typename M_, int C_>
struct test_pair
{
	typedef M_ Model;
	static const int Case = C_;

};

/**
 *	The probe functor. Given two arguments, returns a string containing one or
 *	two comma-separated tags representing types of the arguments.
 */
struct test_probe
{
	typedef std::string result_type;

	template <typename U_, typename V_>
	result_type operator() (const U_& u, const V_& entries) const
	{
		using anta::aux::tag;
		return result_type(tag(u)) + ", " + tag(entries);
	}

	template <typename U_>
	result_type operator() (const U_& u, const U_& entries) const
	{
		return result_type(anta::aux::tag(u));
	}

};

/**
 *	Test fixture: instantiates common objects used for testing.
 */
template <typename P_>
class test_cast: public ::testing::Test
{
protected:
	typedef typename boost::mpl::if_<boost::is_same<typename P_::Model, void>,
			anta::model<>, typename P_::Model>::type Model;

	typename anta::ndl::ContextOwner<Model>		owner;
	typename anta::aux::null<Model>::type		n;
	typename anta::aux::boolean<Model>::type	b;
	typename anta::aux::integer<Model>::type	i;
	typename anta::aux::real<Model>::type		r;
	typename anta::string<Model>::type			s, s1, s2, s3;
	typename anta::aux::array<Model>::type		a;

	test_cast ():
		owner (),
		n	(),
		b	(test_case::entries[P_::Case].b),
		i	(test_case::entries[P_::Case].i),
		r	(test_case::entries[P_::Case].r),
		s	(test_case::entries[P_::Case].s),
		s1	(test_case::entries[P_::Case].s1),
		s2	(test_case::entries[P_::Case].s2),
		s3	(test_case::entries[P_::Case].s3)
	{
		this -> owner. set_capacity(1024);
		this -> a = typename anta::aux::array<Model>::type(
				this -> owner. create(NULL));
	}

	/**
	 *	Applies the specified cast C to the given arguments L and R and sends
	 *	its values to the probe functor.
	 */
	template <typename L_, typename R_, typename C_>
	static std::string x_cast (const L_& a_left, const R_& a_right,
			const C_& a_cast)
	{
		return a_cast(a_left, a_right, test_probe());
	}

	/**
	 *	Applies both left and right casts to the given arguments L and R.
	 */
	template <typename U_, typename V_>
	static std::string d_cast (const U_& u0, const V_& v0)
	{
		using namespace anta::dsel::util;
		const anta::aux::Variable<Model> u(u0);
		const anta::aux::Variable<Model> entries(v0);
		const std::string uv = x_cast(u, entries, left_cast);
		const std::string vu = x_cast(entries, u, right_cast);
		if (uv == vu)
		{
			return uv;
		}
		else
		{
			return uv + ", " + vu;
		}
	}

	/**
	 *	Applies the priority cast to the given arguments L and R.
	 */
	template <typename U_, typename V_>
	static std::string p_cast (const U_& u0, const V_& v0)
	{
		using namespace anta::dsel::util;
		const anta::aux::Variable<Model> u(u0);
		const anta::aux::Variable<Model> entries(v0);
		const std::string uv = x_cast(u, entries, priority_cast);
		const std::string vu = x_cast(entries, u, priority_cast);
		if (uv == vu)
		{
			return uv;
		}
		else
		{
			return uv + ", " + vu;
		}
	}

};

/**
 *	A specialization of the test fixture for the default ANTA model.
 */
template <int C_>
class test_cast<test_pair<anta::model<>, C_> >:
	public test_cast<test_pair<void, C_> >
{
	typedef test_cast<test_pair<anta::model<>, C_> > self;

protected:
	/**
	 *	Applies both left and right casts to the given arguments L and R.
	 */
	template <typename U_, typename V_>
	static std::string d_cast (const U_& u, const V_& entries)
	{
		using namespace anta::dsel::util;
		const std::string uv = self::x_cast(u, entries, left_cast);
		const std::string vu = self::x_cast(entries, u, right_cast);
		if (uv == vu)
		{
			return uv;
		}
		else
		{
			return uv + ", " + vu;
		}
	}

	/**
	 *	Applies the priority cast to the given arguments L and R.
	 */
	template <typename U_, typename V_>
	static std::string p_cast (const U_& u, const V_& entries)
	{
		using namespace anta::dsel::util;
		const std::string uv = self::x_cast(u, entries, priority_cast);
		const std::string vu = self::x_cast(entries, u, priority_cast);
		if (uv == vu)
		{
			return uv;
		}
		else
		{
			return uv + ", " + vu;
		}
	}

};

} // namespace

/**
 *	Test case instantiation point.
 *	@{ */

const test_case test_case::entries[] = {
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

TYPED_TEST_CASE_P(test_cast);

//
//	tests for left/right directed casts
//
#define D_CAST(u, v) this->d_cast(this->u, this->v)
#define P_CAST(u, v) this->p_cast(this->u, this->v)

TYPED_TEST_P(test_cast, directed_empty)
{
	EXPECT_EQ( "null",		D_CAST(n, n) );
	EXPECT_EQ( "null",		D_CAST(n, b) );
	EXPECT_EQ( "null",		D_CAST(n, i) );
	EXPECT_EQ( "null",		D_CAST(n, r) );
	EXPECT_EQ( "null",		D_CAST(n, s) );
	EXPECT_EQ( "null",		D_CAST(n, a) );
}

TYPED_TEST_P(test_cast, directed_boolean)
{
	EXPECT_EQ( "boolean",	D_CAST(b, n) );
	EXPECT_EQ( "boolean",	D_CAST(b, b) );
	EXPECT_EQ( "boolean",	D_CAST(b, i) );
	EXPECT_EQ( "boolean",	D_CAST(b, r) );
	EXPECT_EQ( "boolean",	D_CAST(b, s1) );
	ASSERT_THROW( D_CAST(b, s), std::bad_cast );
	ASSERT_THROW( D_CAST(b, a), std::bad_cast );
}

TYPED_TEST_P(test_cast, directed_integer)
{
	EXPECT_EQ( "integer",	D_CAST(i, n) );
	EXPECT_EQ( "integer",	D_CAST(i, b) );
	EXPECT_EQ( "integer",	D_CAST(i, i) );
	EXPECT_EQ( "integer",	D_CAST(i, r) );
	EXPECT_EQ( "integer",	D_CAST(i, s2) );
	ASSERT_THROW( D_CAST(i, s), std::bad_cast );
	ASSERT_THROW( D_CAST(i, a), std::bad_cast );
}

TYPED_TEST_P(test_cast, directed_real)
{
	EXPECT_EQ( "real",		D_CAST(r, n) );
	EXPECT_EQ( "real",		D_CAST(r, b) );
	EXPECT_EQ( "real",		D_CAST(r, i) );
	EXPECT_EQ( "real",		D_CAST(r, r) );
	EXPECT_EQ( "real",		D_CAST(r, s3) );
	ASSERT_THROW( D_CAST(r, s), std::bad_cast );
	ASSERT_THROW( D_CAST(r, a), std::bad_cast );
}

TYPED_TEST_P(test_cast, directed_string)
{
	EXPECT_EQ( "string",	D_CAST(s, n) );
	EXPECT_EQ( "string",	D_CAST(s, b) );
	EXPECT_EQ( "string",	D_CAST(s, i) );
	EXPECT_EQ( "string",	D_CAST(s, r) );
	EXPECT_EQ( "string",	D_CAST(s, s) );
	EXPECT_EQ( "string",	D_CAST(s, a) );
}

TYPED_TEST_P(test_cast, directed_array)
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

TYPED_TEST_P(test_cast, priority_empty)
{
	EXPECT_EQ( "null",		P_CAST(n, n) );
	EXPECT_EQ( "boolean",	P_CAST(n, b) );
	EXPECT_EQ( "integer",	P_CAST(n, i) );
	EXPECT_EQ( "real",		P_CAST(n, r) );
	EXPECT_EQ( "string",	P_CAST(n, s) );
	EXPECT_EQ( "array",		P_CAST(n, a) );
}

TYPED_TEST_P(test_cast, priority_boolean)
{
	EXPECT_EQ( "boolean",	P_CAST(b, n) );
	EXPECT_EQ( "boolean",	P_CAST(b, b) );
	EXPECT_EQ( "integer",	P_CAST(b, i) );
	EXPECT_EQ( "real",		P_CAST(b, r) );
	EXPECT_EQ( "string",	P_CAST(b, s) );
	EXPECT_EQ( "array",		P_CAST(b, a) );
}

TYPED_TEST_P(test_cast, priority_integer)
{
	EXPECT_EQ( "integer",	P_CAST(i, n) );
	EXPECT_EQ( "integer",	P_CAST(i, b) );
	EXPECT_EQ( "integer",	P_CAST(i, i) );
	EXPECT_EQ( "real",		P_CAST(i, r) );
	EXPECT_EQ( "string",	P_CAST(i, s) );
	EXPECT_EQ( "array",		P_CAST(i, a) );
}

TYPED_TEST_P(test_cast, priority_real)
{
	EXPECT_EQ( "real",		P_CAST(r, n) );
	EXPECT_EQ( "real",		P_CAST(r, b) );
	EXPECT_EQ( "real",		P_CAST(r, i) );
	EXPECT_EQ( "real",		P_CAST(r, r) );
	EXPECT_EQ( "string",	P_CAST(r, s) );
	EXPECT_EQ( "array",		P_CAST(r, a) );
}

TYPED_TEST_P(test_cast, priority_string)
{
	EXPECT_EQ( "string",	P_CAST(s, n) );
	EXPECT_EQ( "string",	P_CAST(s, b) );
	EXPECT_EQ( "string",	P_CAST(s, i) );
	EXPECT_EQ( "string",	P_CAST(s, r) );
	EXPECT_EQ( "string",	P_CAST(s, s) );
	EXPECT_EQ( "array",		P_CAST(s, a) );
}

TYPED_TEST_P(test_cast, priority_array)
{
	EXPECT_EQ( "array",		P_CAST(a, n) );
	EXPECT_EQ( "array",		P_CAST(a, b) );
	EXPECT_EQ( "array",		P_CAST(a, i) );
	EXPECT_EQ( "array",		P_CAST(a, r) );
	EXPECT_EQ( "array",		P_CAST(a, s) );
	EXPECT_EQ( "array",		P_CAST(a, a) );
}

REGISTER_TYPED_TEST_CASE_P(test_cast,
		directed_empty,
		directed_boolean,
		directed_integer,
		directed_real,
		directed_string,
		directed_array,
		priority_empty,
		priority_boolean,
		priority_integer,
		priority_real,
		priority_string,
		priority_array
);

typedef ::testing::Types<
	test_pair<M1, 0>, test_pair<M1, 1>, test_pair<M1, 2>, test_pair<M1, 3>, test_pair<M1, 4>,
	test_pair<M1, 5>, test_pair<M1, 6>, test_pair<M1, 7>, test_pair<M1, 8>, test_pair<M1, 9>,
	test_pair<M2, 0>, test_pair<M2, 1>, test_pair<M2, 2>, test_pair<M2, 3>, test_pair<M2, 4>,
	test_pair<M2, 5>, test_pair<M2, 6>, test_pair<M2, 7>, test_pair<M2, 8>, test_pair<M2, 9>,
	test_pair<M3, 0>, test_pair<M3, 1>, test_pair<M3, 2>, test_pair<M3, 3>, test_pair<M3, 4>,
	test_pair<M3, 5>, test_pair<M3, 6>, test_pair<M3, 7>, test_pair<M3, 8>, test_pair<M3, 9>
> Models;

INSTANTIATE_TYPED_TEST_CASE_P(cast, test_cast, Models);

/**	@} */
