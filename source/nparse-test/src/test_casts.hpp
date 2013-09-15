#ifndef SRC_TEST_CASTS_HPP_
#define SRC_TEST_CASTS_HPP_

#include <boost/mpl/if.hpp>
#include <anta/dsel.hpp>
#include <gtest/gtest.h>

/**
 *	The models to be tested.
 */
namespace {

// NOTE: Since there is no tenable way to obtain the model for native types, the
//		 corresponding test case MUST use the default ANTA model.
typedef anta::model<> M1;

struct M2: public anta::model<anta::meta::narrow> {};

struct M3: public anta::model<anta::meta::wide> {};

} // namespace

/**
 *	Necessary specializations.
 */
namespace anta {

#if 0	// it's not necessary since the default string type is std::string,
		// hence internal test data will be handled correctly
template <>
struct string<M2>
{
	typedef encode::wrapper<std::basic_string<character<M2>::type> > type;

};
#endif

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

}} // namespace anta::ndl

/**
 *	The test tuple.
 */
struct TestEntry
{
	bool			b;
	int				i;
	double			r;
	const char*		s;
	const char*		s1;
	const char*		s2;
	const char*		s3;

	static const TestEntry entries[10];

};

/**
 *	An auxiliary structure that is used to combine models and test tuples.
 */
template <typename M_, int C_>
struct Pair
{
	typedef M_ Model;
	static const int Case = C_;

};

/**
 *	Probe functor: returns a string containing comma-separated tags of the
 *	reduced types, either downcasted or upcasted.
 */
struct Probe
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
 *	Test fixture: instantiates common elements that are used in type casting.
 */
template <typename P_>
class TestCast: public ::testing::Test
{
protected:
	typedef typename boost::mpl::if_<boost::is_same<typename P_::Model, void>,
			anta::model<>, typename P_::Model>::type Model;

	typename anta::aux::null<Model>::type		n;
	typename anta::aux::boolean<Model>::type	b;
	typename anta::aux::integer<Model>::type	i;
	typename anta::aux::real<Model>::type		r;
	typename anta::string<Model>::type			s, s1, s2, s3;
	typename anta::aux::array<Model>::type		a;

	TestCast ():
		n	(),
		b	(TestEntry::entries[P_::Case].b),
		i	(TestEntry::entries[P_::Case].i),
		r	(TestEntry::entries[P_::Case].r),
		s	(TestEntry::entries[P_::Case].s),
		s1	(TestEntry::entries[P_::Case].s1),
		s2	(TestEntry::entries[P_::Case].s2),
		s3	(TestEntry::entries[P_::Case].s3),
		a	(new anta::ndl::Context<Model>())
	{
	}

	/**
	 *	Applies specified cast C to the given arguments L and R and sends its
	 *	values to the inner probe functor.
	 */
	template <typename L_, typename R_, typename C_>
	static std::string test_cast (const L_& a_left, const R_& a_right,
			const C_& a_cast)
	{
		return a_cast(a_left, a_right, Probe());
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
		const std::string uv = test_cast(u, entries, left_cast);
		const std::string vu = test_cast(entries, u, right_cast);
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
	 *	Applies priority cast to the given arguments L and R.
	 */
	template <typename U_, typename V_>
	static std::string p_cast (const U_& u0, const V_& v0)
	{
		using namespace anta::dsel::util;
		const anta::aux::Variable<Model> u(u0);
		const anta::aux::Variable<Model> entries(v0);
		const std::string uv = test_cast(u, entries, priority_cast);
		const std::string vu = test_cast(entries, u, priority_cast);
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

template <int C_>
class TestCast<Pair<anta::model<>, C_> >: public TestCast<Pair<void, C_> >
{
	typedef TestCast<Pair<anta::model<>, C_> > self;

protected:
	/**
	 *	Applies both left and right casts to the given arguments L and R.
	 */
	template <typename U_, typename V_>
	static std::string d_cast (const U_& u, const V_& entries)
	{
		using namespace anta::dsel::util;
		const std::string uv = self::test_cast(u, entries, left_cast);
		const std::string vu = self::test_cast(entries, u, right_cast);
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
	 *	Applies priority cast to the given arguments L and R.
	 */
	template <typename U_, typename V_>
	static std::string p_cast (const U_& u, const V_& entries)
	{
		using namespace anta::dsel::util;
		const std::string uv = self::test_cast(u, entries, priority_cast);
		const std::string vu = self::test_cast(entries, u, priority_cast);
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

TYPED_TEST_CASE_P(TestCast);

#endif /* SRC_TEST_CASTS_HPP_ */
