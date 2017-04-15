/*
 * @file $/include/anta/_aux/variable.hpp
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
#ifndef ANTA_AUX_VARIABLE_HPP_
#define ANTA_AUX_VARIABLE_HPP_

#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <encode/encode.hpp>
#include "lister.hpp"

namespace anta { namespace aux {

// (forward declaration)
template <typename M_> class Variable;

/**
 *	Type identifier.
 */
struct type_id { enum type {
	unknown,
	dynamic,
	null,
	boolean,
	integer,
	real,
	string,
	array
}; };

/**
 *	Set of overridable wrappers for model-dependent static types.
 *	@{ */

/// Unified implementation of the null type to be shared among all models.
struct unified_null_type {};

inline bool operator== (unified_null_type, unified_null_type) { return true; }
inline bool operator!= (unified_null_type, unified_null_type) { return false; }

/// Default null type.
template <typename M_>
struct null
{
	typedef unified_null_type type;

};

/// Default boolean type.
template <typename M_>
struct boolean
{
	typedef bool type;

};

/// Default integer type.
template <typename M_>
struct integer
{
	typedef long type;

};

/// Default real (floating point) type.
template <typename M_>
struct real
{
	typedef double type;

};

// NOTE: We assume that a model-dependent string type has been defined already.

/// Associative array type.
template <typename M_>
struct array
{
	typedef typename ndl::context<M_>::type type;

};

/**	@} */

/**
 *	Metafunction that converts static types to the correpsonding wrappers in the
 *	context of a given model.
 *	@{ */

template <typename M_, typename T_>
struct wrapper;

template <typename M_>
struct wrapper<M_, typename null<M_>::type>
{
	typedef null<M_> type;

};

template <typename M_>
struct wrapper<M_, typename boolean<M_>::type>
{
	typedef boolean<M_> type;

};

template <typename M_>
struct wrapper<M_, typename integer<M_>::type>
{
	typedef integer<M_> type;

};

template <typename M_>
struct wrapper<M_, typename real<M_>::type>
{
	typedef real<M_> type;

};

template <typename M_>
struct wrapper<M_, typename string<M_>::type>
{
	typedef string<M_> type;

};

template <typename M_>
struct wrapper<M_, typename array<M_>::type>
{
	typedef array<M_> type;

};

/**	@} */

/**
 *	Type identification mechanism.
 *	@{ */

namespace id_ {

// (default implementation for unspecialized types)
template <typename M_, typename T_>
struct impl
{
	static type_id::type value () { return type_id::unknown; }
	template <typename X_> static type_id::type value (X_&) { return value(); }

};

// (specializations for types resolved in compile-time)
#define ANTA_AUX_VARIABLE_DEFTAG(TYPE)								\
	template <typename M_> struct impl<M_, TYPE<M_> > {				\
		static type_id::type value () { return type_id::TYPE; }		\
		template <typename X_> static type_id::type value (X_&) {	\
			return value(); } };
ANTA_AUX_VARIABLE_DEFTAG(null)
ANTA_AUX_VARIABLE_DEFTAG(boolean)
ANTA_AUX_VARIABLE_DEFTAG(integer)
ANTA_AUX_VARIABLE_DEFTAG(real)
ANTA_AUX_VARIABLE_DEFTAG(string)
ANTA_AUX_VARIABLE_DEFTAG(array)
#undef ANTA_AUX_VARIABLE_DEFTAG

// (workaround specialization for arrays)
template <typename M_>
struct impl<M_, typename array<M_>::type>
{
	static type_id::type value () { return type_id::array; }
	template <typename X_> static type_id::type value (X_&) { return value(); }

};

// (handles unwrapped static types in compile-time)
template <typename T_, typename M_ = typename model_of<T_>::type>
struct of: impl<M_, typename wrapper<M_, T_>::type> {};

// (handles wrapped model-dependent static types in compile-time)
template <typename M_, template <typename M2_> class Type_>
struct of<Type_<M_> >: impl<M_, Type_<M_> > {};

// (handles dynamic types in realtime)
template <typename M_>
struct of<Variable<M_> >
{
	static type_id::type value () { return type_id::dynamic; }
	static type_id::type value (const Variable<M_>& a_v) {
		return a_v. value(); }

};

// (handles standard strings)
template <typename C_, typename T_, typename A_>
struct of<std::basic_string<C_, T_, A_> >
{
	static type_id::type value () { return type_id::string; }
	template <typename X_> static type_id::type value (X_) { return value(); }

};

// (handles wrapped standard strings)
template <typename C_, typename T_, typename A_>
struct of<encode::wrapper<std::basic_string<C_, T_, A_> > >
{
	static type_id::type value () { return type_id::string; }
	template <typename X_> static type_id::type value (X_) { return value(); }

};

} // namespace id_

