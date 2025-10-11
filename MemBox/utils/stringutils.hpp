#pragma once
#include "libs/utf8.h"
#include <iostream>


using std::string;
namespace MemBox
{
	namespace StringUtils
	{
		string WideToUTF8(const std::wstring& wstr)
		{
			std::string result;
			utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(result));
			return result;
		}
	}
}