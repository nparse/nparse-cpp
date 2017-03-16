/*
 * @file $/include/utility/pattern.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef UTILITY_PATTERN_HPP_
#define UTILITY_PATTERN_HPP_

namespace utility {

namespace {

template <typename CharT_> struct cd;

template <> struct cd<char>
{
	static const char slash = '\\';
	static const char plus = '+';
	static const char star = '*';
	static const char qmark = '?';
	static const char dollar = '$';
	static const char dot = '.';

};

template <> struct cd<wchar_t>
{
	static const wchar_t slash = L'\\';
	static const wchar_t plus = L'+';
	static const wchar_t star = L'*';
	static const wchar_t qmark = L'?';
	static const wchar_t dollar = L'$';
	static const wchar_t dot = '.';

};

} // namespace

template <typename CharT_>
struct pattern
{
	template <typename SampleIterator_, typename PatternIterator_, typename Is_,
		typename Of_>
	static bool match (SampleIterator_& s, const SampleIterator_& s_max,
			PatternIterator_& p, const PatternIterator_& p_max, const Is_& is,
			const Of_& of)
	{
		enum
		{
			stNormal, stEscaped, stSelected, stSelectedStar, stSelectedPlus,
			stSelectedQMark, stStuck
		} state = stNormal;

		for ( ; ((p != p_max) || (state == stSelected)) && (s != s_max); ++ p)
		{
			switch (state)
			{
			case stNormal:
				switch (*p)
				{
				case cd<CharT_>::slash:
					state = stEscaped;
					continue;

				case cd<CharT_>::dot:
					state = stSelected;
					continue;
				}
				break;

			case stEscaped:
				switch (*p)
				{
				case cd<CharT_>::slash:
				case cd<CharT_>::dot:
					state = stNormal;
					break;

				case cd<CharT_>::dollar:
					state = stStuck;
					goto done;

				default:
					state = stSelected;
					continue;
				}
				break;

			case stSelected:
				if (p != p_max)
				{
					switch (*p)
					{
					case cd<CharT_>::star:
						state = stSelectedStar;
						break;

					case cd<CharT_>::plus:
						state = stSelectedPlus;
						break;

					case cd<CharT_>::qmark:
						state = stSelectedQMark;
						break;

					default:
						break;
					}
				}
				-- p;
				break;

			default:
				break;
			}

			switch (state)
			{
			case stNormal:
				if (! is(*s, *p))
				{
					state = stStuck;
					goto done;
				}
				++ s;
				break;

			case stSelected:
				if (*p != cd<CharT_>::dot && ! of(*s, *p))
				{
					state = stStuck;
					goto done;
				}
				++ s;
				break;

			case stSelectedStar:
			case stSelectedPlus:
				if (*p == cd<CharT_>::dot || of(*s, *p))
				{
					do
					{
						if (++ s == s_max)
						{
							p += 2;
							goto done;
						}
					} while (*p == cd<CharT_>::dot || of(*s, *p));
				}
				else
				if (state == stSelectedPlus)
				{
					state = stStuck;
					goto done;
				}
				++ p;
				break;

			case stSelectedQMark:
				if (*p == cd<CharT_>::dot || of(*s, *p))
					++ s;
				++ p;
				break;

			default:
				break;
			}

			state = stNormal;
		}

done:	if (state == stStuck)
		{
			return false;
		}

		if (p == p_max)
		{
			return true;
		}

		if	(	p + 2 != p_max
			||	p[0] != cd<CharT_>::slash
			||	p[1] != cd<CharT_>::dollar
			)
		{
			return false;
		}
		else
		{
			p += 2;
		}

		return (s == s_max);
	}

};

} // namespace utility

#endif /* UTILITY_PATTERN_HPP_ */
