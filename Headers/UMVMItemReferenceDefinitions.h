//UMVMItemReferenceDefinitions.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ITEM_REFERENCE_DEFINITIONS_H
#define ITEM_REFERENCE_DEFINITIONS_H

namespace UserModeVirtualMemory
{
	typedef uint64_t UMVMItemReferenceData;

	const int QuantumNumberBits = 32; //QNB
	const int QuantumFreeSpaceBits = BlockSizeBits; //QFSB
	const int RelativeItemBits = 11; // RIB
	const int LittlePointerBlockLiteralSizeBits = 3;

	const int LittlePointerBlockLiteralMaxSize = (1 << LittlePointerBlockLiteralSizeBits) - 1;

	static_assert(QuantumNumberBits + QuantumFreeSpaceBits + RelativeItemBits + LittlePointerBlockLiteralSizeBits <= 8 * sizeof(UMVMItemReferenceData), "Item reference won't fit in 64 bits!");

	const UMVMItemReferenceData IRD1(1);

	const RelativeItemNumber NoItem = (1 << RelativeItemBits) - 1; // = the maximum number of elements in each block

	const ArrayIndex QuantumNotFound = 0;
}

#endif