template <typename V_>
inline type_id::type id (const V_& a_v)
{
	return id_::of<V_>::value(a_v);
}

template <typename V_>
inline const char* tag (const V_& a_v)
{
	return tag(id(a_v));
}

inline const char* tag (const type_id::type a_id)
{
	switch (a_id)
	{
#define ANTA_AUX_VARIABLE_TAG(tag) case type_id::tag: return #tag;
	ANTA_AUX_VARIABLE_TAG(unknown)
	ANTA_AUX_VARIABLE_TAG(dynamic)
	ANTA_AUX_VARIABLE_TAG(null)
	ANTA_AUX_VARIABLE_TAG(boolean)
	ANTA_AUX_VARIABLE_TAG(integer)
	ANTA_AUX_VARIABLE_TAG(real)
	ANTA_AUX_VARIABLE_TAG(string)
	ANTA_AUX_VARIABLE_TAG(array)
#undef ANTA_AUX_VARIABLE_TAG
	default: return "~";
	}
}

/** @} */

/**
 *	Proxy container, owns instances of static types.
 *	@{ */
template <typename Wrapper_>
class Proxy
{
public:
	typedef typename Wrapper_::type value_type;

	Proxy ()
	{
	}

	Proxy (const value_type& a_value):
		m_value (a_value)
	{
	}

	const Proxy& operator= (const value_type& a_value)
	{
		m_value = a_value;
		return *this;
	}

	const value_type& value () const
	{
		return m_value;
	}

	value_type& value ()
	{
		return m_value;
	}

	static type_id::type id ()
	{
		return id_::of<Wrapper_>::value();
	}

	static const char* tag ()
	{
		return aux::tag(id());
	}

private:
	value_type m_value;

};

/**
 *	Specialization of the Proxy container for the empty type.
 */
template <typename M_>
class Proxy<null<M_> >
{
public:
	typedef typename null<M_>::type value_type;

	Proxy ()
	{
	}

	template <typename T_>
	Proxy (const T_&)
	{
	}

	value_type value () const
	{
		return value_type();
	}

	static type_id::type id ()
	{
		return id_::of<null<M_> >::value();
	}

	static const char* tag ()
	{
		return aux::tag(id());
	}

};

/**	@} */

/**
 *	Dynamic type container.
 */
template <typename M_>
struct container
{
	typedef boost::variant<
		Proxy<null<M_> >
	,	Proxy<boolean<M_> >
	,	Proxy<integer<M_> >
	,	Proxy<real<M_> >
	,	Proxy<string<M_> >
	,	Proxy<array<M_> >
	> type;

};

/**
 *	Static proxy factory, simplifies proxy instantiation.
 */
template <typename M_>
class proxy
{
public:
	template <typename T_>
	static Proxy<typename wrapper<M_, T_>::type> from (const T_& a_t)
	{
		// (implies an implicit invocation of the Proxy class constructor)
		return a_t;
	}

	template <typename C_, typename T_, typename A_>
	static Proxy<string<M_> > from (const std::basic_string<C_, T_, A_>& a_t)
	{
		return encode::make<typename string<M_>::type>::from(a_t);
	}

};

/**
 *	Dynamic type derivation mechanism.
 */
template <typename M_>
class derive: public boost::static_visitor<void>
{
public:
	derive (typename container<M_>::type& a_container):
		m_container (a_container)
	{
	}

	template <typename T_>
	void operator() (const Proxy<T_>& a_ancestor) const
	{
		m_container = proxy<M_>::from(a_ancestor. value());
	}

