#include <gtest/gtest.h>
#include <anta/core.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/string.hpp>
#include <anta/sas/end.hpp>
#include <anta/sas/test.hpp>
#include "utils/parsing.hpp"

/**
 *	The model to be tested.
 */
namespace {
struct Model: anta::model<> {};
}

/**
 *	Link the debug observer to the model.
 */
#if defined(DEBUG_PRINT)
ANTA_AUX_DEBUG_PRINT_OBSERVER(Model)
#endif

/**
 *	Test fixture: instantiates common elements such as acceptors and nodes of an
 *	acceptor network that is used for running the tests.
 */
class test_anta_core: public ::testing::Test
{
protected:
	// acceptors
	anta::UnconditionalRepeat<Model> pass;
	anta::sas::End<Model> end;
	anta::sas::Symbol<Model> pound;
	anta::sas::String<Model> alpha, omega, _12345;
	anta::sas::Test<Model> letters, lowercase, digits, punct;

	// nodes
	anta::Node<Model> entry, exit, term;

	// fixture construction
	test_anta_core ():
		pound ('#'),
		alpha ("alpha"), omega ("omega"), _12345 ("12345"),
		letters (std::isalpha, anta::sas::tfGreedy),
		lowercase (std::islower, anta::sas::tfGreedy),
		digits (std::isdigit, anta::sas::tfGreedy),
		punct (std::ispunct)
	{
	}

};

using namespace anta;

/**
 *	Test transitions of simple type.
 */
TEST_F(test_anta_core, transition_SS)
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

/**
 *	Test transitions of invocation type.
 */
TEST_F(test_anta_core, transition_IS)
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

/**
 *	Test transitions of positive type (positive assumption).
 */
TEST_F(test_anta_core, transition_PSS)
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

/**
 *	Test transitions of negative type (negative assumption).
 */
TEST_F(test_anta_core, transition_NS)
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

/**
 *	Test two sequential transitions of invocation type.
 */
TEST_F(test_anta_core, transition_IISS)
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

/**
 *	Test sequence of invokation and positive type transitions.
 */
TEST_F(test_anta_core, transition_IPSS)
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

/**
 *	Test sequence of invokation and negative type transitions.
 */
TEST_F(test_anta_core, transition_INS)
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

/**
 *	Test sequence of two positive type transitions after an invocation.
 */
TEST_F(test_anta_core, transition_IPPSS)
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

/**
 *	Test sequence of positive and negative type transitions after an invocation.
 */
TEST_F(test_anta_core, transition_IPNSS)
{
	sas::String<Model> alpha1("ALPHA"), omega1("OMEGA");

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

/**
 *	Test sequence of negative and positive type transitions after an invocation.
 */
TEST_F(test_anta_core, transition_INPSS)
{
	sas::String<Model> alpha1("ALPHA"), omega1("OMEGA");

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

/**
 *	Test sequence of two negative transitions.
 */
TEST_F(test_anta_core, transition_NNSSS)
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

/**
 *	Test sequence of two negative transitions followed by an invocation.
 */
TEST_F(test_anta_core, transition_NNIS)
{
	// define additional node chains
	NodeChain<Model> ch1(4, 100), ch2(6, 200), ch3(2, 300);

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
