#pragma once

namespace Engine
{

	template <typename T>
	void Safe_Delete(T& ptr)
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& ptr)
	{
		if (ptr)
		{
			delete[] ptr;
			ptr = nullptr;
		}
	}

	template <typename T>
	unsigned long Safe_AddRef(T& ptr)
	{
		unsigned long refCount = 0;

		if (ptr)
		{
			refCount = ptr->AddRef();
		}

		return refCount;
	}

	template <typename T>
	unsigned long Safe_Release(T& ptr)
	{
		unsigned long refCount = 0;

		if (ptr)
		{
			refCount = ptr->Release();
		}

		if (0 == refCount)
			ptr = nullptr;

		return refCount;
	}

	class CTagFinder
	{
	public:
		CTagFinder(const _tchar* pTag) : m_pTag(pTag) { }
		~CTagFinder() = default;
	private:
		const _tchar*	m_pTag = nullptr;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{
			return !lstrcmp(Pair.first, m_pTag);
		}
	};
}