	void operator() (const Proxy<array<M_> >& a_ancestor) const
	{
		m_container = proxy<M_>::from(typename array<M_>::type(
					&* a_ancestor. value()));
	}

private:
	typename container<M_>::type& m_container;

};

/**
 *	Dynamic type identification mechanism.
 *	@{ */

namespace check {

/**
 *	Identifies the current type of a container.
 */
template <typename T_>
class is: public boost::static_visitor<bool>
{
public:
	bool operator() (const T_& t) const
	{
		// If the types T_ and S_ are equal then return true.
		return true;
	}

	template <typename S_>
	bool operator() (S_) const
	{
		// Otherwise, if the types don't match then return false.
		return false;
	}

};

/**
 *	Returns the type identifier of a container.
 */
template <typename M_>
class id: public boost::static_visitor<type_id::type>
{
public:
	template <typename Proxy_>
	type_id::type operator() (const Proxy_& a_proxy) const
	{
		return a_proxy. id();
	}

};

} // namespace check

/**	@} */

/**
 *	Dynamic type conversion mechanism.
 *	@{ */

namespace cast {

/**
 *	Expecting cast (default).
 *	Returns the stored value if its type matches the template argument, and
 *	throws std::bad_cast exception otherwise.
 */
template <typename T_>
class as: public boost::static_visitor<T_>
{
public:
	template <typename S_>
	T_ operator() (S_) const
	{
		// Arbitrary unequal types T_ and S_ are not conversible by default.
		throw std::bad_cast();
	}

