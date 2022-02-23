#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL DXString final 
{
public:
	static void SplitString(vector<string>* result, string origin, string tok);
	static void SplitString(vector<wstring>* result, wstring origin, wstring tok);

	static bool StartsWith(string str, string comp);
	static bool StartsWith(wstring str, wstring comp);

	static bool Contain(string str, string comp);
	static bool Contain(wstring str, wstring comp);

	static void Replace(string* str, string comp, string rep);
	static void Replace(wstring* str, wstring comp, wstring rep);

	static wstring ToWString(string value);
	static string ToString(wstring value);

	// ... 가변 인수
	static string Format(const string format, ...);
	static wstring Format(const wstring format, ...);

#define MAX_TAG_LEN 128
	// Wide -> Char
	static const char* WideToChar (const _tchar* pWideChar);
	// Char -> Wide
	static const _tchar* CharToWide (const char* pChar);

	static string DXString::WideToString(const _tchar* pWideChar);
	static wstring DXString::CharToWString(const char* pChar);
};

END