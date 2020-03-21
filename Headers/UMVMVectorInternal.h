//UMVMVectorInternal.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMVECTORINTERNAL_H
#define UMVMVECTORINTERNAL_H

#include "UMVMBlocki.h"
//#include "UMVMBlock.h"

namespace UserModeVirtualMemory
{
	class UMVMVectorInternalRef;

	class UMVMVectorInternal
	{
	private:
		string m_ArrayName;
		UserModeVirtualMemoryFile* m_QF;
		BlockNumber m_BPANumber;
		BigArrayIndex m_ElementCount;
		BigArrayIndex m_MaximumElementCount;
		BigArrayIndex m_MaximumSegmentNumber;
		bool m_Closed;

		BlockNumber m_CurrentStoreLeafBlockNumber;
		BigArrayIndex m_CurrentStoreSegmentNumber;
		AccessVector<BlockNumber> m_CurrentStoreAccessVector;

		BlockNumber m_CurrentGetLeafBlockNumber;
		BigArrayIndex m_CurrentGetSegmentNumber;
		AccessVector<BlockNumber> m_CurrentGetAccessVector;

		void StoreElementHelper(ArrayIndex SegmentNumber, ArrayIndex ElementNumber, BlockNumber p_Element);
		static const BigArrayIndex EntriesPerBlock = BlockSize / sizeof(BlockNumber);

	public:
		UMVMVectorInternal();
		~UMVMVectorInternal();
		UMVMVectorInternal(UserModeVirtualMemoryFile* UMVMF, const string& p_ArrayName, const string& p_Type = "UMVMVectorInternal");
		UMVMVectorInternal& operator = (UMVMVectorInternal&& other) noexcept;
		//UMVMVectorInternal(UserModeVirtualMemoryFile& UMVMF);
		void Open();
		void StoreElement(BigArrayIndex p_Index, BlockNumber p_Element);
		BlockNumber GetElement(BigArrayIndex p_Index);
		UMVMVectorInternalRef operator[](BigArrayIndex p_Index);
		BigArrayIndex GetMaximumElementNumber();
		void Flush();
		void Close();
		BigArrayIndex size();
		void ReduceSize(BigArrayIndex NewSize);
		void UpdateSize(BigArrayIndex NewSize);
		BlockNumber GetBPAQuantumNumber();
		static ArrayIndex GetElementsPerBlock();
	};

	class UMVMVectorInternalRef
	{
	private:
		UMVMVectorInternal & m_PAU;
		BigArrayIndex m_Index;

		UMVMVectorInternalRef& operator=(const UMVMVectorInternalRef&) = delete;

	public:
		UMVMVectorInternalRef(UMVMVectorInternal &p_PAU, BigArrayIndex p_Index);
		UMVMVectorInternalRef& operator=(BlockNumber p_Element);
		operator BlockNumber();
		operator ArrayIndex();
	};
};

#endif