	const T_& operator() (const T_& a_t) const
	{
		// If the types T_ and S_ are equal then there's nothing special to do.
		return a_t;
	}

};

/**
 *	Voiding cast.
 *	Returns a null value regardless of the stored type.
 */
template <typename M_>
class as<Proxy<null<M_> > >: public boost::static_visitor<Proxy<null<M_> > >
{
public:
	template <typename S_>
	Proxy<null<M_> > operator() (S_) const
	{
		// Any type can be easily converted to the null type.
		return Proxy<null<M_> >();
	}

};

/**
 *	Boolean cast.
 *	Tries to interpret the stored value as a boolean value.
 */
template <typename M_>
class as<Proxy<boolean<M_> > >:
	public boost::static_visitor<Proxy<boolean<M_> > >
{
public:
	template <typename S_>
	Proxy<boolean<M_> > operator() (S_) const
	{
		throw std::bad_cast();
	}

	Proxy<boolean<M_> > operator() (const Proxy<null<M_> >&) const
	{
		return false;
	}

	const Proxy<boolean<M_> >& operator() (const Proxy<boolean<M_> >& a_v) const
	{
		return a_v;
	}

	Proxy<boolean<M_> > operator() (const Proxy<integer<M_> >& a_v) const
	{
		return (a_v. value() != 0);
	}

	Proxy<boolean<M_> > operator() (const Proxy<real<M_> >& a_v) const
	{
		return (a_v. value() != 0.0);
	}

	Proxy<boolean<M_> > operator() (const Proxy<string<M_> >& a_v) const
	{
#define ANTA_AUX_VARIABLE_MAKE_CONST(TAG) \
		static const typename string<M_>::type c_##TAG = \
			encode::make<typename string<M_>::type>::from(#TAG);
		ANTA_AUX_VARIABLE_MAKE_CONST(no)
		ANTA_AUX_VARIABLE_MAKE_CONST(yes)
		ANTA_AUX_VARIABLE_MAKE_CONST(true)
		ANTA_AUX_VARIABLE_MAKE_CONST(false)
#undef ANTA_AUX_VARIABLE_MAKE_CONST

		// @todo: the following solution is not cheap enough
		switch (a_v. value(). size())
		{
		case 0:
			return false;

		case 1:
			switch (encode::make<char>::from(a_v. value()[0]))
			{
			case '1': case 't': case 'T': case 'y': case 'Y':
				return true;
			case '0': case 'f': case 'F': case 'n': case 'N':
				return false;
			}
			break;

		case 2:
			if (boost::to_lower_copy(a_v. value()) == c_no)
				return false;
			break;

		case 3:
			if (boost::to_lower_copy(a_v. value()) == c_yes)
				return true;
			break;

		case 4:
			if (boost::to_lower_copy(a_v. value()) == c_true)
				return true;
			break;

		case 5:
			if (boost::to_lower_copy(a_v. value()) == c_false)
				return false;
			break;
		}

		throw std::bad_cast();
	}

};

/**
 *	Integer cast.
 *	Tries to interpret the stored value as an integer value.
 */
template <typename M_>
class as<Proxy<integer<M_> > >:
	public boost::static_visitor<Proxy<integer<M_> > >
{
public:
	template <typename S_>
	Proxy<integer<M_> > operator() (S_) const
	{
		throw std::bad_cast();
	}

	Proxy<integer<M_> > operator() (const Proxy<null<M_> >&) const
	{
		return 0;
	}

	Proxy<integer<M_> > operator() (const Proxy<boolean<M_> >& a_v) const
	{
		return (a_v. value() ? 1 : 0);
	}

	const Proxy<integer<M_> >& operator() (const Proxy<integer<M_> >& a_v) const
	{
		return a_v;
	}

	Proxy<integer<M_> > operator() (const Proxy<real<M_> >& a_v) const
	{
		return static_cast<typename integer<M_>::type>(a_v. value());
	}

	Proxy<integer<M_> > operator() (const Proxy<string<M_> >& a_v) const
	{
		return boost::lexical_cast<typename integer<M_>::type>(
				a_v. value(). c_str());
	}

};

/**
 *	Real cast.
 *	Tries to interpret the stored value as a real value.
 */
template <typename M_>
class as<Proxy<real<M_> > >: public boost::static_visitor<Proxy<real<M_> > >
{
public:
	template <typename S_>
	Proxy<real<M_> > operator() (S_) const
	{
		throw std::bad_cast();
	}

	Proxy<real<M_> > operator() (const Proxy<null<M_> >&) const
	{
		return 0.0;
	}

	Proxy<real<M_> > operator() (const Proxy<boolean<M_> >& a_v) const
	{
		return (a_v. value() ? 1.0 : 0.0);
	}

	Proxy<real<M_> > operator() (const Proxy<integer<M_> >& a_v) const
	{
		return static_cast<typename real<M_>::type>(a_v. value());
	}

	const Proxy<real<M_> >& operator() (const Proxy<real<M_> >& a_v) const
	{
		return a_v;
	}

	Proxy<real<M_> > operator() (const Proxy<string<M_> >& a_v) const
	{
		return boost::lexical_cast<typename real<M_>::type>(
				a_v. value(). c_str());
	}

};

/**
 *	Lexical cast.
 *	Returns a text representation of the stored value.
 */
template <typename M_>
class as<Proxy<string<M_> > >: public boost::static_visitor<Proxy<string<M_> > >
{
public:
	template <typename S_>
	Proxy<string<M_> > operator() (S_) const
	{
		throw std::bad_cast();
	}

	Proxy<string<M_> > operator() (const Proxy<null<M_> >&) const
	{
		return typename string<M_>::type();
	}

	Proxy<string<M_> > operator() (const Proxy<boolean<M_> >& a_v) const
	{
		return encode::make<typename string<M_>::type>::from(
				a_v. value() ? "true" : "false");
	}

	Proxy<string<M_> > operator() (const Proxy<integer<M_> >& a_v) const
	{
		std::basic_stringstream<typename string<M_>::type::value_type> tmp;
		tmp << a_v. value();
		return typename string<M_>::type(tmp. str());
	}

	Proxy<string<M_> > operator() (const Proxy<real<M_> >& a_v) const
	{
		std::basic_stringstream<typename string<M_>::type::value_type> tmp;
		tmp << std::scientific << std::setprecision(6) << a_v. value();
		return typename string<M_>::type(tmp. str());
	}

	const Proxy<string<M_> >& operator() (const Proxy<string<M_> >& a_v) const
	{
		return a_v;
	}

	Proxy<string<M_> > operator() (const Proxy<array<M_> >& a_v) const
	{
		return encode::make<typename string<M_>::type>::from(
				list(* a_v. value(), false));
	}

};

/**
 *	Array cast.
 *	Returns an associative array containing the source value associated with the
 *	0th element.
 */
template <typename M_>
class as<Proxy<array<M_> > >: public boost::static_visitor<Proxy<array<M_> > >
{
public:
	Proxy<array<M_> > operator() (const Proxy<null<M_> >&) const
	{
		return typename array<M_>::type();
	}

	Proxy<array<M_> > operator() (const Proxy<array<M_> >& a_v) const
	{
		return a_v;
	}

	template <typename Wrapper_>
	Proxy<array<M_> > operator() (const Proxy<Wrapper_>& a_t) const
	{
		static const typename ndl::context_key<M_>::type index =
			typename ndl::context_key<M_>::type();
		typename array<M_>::type r = typename array<M_>::type();
		r -> ref(index) = a_t. value();
		return r;
	}

};

} // namespace cast

/**	@} */

/**
 *	Variable, the main class that does dynamic value handling.
 */
template <typename M_>
class Variable
{
public:
	/**
	 *	Default constructor.
	 */
	Variable ():
		m_container (Proxy<null<M_> >())
	{
	}

private:
	/**
	 *	Auxiliary constructor for inner purposes.
	 */
	Variable (const typename container<M_>::type& a_container):
		m_container (a_container)
	{
	}

public:
	/**
	 *	Value copying operator (copy constructor).
	 */
	Variable (const Variable& a_ancestor):
		m_container (a_ancestor. m_container)
	{
	}

