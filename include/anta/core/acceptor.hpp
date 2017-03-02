/*
 * @file $/include/anta/core/acceptor.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@nparse.com>
 
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
#ifndef ANTA_CORE_ACCEPTOR_HPP_
#define ANTA_CORE_ACCEPTOR_HPP_

namespace anta {

/******************************************************************************/

// (forward declaration)
template <typename M_> class Traveller;

/**
 *	Default spectrum type.
 */
template <typename M_>
struct spectrum
{
	typedef Traveller<M_> type;

};

/**
 *	Acceptor interface.
 */
template <typename M_>
class Acceptor: public Base<Acceptor<M_>, M_>
{
public:
	/**
	 *	The polymorphic destructor.
	 */
	virtual ~Acceptor () {}

	/**
	 *	Perform an acceptation procedure on the given element @E in the given
	 *	context @C and store the obtained eigenvalues within the spectrum @S.
	 */
	virtual void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const = 0;

};

/******************************************************************************/

/**
 *	Default unconditional repeating acceptor.
 */
template <typename M_>
class UnconditionalRepeat: public Acceptor<M_>
{
public:
	UnconditionalRepeat ()
	{
	}

	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		// Push the last accepted element.
		S. push(E. first, E. second);
	}

};

/**
 *	Default unconditional shifting acceptor.
 */
template <typename M_>
class UnconditionalShift: public Acceptor<M_>
{
public:
	UnconditionalShift ()
	{
	}

	// Overridden from Acceptor<M_>:

	void accept (const typename range<M_>::type& C,
			const typename range<M_>::type& E, typename spectrum<M_>::type& S)
		const
	{
		// Push a zero sized element at the current position.
		S. push(E. second, E. second);
	}

};

/**
 *	Generator function for the unconditional acceptors.
 */
template <typename M_>
inline const Acceptor<M_>& unconditional (const bool a_repeating = true)
{
	static const UnconditionalRepeat<M_> sc_repeat = UnconditionalRepeat<M_>();
	static const UnconditionalShift<M_> sc_shift = UnconditionalShift<M_>();
	if (a_repeating)
		return sc_repeat;
	else
		return sc_shift;
}

/******************************************************************************/

} // namespace anta

#endif /* ANTA_CORE_ACCEPTOR_HPP_ */
