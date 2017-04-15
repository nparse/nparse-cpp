/*
The MIT License (MIT)
Copyright (c) 2006-2010 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef ENCODE_ENCODE_HPP_
#define ENCODE_ENCODE_HPP_

#include <string>
#include <ostream>

namespace encode { namespace detail {

/**
 *	Auxiliary specializable string conversion functor.
 */
template <typename Char1_, typename Char2_>
class converter_string;

/**
 *	String conversion functor specialization: narrow to wide.
 */
template <>
class converter_string<wchar_t, char>
{
public:
	static std::wstring f (const std::string&);

};

/**
 *	String conversion functor specialization: wide to narrow.
 */
template <>
class converter_string<char, wchar_t>
{
public:
	static std::string f (const std::wstring&);

};

/**
 *	String conversion functor specialization: equal types.
 */
template <typename Char_>
class converter_string<Char_, Char_>
{
public:
	typedef std::basic_string<Char_> string;

	static const string& f (const string& s)
	{
		return s;
	}

};

/**
 *	Auxiliary specializable iterator range conversion functor.
 */
template <typename Char1_, typename Char2_>
class converter_range;

/**
 *	Range conversion functor specialization: narrow to wide.
 */
template <>
class converter_range<wchar_t, char>
{
public:
	static std::wstring f (const char* begin, const char* end);

};

/**
 *	Range conversion functor specialization: wide to narrow.
 */
template <>
class converter_range<char, wchar_t>
{
public:
	static std::string f (const wchar_t* begin, const wchar_t* end);

};

/**
 *	Range conversion functor specialization: equal types.
 */
template <typename Char_>
class converter_range<Char_, Char_>
{
public:
	typedef std::basic_string<Char_> string;

	template <typename Iter_>
	static string f (Iter_ begin, Iter_ end)
	{
		return string(begin, end);
	}

};

/**
 *	Auxiliary specializable generic conversion functor.
 */
template <typename Out_, typename In_>
class converter;

/**
 *	Generic conversion functor specialization: narrow to wide character.
 */
template <>
class converter<wchar_t, char>
{
public:
	static wchar_t f (const char c)
	{
		return (static_cast<unsigned int>(c) < 128)
			? static_cast<wchar_t>(c) : L'?';
	}

};

/**
 *	Generic conversion functor specialization: wide to narrow character.
 */
template <>
class converter<char, wchar_t>
{
public:
	static char f (const wchar_t c)
	{
		return (static_cast<unsigned int>(c) < 128)
			? static_cast<char>(c) : '?';
	}

};

/**
 *	Generic conversion functor specialization: single character to string.
 */
template <typename Char_>
class converter<std::basic_string<Char_>, Char_>
{
public:
	static std::basic_string<Char_> f (const Char_& a_in)
	{
		return std::basic_string<Char_>(1, a_in);
	}

};

/**
 *	Generic conversion functor specialization: string to a single character.
 */
template <typename Char_>
class converter<Char_, std::basic_string<Char_> >
{
public:
	static Char_ f (const std::basic_string<Char_>& a_in)
	{
		return a_in. empty() ? static_cast<Char_>(0) : a_in[0];
	}

};

/**
 *	Generic conversion functor specialization: strings of the same type.
 */
template <typename Char1_, typename Char2_>
class converter<std::basic_string<Char1_>, std::basic_string<Char2_> >:
	public converter_string<Char1_, Char2_>
{
};

/**
 *	An auxiliary metafunction that returns the type of an iterator.
 *	@{ */
template <typename X_>
struct iterator_type
{
	typedef typename X_::value_type type;
};

template <> struct iterator_type<char> { typedef char type; };
template <> struct iterator_type<wchar_t> { typedef wchar_t type; };
template <> struct iterator_type<char*> { typedef char type; };
template <> struct iterator_type<wchar_t*> { typedef wchar_t type; };
template <> struct iterator_type<const char*> { typedef char type; };
template <> struct iterator_type<const wchar_t*> { typedef wchar_t type; };
/**	@} */

/**
 *	An auxiliary tag structure for specializations.
 */
