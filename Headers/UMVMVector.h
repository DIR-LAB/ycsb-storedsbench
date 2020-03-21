//UMVMVector.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVM_VECTOR_H
#define UMVM_VECTOR_H

#include "UMVMExceptions.h"

#include <functional>

using std::function;

namespace UserModeVirtualMemory
{

	template<class T>
	struct MyTypeTraits
	{
		static const char* name;
	};

	template <class T>
	const char* MyTypeTraits<T>::name = "undefined";

	//template<>
	//struct MyTypeTraits<int>
	//{
	//	static const char* name;
	//};


	template<class T> class UMVMVectorRef;

	template <class T>
	class UMVMVector
	{
		static_assert (BlockSize > sizeof(T), "m_Data type too large to fit in block!");
		static const BigArrayIndex EntriesPerBlock = BlockSize / sizeof(T);

		typedef pair<T, bool> UserFunctionStatusType;

	private:
		string m_ArrayName;
		UserModeVirtualMemoryFile* m_QF;
		UMVMVectorInternal m_SegmentBlockNumbers;
		bool m_Closed;

		BlockNumber m_BPANumber;
		BigArrayIndex m_ElementCount;
		BigArrayIndex m_MaximumElementCount;
		BigArrayIndex m_MaximumSegmentNumber;

		BlockNumber m_CurrentStoreLeafBlockNumber;
		BigArrayIndex m_CurrentStoreSegmentNumber;
		AccessVector<T> m_CurrentStoreAccessVector;

		BlockNumber m_CurrentGetLeafBlockNumber;
		BigArrayIndex m_CurrentGetSegmentNumber;
		AccessVector<T> m_CurrentGetAccessVector;

//		m_BlockNumber m_SegmentBlockNumbersVectorBPANumber;

		void StoreElementHelper(BigArrayIndex SegmentNumber, ArrayIndex ElementNumber, T p_Element);
		T GetElementHelper(BigArrayIndex SegmentNumber, ArrayIndex ElementNumber);

	public:
		UMVMVector<T>();
		~UMVMVector<T>();
		UMVMVector<T>(UserModeVirtualMemoryFile* UMVMF, const string& p_ArrayName, const string& p_Type = MyTypeTraits<T>::name);
		UMVMVector<T>& operator = (UMVMVector<T>&& other);
		void Open(const string& p_ArrayName, const string& p_Type = MyTypeTraits<T>::name);
		void StoreElement(BigArrayIndex p_Index, T p_Element);
		T GetElement(BigArrayIndex p_Index);
		UMVMVectorRef<T> operator[](BigArrayIndex p_Index);

//		pair<T, BigArrayIndex> CallUserFunctionToReadElementsInRange(std::function<UserFunctionStatusType(const T argument)> UserFunc, BigArrayIndex Start, BigArrayIndex End);
		pair<T, BigArrayIndex> FindElementInVector(const T argument, BigArrayIndex Start, BigArrayIndex End);

		BigArrayIndex GetMaximumElementNumber();
		void Flush();
		void Close();
		BigArrayIndex size();
		void ReduceSize(BigArrayIndex NewSize);
		void UpdateSize(BigArrayIndex NewSize);
		BlockNumber GetBPAQuantumNumber();
		T PopLastElementIfItExists();
		void PushElementBack(T p_Element);

	};

	template <class T>
	class UMVMVectorRef
	{
	private:
		UMVMVector<T>& m_PAU;
		BigArrayIndex m_Index;

		UMVMVectorRef<T>& operator=(const UMVMVectorRef<T>&) = delete;

	public:
		UMVMVectorRef<T>(UMVMVector<T> &p_PAU, BigArrayIndex p_Index);
		UMVMVectorRef<T>& operator=(T p_Element);
		operator T();
	};

