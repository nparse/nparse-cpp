#ifndef SRC_TEST_DSEL_HPP_
#define SRC_TEST_DSEL_HPP_

#include <anta/dsel.hpp>
#include <gtest/gtest.h>

/**
 *	The models to be tested.
 */
namespace {

struct M1: public anta::model<anta::meta::narrow> {};

struct M2: public anta::model<anta::meta::wide> {};

} // namespace

/**
 *	Necessary specializations.
 */
namespace anta {

#if 0	// it's not necessary since the default string type is std::string,
		// hence internal test data will be handled correctly
template <>
struct string<M1>
{
	typedef encode::wrapper<std::basic_string<character<M1>::type> > type;

};
#endif

template <>
struct string<M2>
{
	typedef encode::wrapper<std::basic_string<character<M2>::type> > type;

};

namespace ndl {

template <>
struct context_value<M1>
{
	typedef aux::Variable<M1> type;

};

template <>
struct context_value<M2>
{
	typedef aux::Variable<M2> type;

};

}} // namespace anta::ndl

// (forward declaration)
template <typename M_> class TestDsel;

template <typename M_>
struct TestDselEntry
{
	typedef M_ M;
	typedef anta::aux::Variable<M_> variable;

	char func;
	bool throws;
	variable x, y, z;

};

template <typename M_>
class TestDsel:
	public TestDselEntry<M_>,
	public ::testing::TestWithParam<TestDselEntry<M_> >
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

};

/**
 *	Makes GTest able to print test entries correctly.
 *	@{ */
template <typename M_>
void PrintTo (const TestDselEntry<M_>& a_entry, ::std::ostream* a_stream)
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

#endif /* SRC_TEST_DSEL_HPP_ */
