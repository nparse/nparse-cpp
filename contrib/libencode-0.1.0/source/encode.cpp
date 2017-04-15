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
#include <encode/encode.hpp>
#include <utf8.h>

namespace {

template <int> class adapter;

// 16bit wchar_t
template <>
class adapter<2>
{
public:
	static void shrink (const wchar_t* begin, const wchar_t* end,
			std::string& out)
	{
		out. resize(utf8::utf16to8(begin, end, out. begin()) - out. begin());
	}

	static void stretch (const char* begin, const char* end,
			std::wstring& out)
	{
		out. resize(utf8::utf8to16(begin, end, out. begin()) - out. begin());
	}

};

// 32bit wchar_t
template <>
class adapter<4>
{
public:
	static void shrink (const wchar_t* begin, const wchar_t* end,
			std::string& out)
	{
		out. resize(utf8::utf32to8(begin, end, out. begin()) - out. begin());
	}

	static void stretch (const char* begin, const char* end,
			std::wstring& out)
	{
		out. resize(utf8::utf8to32(begin, end, out. begin()) - out. begin());
	}

};

} // namespace

namespace encode { namespace detail {

std::string converter_string<char, wchar_t>::f (const std::wstring& in)
{
	std::string res(in. size() * 4, static_cast<std::string::value_type>(0));
	adapter<sizeof(std::wstring::value_type)>::shrink(
			&* in. begin(), &* in. end(), res);
	return res;
}

std::wstring converter_string<wchar_t, char>::f (const std::string& in)
{
	std::wstring res(utf8::distance(in. begin(), in. end()),
			static_cast<std::wstring::value_type>(0));
	adapter<sizeof(std::wstring::value_type)>::stretch(
			&* in. begin(), &* in. end(), res);
	return res;
}

std::string converter_range<char, wchar_t>::f (const wchar_t* begin,
		const wchar_t* end)
{
	std::string res((end - begin) * 4, static_cast<std::string::value_type>(0));
	adapter<sizeof(std::wstring::value_type)>::shrink(begin, end, res);
	return res;
}

std::wstring converter_range<wchar_t, char>::f (const char* begin,
		const char* end)
{
	std::wstring res(utf8::distance(begin, end),
			static_cast<std::wstring::value_type>(0));
	adapter<sizeof(std::wstring::value_type)>::stretch(begin, end, res);
	return res;
}

}} // namespace encode::detail
