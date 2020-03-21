//UMVMAccessVector.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVM_ACCESS_VECTOR_H
#define UMVM_ACCESS_VECTOR_H

namespace UserModeVirtualMemory
{

	void inline breakpointFunction(size_t m_Count)
	{
	}

	template <class T>
	class AccessVector
	{
	protected:
		T *m_Data;
		size_t m_Count;
		size_t m_ReservedSize;
	public:
		AccessVector();
		AccessVector(const AccessVector& p_AccessVector);
		AccessVector& operator = (const AccessVector& p_AccessVector);
		AccessVector(T *p_Data, size_t p_Count);
		AccessVector(T *p_Data, size_t p_Count, size_t p_ReservedCount);
		AccessVector(char *p_Data);

		AccessVector(T const *p_Data, size_t p_Count);
		AccessVector(T const *p_Data, size_t p_Count, size_t p_ReservedCount);
		AccessVector(char const *p_Data);

		AccessVector(std::nullptr_t, size_t p_Count);
		AccessVector(std::nullptr_t, size_t p_Count, size_t p_ReservedCount);

		AccessVector(std::string& p_String);
		AccessVector(const std::string& p_String);

		bool resize(size_t p_NewCount);
		T *data() const { return m_Data; }

		int	operator==(const AccessVector& p_AccessVector) const;
		int operator==(const std::string& p_Data) const;
		
		int	operator!=(const AccessVector& p_AccessVector) const;
		int operator!=(const std::string& p_Data) const;

		T& operator[](size_t p_ArrayIndex)
		{
			return m_Data[p_ArrayIndex];
		}

		T& operator[](size_t p_ArrayIndex) const 
		{
			return m_Data[p_ArrayIndex];
		}

		size_t size() const { return m_Count; }
		size_t ReservedSize() const { return m_ReservedSize; }

		string ToString();
	};

	template <class T>
	AccessVector<T>::AccessVector()
		:m_Data(0)
		, m_Count(0)
		, m_ReservedSize(0)
	{
	}

	template <class T>
	AccessVector<T>::AccessVector(const AccessVector<T>& p_AccessVector)
		:m_Data(p_AccessVector.m_Data)
		, m_Count(p_AccessVector.m_Count)
		, m_ReservedSize(p_AccessVector.m_ReservedSize)
	{
	}

	template <class T>
	AccessVector<T>::AccessVector(T *p_Data, size_t p_Count)
		:m_Data(p_Data)
		, m_Count(p_Count)
		, m_ReservedSize(p_Count)
	{
	}

	template <class T>
	AccessVector<T>::AccessVector(T *p_Data, size_t p_Count, size_t p_ReservedCount)
		:m_Data(p_Data)
		, m_Count(p_Count)
		, m_ReservedSize(p_ReservedCount)
	{
	}

	template <class T>
	AccessVector<T>::AccessVector(char *p_Data)
		:m_Data(const_cast<T *>(p_Data))
		, m_Count(strlen(p_Data))
		, m_ReservedSize(UINT_MAX)
	{
	}

	template<class T>
	inline AccessVector<T>::AccessVector(std::string & p_String)
		: m_Data(const_cast<char*>(p_String.data()))
		, m_Count(p_String.size())
		, m_ReservedSize(m_Count)
	{
	}

	template<class T>
	inline AccessVector<T>::AccessVector(const std::string & p_String)
		: m_Data(const_cast<char*>(p_String.data()))
		, m_Count(p_String.size())
		, m_ReservedSize(m_Count)
	{
	}

	template <class T>
	AccessVector<T>::AccessVector(T const *p_Data, size_t p_Count)
		:m_Data(const_cast<T*>(p_Data))
		, m_Count(p_Count)
		, m_ReservedSize(m_Count)
	{
	}

	template <class T>
	AccessVector<T>::AccessVector(T const *p_Data, size_t p_Count, size_t p_ReservedCount)
		:m_Data(const_cast<T*>(p_Data))
		, m_Count(p_Count)
		, m_ReservedSize(p_ReservedCount)
	{
	}

	template <class T>
	AccessVector<T>::AccessVector(char const *p_Data)
		:m_Data(const_cast<T*>(p_Data))
		, m_Count(strlen(p_Data))
		, m_ReservedSize(m_Count)
	{
	}

	template<class T>
	AccessVector<T>::AccessVector(std::nullptr_t, size_t p_Count)
		:m_Data(reinterpret_cast<T*>(0))
		, m_Count(p_Count)
		, m_ReservedSize(p_Count)
	{
	}

	template<class T>
	AccessVector<T>::AccessVector(std::nullptr_t, size_t p_Count, size_t p_ReservedCount)
		:m_Data(reinterpret_cast<T*>(0))
		, m_Count(p_Count)
		, m_ReservedSize(p_ReservedCount)
	{
	}

	template<class T>
	AccessVector<T>& AccessVector<T>::operator=(const AccessVector & p_AccessVector)
	{
		breakpointFunction(m_Count);
		m_Data = p_AccessVector.m_Data;
		m_Count = p_AccessVector.m_Count;
		m_ReservedSize = p_AccessVector.m_ReservedSize;
		return *this;
	}

	template <class T>
	int AccessVector<T>::operator!=(const AccessVector<T>& p_AccessVector) const
	{
		return !(*this == p_AccessVector);
	}

	template<class T>
	inline int AccessVector<T>::operator!=(const std::string& p_Data) const
	{
		return !(*this == p_Data);
	}

	template<class T>
	inline string AccessVector<T>::ToString()
	{
		return string(m_Data, m_Count);
	}

	template <class T>
	int AccessVector<T>::operator==(const AccessVector<T>& p_AccessVector) const
	{
		breakpointFunction(m_Count);
		if (this == &p_AccessVector)
			return 1;
		if (m_Count != p_AccessVector.m_Count)
			return 0;
		if (memcmp(m_Data, p_AccessVector.m_Data, m_Count * sizeof(T)) == 0)
			return 1;
		return 0;
	}

	template<class T>
	inline int AccessVector<T>::operator==(const std::string& p_Data) const
	{
		if (p_Data.size() != m_Count)
			return 0;
		if (memcmp(m_Data, p_Data.data(), p_Data.size()) == 0)
			return 1;
		return 0;
	}

	template <class T>
	bool AccessVector<T>::resize(size_t p_NewCount)
	{
		breakpointFunction(m_Count);
		if (p_NewCount <= m_ReservedSize)
		{
			m_Count = p_NewCount;
			return true;
		}

		return false;
	}

	typedef AccessVector<char> AccessVectorChar;

	inline bool operator == (const std::string& p_String, AccessVectorChar p_AVC)
	{
		if (p_String.size() != p_AVC.size())
			return 0;

		if (memcmp(p_String.data(), p_AVC.data(), p_AVC.size()) == 0)
			return 1;

		return 0;
	}

	inline bool operator != (const std::string& p_String, AccessVectorChar p_AVC)
	{
		return !(p_String == p_AVC);
	}

}

#endif