struct iterator_tag {};

/**
 *	Generic conversion functor specialization: iterator range to string.
 */
template <typename Char_>
class converter<std::basic_string<Char_>, iterator_tag>
{
public:
	template <typename Iter_>
	static std::basic_string<Char_> f (Iter_ begin, Iter_ end)
	{
		return converter_range<Char_, typename iterator_type<Iter_>::type>::f(
				begin, end);
	}

};

/**
 *	Generic conversion functor specialization: iterator range to single
 *	character.
 */
template <typename Char_>
class converter<Char_, iterator_tag>
{
public:
	template <typename Iter_>
	static Char_ f (Iter_ begin, Iter_ end)
	{
		return (begin == end)
			? static_cast<Char_>(0)
			: converter_range<Char_, typename iterator_type<Iter_>::type>::f(
					begin, end)[0];
	}

};

} // namespace detail

/// (forward declaration)
template <typename Out_> class make;

/**
 *	A string wrapper template class that makes it possible to define string
 *	objects regardless whether wide or narrow sequence is given as the
 *	initializer.
 */
template <typename Str_>
class wrapper: public Str_
{
public:
	wrapper ():
		Str_ ()
	{
	}

	wrapper (const Str_& s, typename Str_::size_type pos = 0,
			typename Str_::size_type n = Str_::npos):
		Str_ (s, pos, n)
	{
	}

	wrapper (const typename Str_::value_type* s):
		Str_ (s)
	{
	}

	wrapper (const typename Str_::value_type* s,
			typename Str_::size_type n):
		Str_ (s, n)
	{
	}

	wrapper (typename Str_::size_type n, typename Str_::value_type c):
		Str_ (n, c)
	{
	}

	template <typename Custom_>
	wrapper (Custom_ s):
		Str_ (make<Str_>::from(s))
	{
	}

	template <typename Char_>
	wrapper (typename Str_::size_type n, Char_ c):
		Str_ (n, make<typename Str_::value_type>::from(c))
	{
	}

	template <typename Iterator_>
	wrapper (Iterator_ first, Iterator_ second):
		Str_ (first, second)
	{
	}

	inline operator std::string() const;
	inline operator std::wstring() const;

	static const wrapper<Str_>& null ()
	{
		static const wrapper<Str_> sc_null;
		return sc_null;
	}

};

/**
 *	A set of ports for the original comparison operators.
 *	@{ */
#define ENCODE_WRAPPER_OPERATOR(f) \
	template <typename Str_, typename Val_> \
	inline bool operator f (const wrapper<Str_>& a_str, const Val_& a_val) { \
		return static_cast<const Str_&>(a_str) f make<Str_>::from(a_val); }
ENCODE_WRAPPER_OPERATOR(==)
ENCODE_WRAPPER_OPERATOR(!=)
ENCODE_WRAPPER_OPERATOR(<)
ENCODE_WRAPPER_OPERATOR(<=)
ENCODE_WRAPPER_OPERATOR(>)
ENCODE_WRAPPER_OPERATOR(>=)
#undef ENCODE_WRAPPER_OPERATOR
/**	@} */

/**
 *	An auxiliary specializable functor that transforms wrapped string instances
 *	back into original string instances.
 *	@{ */
template <typename Str_>
struct unwrap_functor;

/**
 *	A helper base functor. @See unwrap_functor.
 */
template <typename reference_type>
struct basic_unwrap_functor
{
	typedef reference_type result_type;

	static result_type f (result_type in)
	{
		return in;
	}

};

/**
 *	Specialization of unwrap_functor for custom mutable references.
 */
template <typename Str_>
struct unwrap_functor<Str_&>:
	basic_unwrap_functor<Str_&> {};

/**
 *	Specialization of unwrap_functor for custom constant references.
 */
template <typename Str_>
struct unwrap_functor<const Str_&>:
	basic_unwrap_functor<const Str_&> {};

/**
 *	Specialization of unwrap_functor for custom mutable references to wrapped
 *	strings.
 */
template <typename Str_>
struct unwrap_functor<wrapper<Str_>&>:
	basic_unwrap_functor<Str_&> {};