	/**
	 *	Value derivation operator (assignment operator).
	 */
	Variable& operator= (const Variable& a_ancestor)
	{
		if (this != &a_ancestor)
		{
			boost::apply_visitor(derive<M_>(m_container),
					a_ancestor. m_container);
		}
		return *this;
	}

	/**
	 *	Exchange contents of two variables.
	 */
	void swap (Variable& a_ancestor)
	{
		m_container. swap(a_ancestor. m_container);
	}

public:
	/**
	 *	Generic only argument constructor.
	 */
	template <typename First_>
	Variable (const First_& a_first):
		m_container (Proxy<null<M_> >())
	{
		*this = a_first; // This is a lazy trick that keeps it necessary to
						 // overload assignment operator only.
	}

	/**
	 *	Generic assignent operator.
	 *	@{ */

	template <typename First_>
	Variable& operator= (const First_& a_first)
	{
		m_container = proxy<M_>::from(a_first);
		return *this;
	}

	template <typename T_>
	Variable& operator= (const Proxy<T_>& a_first)
	{
		m_container = a_first;
		return *this;
	}

	/**	@} */

	/**
	 *	Narrow string constant assignment operator.
	 */
	Variable& operator= (const char* a_value)
	{
		m_container = proxy<M_>::from(
				encode::make<typename string<M_>::type>::from(a_value));
		return *this;
	}

	/**
	 *	Wide string constant assignment operator.
	 */
	Variable& operator= (const wchar_t* a_value)
	{
		m_container = proxy<M_>::from(
				encode::make<typename string<M_>::type>::from(a_value));
		return *this;
	}

	/**
	 *	Generic two argument string type constructor.
	 */
	template <typename First_, typename Second_>
	Variable (const First_& a_first, const Second_& a_second):
		m_container (proxy<M_>::from(
					typename string<M_>::type(a_first, a_second)))
	{
	}

public:
	/**
	 *	Checks whether the stored value is of the given type.
	 *	@{ */

	template <typename Proxy_>
	bool is () const
	{
		return boost::apply_visitor(check::is<Proxy_>(), m_container);
	}

	bool is_null () const
	{
		return is<Proxy<null<M_> > >();
	}

	bool is_boolean () const
	{
		return is<Proxy<boolean<M_> > >();
	}

	bool is_integer () const
	{
		return is<Proxy<integer<M_> > >();
	}

	bool is_real () const
	{
		return is<Proxy<real<M_> > >();
	}

	bool is_string () const
	{
		return is<Proxy<string<M_> > >();
	}

	bool is_array () const
	{
		return is<Proxy<aux::array<M_> > >();
	}

	/**	@} */

	/**
	 *	Returns the type identifier of the stored value.
	 */
	type_id::type id () const
	{
		return boost::apply_visitor(check::id<M_>(), m_container);
	}

