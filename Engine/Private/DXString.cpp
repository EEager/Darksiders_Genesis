#include "../Public/DXString.h"
#pragma warning( disable : 4996 )

//////////////////////////////////////////////////////////////////////////
///@brief ���ڿ� �ڸ���
///@param orgin : ���� ���ڿ�
///@param tok : �ڸ� ������ �Ǵ� ���ڿ�
///@return �Ϸ�� ���ڿ� �迭
//////////////////////////////////////////////////////////////////////////
void DXString::SplitString(vector<string>* result, string origin, string tok)
{
	result->clear();

	int cutAt = 0; //�ڸ� ��ġs
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //�ڸ��� ��ġ�� 0����ũ��
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //�ڸ��� ���� ���� �ִٸ�
		result->push_back(origin.substr(0, cutAt));
}

void DXString::SplitString(vector<wstring>* result, wstring origin, wstring tok)
{
	result->clear();

	int cutAt = 0; //�ڸ� ��ġs
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //�ڸ��� ��ġ�� 0����ũ��
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //�ڸ��� ���� ���� �ִٸ�
		result->push_back(origin.substr(0, cutAt));
}


//////////////////////////////////////////////////////////////////////////
///@brief ���� ���ڿ��� ������ üũ
///@param str : üũ�Ϸ��� ���ڿ�
///@param comp : ���� �񱳹��ڿ�
//////////////////////////////////////////////////////////////////////////
bool DXString::StartsWith(string str, string comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool DXString::StartsWith(wstring str, wstring comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
///@brief �ش� ���ڿ��� ���ԵǾ� �ִ���
///@param str : üũ�Ϸ��� ���ڿ�
///@param comp : �񱳹��ڿ�
//////////////////////////////////////////////////////////////////////////
bool DXString::Contain(string str, string comp)
{
	size_t found = str.find(comp);

	return found != wstring::npos;
}

bool DXString::Contain(wstring str, wstring comp)
{
	size_t found = str.find(comp);

	return found != wstring::npos;
}


//////////////////////////////////////////////////////////////////////////
///@brief �ش� ���ڿ����� comp ���ڸ� rep�� ����
///@param str : üũ�Ϸ��� ���ڿ�
///@param comp : �񱳹��ڿ�
///@param rep : �ٲܹ��ڿ�
//////////////////////////////////////////////////////////////////////////
void DXString::Replace(string* str, string comp, string rep)
{
	string temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

void DXString::Replace(wstring* str, wstring comp, wstring rep)
{
	wstring temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief string���� wstring������ ����
///@param value : ��ȯ�� ���ڿ�
///@return ��ȯ �Ϸ� ���ڿ�
//////////////////////////////////////////////////////////////////////////
wstring DXString::ToWString(string value)
{
	wstring temp = L"";
	temp.assign(value.begin(), value.end());

	return temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief wstring���� string������ ����
///@param value : ��ȯ�� ���ڿ�
///@return ��ȯ �Ϸ� ���ڿ�
//////////////////////////////////////////////////////////////////////////
string DXString::ToString(wstring value)
{
	string temp = "";
	temp.assign(value.begin(), value.end());

	return temp;
}
string DXString::Format(const string format, ...)
{
	// ���� �μ��� ������ ��
	va_list args;

	va_start(args, format); // window �ȿ� �ִ°�
	// sprintf ���ڿ� null ������ �� �� �־�� snpritnf�̰� v�� �����μ� �����Ϸ��� �ϴ� �Լ�
	size_t len = vsnprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	vector<char> vec(len + 1);

	va_start(args, format);
	vsnprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}
wstring DXString::Format(const wstring  format, ...)
{

	// ���� �μ��� ������ ��
	va_list args;

	va_start(args, format); // window �ȿ� �ִ°�
							// sprintf ���ڿ� null ������ �� �� �־�� snpritnf�̰� v�� �����μ� �����Ϸ��� �ϴ� �Լ�
	size_t len = _vsnwprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	vector<WCHAR> vec(len + 1);

	va_start(args, format);
	_vsnwprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}
const char* DXString::WideToChar(const _tchar* pWideChar)
{
	char retChar[MAX_TAG_LEN] = { 0, };
	WideCharToMultiByte(CP_ACP, 0, pWideChar, MAX_TAG_LEN, retChar, MAX_TAG_LEN, NULL, NULL);
	return retChar;
}
const _tchar* DXString::CharToWide(const char* pChar)
{
	_tchar retWideChar[MAX_TAG_LEN] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pChar, MAX_TAG_LEN, retWideChar, MAX_TAG_LEN);
	return retWideChar;
}
//////////////////////////////////////////////////////////////////////////