	template <class T>
	UMVMVector<T>::UMVMVector()
		:m_QF(0), m_ElementCount(0), m_BPANumber(NoBlockNumber), m_SegmentBlockNumbers(), m_Closed(false), m_MaximumElementCount(UINT64_MAX),
		m_MaximumSegmentNumber(UINT32_MAX), m_CurrentStoreSegmentNumber(UINT32_MAX), m_CurrentGetSegmentNumber(UINT32_MAX)
	{
	}

	template <class T>
	void UMVMVector<T>::Close()
	{
		if (m_Closed)
			return;

		m_Closed = true;

		if (!m_QF)
			return;

		m_CurrentStoreLeafBlockNumber = QuantumNotFound;
		m_CurrentGetLeafBlockNumber = QuantumNotFound;

		if (m_QF->ErrorHasOccurred())
			return;

		Flush();
		m_QF->CloseObject(m_BPANumber);

		m_SegmentBlockNumbers.Close();
	}

	template <class T>
	UMVMVector<T>::~UMVMVector()
	{
		Close();
	}

	template<class T>
	inline UMVMVector<T>& UMVMVector<T>::operator=(UMVMVector<T>&& other)
	{
		if (this != &other)
		{
			m_Closed = other.m_Closed;
			other.m_Closed = true;

			m_ArrayName = other.m_ArrayName;
			m_QF = std::move(other.m_QF);
			m_BPANumber = other.m_BPANumber;
			m_ElementCount = other.m_ElementCount;
			m_MaximumSegmentNumber = other.m_MaximumSegmentNumber;

			m_CurrentStoreLeafBlockNumber = other.m_CurrentStoreLeafBlockNumber;
			m_CurrentStoreSegmentNumber = other.m_CurrentStoreSegmentNumber;
			m_CurrentStoreAccessVector = other.m_CurrentStoreAccessVector;

			m_CurrentGetLeafBlockNumber = other.m_CurrentGetLeafBlockNumber;
			m_CurrentGetSegmentNumber = other.m_CurrentGetSegmentNumber;
			m_CurrentGetAccessVector = other.m_CurrentGetAccessVector;

			m_SegmentBlockNumbers = std::move(other.m_SegmentBlockNumbers);
		}

		return *this;
	}

	template <class T>
	void UMVMVector<T>::Open(const string& p_ArrayName, const string& p_Type)
	{
		m_ArrayName = p_ArrayName + ":" + p_Type;
		m_BPANumber = m_QF->FindObjectByName(m_ArrayName);

		BigArrayIndex ElementsPerSegment = m_QF->GetBlockSize() / sizeof(T);
		BigArrayIndex MaximumSegmentCount = m_QF->GetMaximumSimpleVectorElementNumber();
		m_MaximumSegmentNumber = MaximumSegmentCount - 1;

		m_MaximumElementCount = ElementsPerSegment * MaximumSegmentCount;

		if (m_BPANumber == QuantumNotFound)
		{
			m_BPANumber = m_QF->CreateObjectAndAddToDirectory(m_ArrayName, m_MaximumElementCount, true);
		}

		m_ElementCount = m_QF->GetObjectElementCount(m_BPANumber);
		m_QF->OpenObject(m_BPANumber);

		m_CurrentGetLeafBlockNumber = QuantumNotFound;
		m_CurrentGetSegmentNumber = BadBigArrayIndex;
		m_CurrentGetAccessVector = AccessVector<T>();

		m_CurrentStoreLeafBlockNumber = QuantumNotFound;
		m_CurrentStoreSegmentNumber = BadBigArrayIndex;
		m_CurrentStoreAccessVector = AccessVector<T>();
	}

	template <class T>
	UMVMVector<T>::UMVMVector(UserModeVirtualMemoryFile* UMVMF, const string& p_ArrayName, const string& p_Type)
		: m_ArrayName(p_ArrayName)
		, m_QF(UMVMF)
		, m_SegmentBlockNumbers(m_QF,m_ArrayName)
		, m_Closed(false)
		, m_BPANumber(0)
		, m_ElementCount(0)
		, m_MaximumElementCount(UINT64_MAX)
		, m_MaximumSegmentNumber(UINT32_MAX)
		, m_CurrentStoreLeafBlockNumber(UINT32_MAX)
		, m_CurrentStoreSegmentNumber(UINT32_MAX)
		, m_CurrentGetSegmentNumber(UINT32_MAX)
	{
		Open(p_ArrayName, p_Type);
	}