	/**
	 *	Retuns the type tag of the stored value.
	 */
	const char* tag () const
	{
		return aux::tag(this->id());
	}

public:
	/**
	 *	Converts the stored value to the given type.
	 *	@{ */

	template <typename R_>
	R_ as () const
	{
		return boost::apply_visitor(
				cast::as<Proxy<typename wrapper<M_, R_>::type> >(), m_container
		). value();
	}

	typename boolean<M_>::type as_boolean () const
	{
		return as<typename boolean<M_>::type>();
	}

	typename integer<M_>::type as_integer () const
	{
		return as<typename integer<M_>::type>();
	}

	typename real<M_>::type as_real () const
	{
		return as<typename real<M_>::type>();
	}

	typename string<M_>::type as_string () const
	{
		return as<typename string<M_>::type>();
	}

	typename array<M_>::type as_array () const
	{
		return as<typename aux::array<M_>::type>();
	}

	/**	@} */

public:
	/**
	 *	Voids the stored value.
	 */
	void clear ()
	{
		m_container = Proxy<null<M_> >();
	}

	/**
	 *	Makes the variable an associative array.
	 *
	typename aux::array<M_>::type array()
	{
		if (! is_array())
			m_container = proxy<M_>::from(typename aux::array<M_>::type());
		return this -> as_array();
	}
	 */

private:
	/**
	 *	Helper: sets the order of the arguments passed to an outer functor.
	 *	@{ */

	template <bool InverseOrder_, typename Void_ = void>
	struct invoke
	{
	};

	template <typename Void_>
	struct invoke<false, Void_>
	{
		template <typename U_, typename V_, typename F_>
		static typename F_::result_type f (const U_& u, const V_& v,
				const F_& f)
		{
			return f(u, v);
		}

	};

	template <typename Void_>
	struct invoke<true, Void_>
	{
		template <typename U_, typename V_, typename F_>
		static typename F_::result_type f (const U_& u, const V_& v,
				const F_& f)
		{
			return f(v, u);
		}

	};

	/**	@} */

private:
	/**
	 *	Helper:	invokes an outer functor with preceding argument transform.
	 *	@{ */

	//
	//	binary, using cast: f( the stored value, a native type )
	//
	template <typename F_, typename R_ = void, bool UseCast_ = true,
			 bool InverseOrder_ = false>
	class applicant: // <default implementation>
		public boost::static_visitor<typename F_::result_type>
	{
	public:
		template <typename Proxy_>
		typename F_::result_type operator() (const Proxy_& p) const
		{
			return invoke<InverseOrder_>::f(
					p. value(),
					cast::as<Proxy_>()(proxy<M_>::from(m_right)). value(),
					m_f);
		}

		applicant (const F_& a_f, const R_& a_right):
			m_f (a_f), m_right (a_right)
		{
		}

	private:
		const F_& m_f;
		const R_& m_right;

	};

	//
	//	binary, NOT using cast: f( the stored value, a native type )
	//
	template <typename F_, typename R_, bool InverseOrder_>
	class applicant<F_, R_, false, InverseOrder_>:
		public boost::static_visitor<typename F_::result_type>
	{
	public:
		template <typename Proxy_>
		typename F_::result_type operator() (const Proxy_& p) const
		{
			return invoke<InverseOrder_>::f(
					p. value(),
					m_right,
					m_f);
		}

		applicant (const F_& a_f, const R_& a_right):
			m_f (a_f), m_right (a_right)
		{
		}

	private:
		const F_& m_f;
		const R_& m_right;

	};

	/* NOTE: The M2_ template parameter is a workaround for a name lookup issue
	 * 		 originally observed in
	 *			GCC version 4.2.1 (Apple Inc. build 5666) (dot 3)
	 */

	//
	//	binary, using cast: f( the stored value, a variable )
	//
	template <typename F_, typename M2_, bool InverseOrder_>
	class applicant<F_, Variable<M2_>, true, InverseOrder_>:
		public boost::static_visitor<typename F_::result_type>
	{
	public:
		template <typename Proxy_>
		typename F_::result_type operator() (const Proxy_& p) const
		{
			return invoke<InverseOrder_>::f(
					p. value(),
					m_right. as<typename Proxy_::value_type>(),
					m_f);
		}

		applicant (const F_& a_f, const Variable& a_right):
			m_f (a_f), m_right (a_right)
		{
		}

	private:
		const F_& m_f;
		const Variable& m_right;

	};