/**
 *	Specialization of unwrap_functor for custom constant references to wrapped
 *	strings.
 */
template <typename Str_>
struct unwrap_functor<const wrapper<Str_>&>:
	basic_unwrap_functor<const Str_&> {};
/**	@} */

/**
 *	Returns a mutable reference to the unwrapped string instance.
 */
template <typename Str_>
inline static typename unwrap_functor<Str_&>::result_type unwrap (Str_& s)
{
	return unwrap_functor<Str_&>::f(s);
}

/**
 *	Returns a constant reference to the unwrapped string instance.
 */
template <typename Str_>
inline static typename unwrap_functor<const Str_&>::result_type unwrap (
		const Str_& s)
{
	return unwrap_functor<const Str_&>::f(s);
}

/**
 *	A metafunction that converts wrapped string types to the original types.
 *	@{ */
template <typename Str_>
struct unwrapper
{
	typedef Str_ type;

};

template <typename Str_>
struct unwrapper<wrapper<Str_> >
{
	typedef Str_ type;

};
/**	@} */

/**
 *	Main string transformation gateway. Creates instances of explicitly
 *	specified type form given data (if possible).
 */
template <typename Out_>
class make
{
public:
	static const Out_& from (const Out_& in)
	{
		return in;
	}

	template <typename In_>
	static Out_ from (const In_& in)
	{
		return detail::converter<typename unwrapper<Out_>::type, In_>::f(in);
	}

	template <typename In_>
	static Out_ from (const wrapper<In_>& in)
	{
		return detail::converter<typename unwrapper<Out_>::type, In_>::f(in);
	}

	template <typename Iter_>
	static Out_ from (const Iter_& begin, const Iter_& end)
	{
		return detail::converter<typename unwrapper<Out_>::type,
				detail::iterator_tag>::f(&* begin, &* end);
	}

	static Out_ from (char* in)
	{
		return make<Out_>::from(in, in + strlen(in));
	}

	static Out_ from (const char* in)
	{
		return make<Out_>::from(in, in + strlen(in));
	}

	static Out_ from (wchar_t* in)
	{
		return make<Out_>::from(in, in + strlen(in));
	}

	static Out_ from (const wchar_t* in)
	{
		return make<Out_>::from(in, in + strlen(in));
	}

private:
	template <typename Iter_>
	static unsigned int strlen (Iter_ iter)
	{
		unsigned int len = 0;
		while (*(iter ++)) ++ len;
		return len;
	}

};

template <typename Str_>
wrapper<Str_>::operator std::string() const
{
	return make<std::string>::from(*this);
}

template <typename Str_>
wrapper<Str_>::operator std::wstring() const
{
	return make<std::wstring>::from(*this);
}

/**
 *	For your convenience: narrow string constructor.
 *	@{ */
inline const std::string& string (const std::string& in)
{
	return in;
}

template <typename In_>
inline std::string string (In_ in)
{
	return make<std::string>::from(in);
}

template <typename Iter_>
inline std::string string (const Iter_& begin, const Iter_& end)
{
	return make<std::string>::from(begin, end);
}
/**	@} */

/**
 *	For your convenience: wide string constructor.
 *	@{ */
inline const std::wstring& wstring (const std::wstring& in)
{
	return in;
}

template <typename In_>
inline std::wstring wstring (In_ in)
{
	return make<std::wstring>::from(in);
}

template <typename Iter_>
inline std::wstring wstring (const Iter_& begin, const Iter_& end)
{
	return make<std::wstring>::from(begin, end);
}
/**	@} */

/**
 *	Supplementary standard stream output operator for wrapped strings.
 */
template <typename Char_, typename Traits_, typename Str_>
std::basic_ostream<Char_, Traits_>& operator<< (
		std::basic_ostream<Char_, Traits_>& a_stream,
		const wrapper<Str_>& a_string)
{
	return a_stream << make<std::basic_string<Char_> >::from(a_string);
}

} // namespace encode

#endif /* ENCODE_ENCODE_HPP_ */