	template <class T>
	void UMVMVector<T>::Flush()
	{
		m_QF->SetObjectElementCount(m_BPANumber, m_ElementCount);
		m_QF->Flush();
	}

	template <class T> pair<T, BigArrayIndex> UMVMVector<T>::FindElementInVector(const T argument, BigArrayIndex Start, BigArrayIndex End)
	{
		UserFunctionStatusType StatusCode(-1,-1);

		BigArrayIndex i;

		T Element;

		for (i = Start; i < End; ++i)
		{
			ArrayIndex SegmentNumber = static_cast<ArrayIndex>(i / EntriesPerBlock);
			ArrayIndex ElementNumber = static_cast<ArrayIndex>(i % EntriesPerBlock);

			if (SegmentNumber == m_CurrentGetSegmentNumber)
				Element = m_CurrentGetAccessVector[ElementNumber];
			else
				Element = GetElement(i);

			if (Element == argument)
			{
				return UserFunctionStatusType(argument, i);
				break;
			}
		}

		return StatusCode;
	}
	
	template <class T>
	void UMVMVector<T>::StoreElementHelper(BigArrayIndex SegmentNumber, ArrayIndex ElementNumber, T p_Element)
	{
		BlockNumber SegmentBlockNumber = m_SegmentBlockNumbers.GetElement(SegmentNumber);

		QuantumBlockPtr ThisQuantumBlock;

		if (SegmentBlockNumber == NoBlockNumber)
		{
			SegmentBlockNumber = m_QF->FindEmptyBlock();
			m_SegmentBlockNumbers.StoreElement(SegmentNumber, SegmentBlockNumber);

			ThisQuantumBlock = m_QF->MakeQuantumBlockPtr(SegmentBlockNumber);

			ThisQuantumBlock->Clear();
		}
		else
		{
			ThisQuantumBlock = m_QF->MakeQuantumBlockPtr(SegmentBlockNumber);
		}

		m_CurrentStoreAccessVector = AccessVector<T>((T*)ThisQuantumBlock->GetBufferAddress(), BlockSize);

		m_CurrentStoreLeafBlockNumber = SegmentBlockNumber;

		m_CurrentStoreSegmentNumber = SegmentNumber;

		m_CurrentStoreAccessVector[ElementNumber] = p_Element;
	}

	template <class T>
	void UMVMVector<T>::StoreElement(BigArrayIndex p_Index, T p_Element)
	{
		if (p_Index >= m_ElementCount)
		{
			m_ElementCount = p_Index + 1;
		}

		BigArrayIndex SegmentNumber = p_Index / EntriesPerBlock;

		if (SegmentNumber == m_CurrentStoreSegmentNumber)
		{
			m_CurrentStoreAccessVector[static_cast<ArrayIndex>(p_Index % EntriesPerBlock)] = p_Element;
			return;
		}

		if (SegmentNumber >= m_MaximumSegmentNumber)
			throw UMVMExceptionLogicError("Quantum file: bad element index in StoreElement.");

		StoreElementHelper(SegmentNumber, static_cast<ArrayIndex>(p_Index % EntriesPerBlock), p_Element);
	}

	template <class T>
	T UMVMVector<T>::GetElementHelper(BigArrayIndex SegmentNumber, ArrayIndex ElementNumber)
	{
		BlockNumber SegmentBlockNumber = m_SegmentBlockNumbers.GetElement(SegmentNumber);
		if (SegmentBlockNumber == NoBlockNumber)
		{
			return T();
		}

		QuantumBlockPtr ThisQuantumBlock = m_QF->MakeQuantumBlockPtr(SegmentBlockNumber);
		m_CurrentGetAccessVector = AccessVector<T>((T*)ThisQuantumBlock->GetBufferAddress(), BlockSize);

		m_CurrentGetLeafBlockNumber = SegmentBlockNumber;

		m_CurrentGetSegmentNumber = SegmentNumber;

		return m_CurrentGetAccessVector[ElementNumber];
	}