	//
	//	binary, NOT using cast: f( the stored value, a variable )
	//
	template <typename F_, typename M2_, bool InverseOrder_>
	class applicant<F_, Variable<M2_>, false, InverseOrder_>:
		public boost::static_visitor<typename F_::result_type>
	{
		template <typename ProxyLeft_>
		class helper: public boost::static_visitor<typename F_::result_type>
		{
		public:
			helper (const F_& a_f, const ProxyLeft_& a_left):
				m_f (a_f), m_left (a_left)
			{
			}

			template <typename ProxyRight_>
			typename F_::result_type operator() (const ProxyRight_& a_right)
				const
			{
				return invoke<InverseOrder_>::f(
						m_left. value(),
						a_right. value(),
						m_f);
			}

		private:
			const F_& m_f;
			const ProxyLeft_& m_left;

		};

	public:
		template <typename ProxyLeft_>
		typename F_::result_type operator() (const ProxyLeft_& a_left) const
		{
			return boost::apply_visitor(helper<ProxyLeft_>(m_f, a_left),
					m_right. m_container);
		}

		applicant (const F_& a_f, const Variable& a_right):
			m_f (a_f), m_right (a_right)
		{
		}

	private:
		const F_& m_f;
		const Variable& m_right;

	};

	//
	//	unary: f( the stored value )
	//
	template <typename F_, bool UseCast_, bool InverseOrder_>
	class applicant<F_, void, UseCast_, InverseOrder_>:
		public boost::static_visitor<typename F_::result_type>
	{
	public:
		template <typename Proxy_>
		typename F_::result_type operator() (Proxy_& p) const
		{
			return m_f(p. value());
		}

		applicant (const F_& a_f):
			m_f (a_f)
		{
		}

	private:
		const F_& m_f;

	};

	/**	@} */

public:
	/**
	 *	Invokes the given functor with the stored value passed as the only
	 *	argument.
	 *	@{	*/
	template <typename F_>
	typename F_::result_type apply (const F_& a_f) const
	{
		return boost::apply_visitor(applicant<F_>(a_f), m_container);
	}

	template <typename F_>
	typename F_::result_type applySelf (const F_& a_f)
	{
		return boost::apply_visitor(applicant<F_>(a_f), m_container);
	}
	/**	@} */

	/**
	 *	Invokes the given functor with the stored value passed as the first
	 *	(left) argument and the given value passed as the second (right)
	 *	argument.
	 */
	template <typename F_, typename R_>
	typename F_::result_type apply (const F_& a_f, const R_& a_right) const
	{
		return boost::apply_visitor(applicant<F_, R_, false>(a_f, a_right),
				m_container);
	}

	/**
	 *	Invokes the given functor with the stored value passed as the first
	 *	(left) argument and the given value passed as the second (right)
	 *	argument casting the second argument to the type of the stored value.
	 */
	template <typename F_, typename R_>
	typename F_::result_type applyCast (const F_& a_f, const R_& a_right) const
	{
		return boost::apply_visitor(applicant<F_, R_, true>(a_f, a_right),
				m_container);
	}

	/**
	 *	Invokes the given functor with the stored value passed as the second
	 *	(right) argument and the given value passed as the first (left)
	 *	argument.
	 */
	template <typename L_, typename F_>
	typename F_::result_type applyInverse (const F_& a_f, const L_& a_left)
		const
	{
		return boost::apply_visitor(applicant<F_, L_, false, true>(a_f, a_left),
				m_container);
	}

	/**
	 *	Invokes the given functor with the stored value passed as the second
	 *	(right) argument and the given value passed as the first (left)
	 *	argument casting the first argument to the type of the stored value.
	 */
	template <typename L_, typename F_>
	typename F_::result_type applyInverseCast (const F_& a_f, const L_& a_left)
		const
	{
		return boost::apply_visitor(applicant<F_, L_, true, true>(a_f, a_left),
				m_container);
	}

private:
	typename container<M_>::type m_container;

};

}} // namespace anta::aux

#endif /* ANTA_AUX_VARIABLE_HPP_ */
