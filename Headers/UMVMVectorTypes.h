//UMVMVectorTypes.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMVECTORTYPES_H
#define  UMVMVECTORTYPES_H

#include "UMVMVector.h"

namespace UserModeVirtualMemory
{
	class Date;

	struct FreedBlockNumberDataStruct
	{
		BlockNumber m_StartingBlockNumber;
		ArrayIndex m_ConsecutiveBlockCount;
		FreedBlockNumberDataStruct() : m_StartingBlockNumber(0), m_ConsecutiveBlockCount(0) {}
		FreedBlockNumberDataStruct(BlockNumber StartingBlockNumber, ArrayIndex ConsecutiveBlockCount)
			: m_StartingBlockNumber(StartingBlockNumber), m_ConsecutiveBlockCount(ConsecutiveBlockCount) {}
	};

	struct OverflowAreaFreeListPersistentStruct
	{
		ArrayIndex m_Length;
		BlockNumber m_BlockNumber;
		ArrayIndex m_Offset;
		OverflowAreaFreeListPersistentStruct() : m_Length(0), m_BlockNumber(0), m_Offset(0) {}
		OverflowAreaFreeListPersistentStruct(ArrayIndex Length, BlockNumber ThisBlockNumber, ArrayIndex Offset)
			: m_Length(Length), m_BlockNumber(ThisBlockNumber), m_Offset(Offset) {}
	};

	typedef UMVMVector<FreedBlockNumberDataStruct> FreedBlockSaveVector;
	typedef UMVMVector<ArrayIndex> ArrayIndexVector;
	typedef UMVMVector<unsigned int> UnsignedIntVector;
	typedef UMVMVector<float> FloatVector;
	typedef UMVMVector<Date> DateVector;
	typedef UMVMVector<int64_t> Int64_tVector;
	typedef UMVMVector<unsigned char> ByteVector;
	typedef UMVMVector<OverflowAreaFreeListPersistentStruct> OverflowAreaFreeListPersistentStructVector;
}

#endif