	template <class T>
	T UMVMVector<T>::GetElement(BigArrayIndex p_Index)
	{
		BigArrayIndex SegmentNumber = p_Index / EntriesPerBlock;

		if ((p_Index / EntriesPerBlock) == m_CurrentGetSegmentNumber)
		{
			return m_CurrentGetAccessVector[static_cast<ArrayIndex>(p_Index % EntriesPerBlock)];
		}

		if (p_Index >= m_ElementCount)
			return T();

		return GetElementHelper(SegmentNumber, static_cast<ArrayIndex>(p_Index % EntriesPerBlock));
	}

	template <class T>
	UMVMVectorRef<T>::UMVMVectorRef(UMVMVector<T> &p_PAU,
		BigArrayIndex p_Index) : m_PAU(p_PAU), m_Index(p_Index)
	{
	}

	template <class T>
	UMVMVectorRef<T> &UMVMVectorRef<T>::operator=(T p_Element)
	{
		m_PAU.StoreElement(m_Index, p_Element);
		return *this;
	}

	template <class T>
	UMVMVectorRef<T>::operator T()
	{
		return m_PAU.GetElement(m_Index);
	}

	template <class T>
	UMVMVectorRef<T> UMVMVector<T>::operator[](BigArrayIndex p_Index)
	{
		return UMVMVectorRef<T>(*this, p_Index);
	}

	template <class T>
	BigArrayIndex UMVMVector<T>::size()
	{
		return m_ElementCount;
	}


	template <class T>
	void UMVMVector<T>::ReduceSize(BigArrayIndex NewSize)
	{
		if (NewSize < m_ElementCount)
			m_ElementCount = NewSize;
	}

	template <class T>
	void UMVMVector<T>::UpdateSize(BigArrayIndex NewSize)
	{
		if (NewSize >= m_ElementCount)
			m_ElementCount = NewSize;
	}

	template <class T>
	BigArrayIndex UMVMVector<T>::GetMaximumElementNumber()
	{
		return m_QF->GetObjectMaxElementCount(m_BPANumber)*EntriesPerBlock - 1;
	}

	template <class T>
	BlockNumber UMVMVector<T>::GetBPAQuantumNumber()
	{
		return m_BPANumber;
	}

	template<class T>
	inline T UMVMVector<T>::PopLastElementIfItExists()
	{
		if (size() == 0)
			return T();

		return GetElement(--m_ElementCount);
	}

	template<class T>
	inline void UMVMVector<T>::PushElementBack(T p_Element)
	{
		StoreElement(m_ElementCount, p_Element);
	}

	//template <class T> pair<T, BigArrayIndex> UMVMVector<T>::CallUserFunctionToReadElementsInRange(std::function<UserFunctionStatusType(const T argument)> UserFunc, BigArrayIndex Start, BigArrayIndex End)
	//{
	//	UserFunctionStatusType StatusCode;

	//	BigArrayIndex i;

	//	T Element;

	//	for (i = Start; i < End; ++i)
	//	{
	//		ArrayIndex SegmentNumber = static_cast<ArrayIndex>(i / EntriesPerBlock);
	//		ArrayIndex ElementNumber = static_cast<ArrayIndex>(i % EntriesPerBlock);

	//		if (SegmentNumber == m_CurrentGetSegmentNumber)
	//			Element = m_CurrentGetAccessVector[ElementNumber];
	//		else
	//			Element = GetElement(i);

	//		StatusCode = UserFunc(Element);
	//		if (StatusCode.second == false)
	//			break;
	//	}

	//	return pair<T, BigArrayIndex>(StatusCode.first, i);
	//}


}

#endif
