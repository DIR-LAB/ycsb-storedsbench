//UMVMUtility.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMUTILITY_H
#define UMVMUTILITY_H

namespace UserModeVirtualMemory
{
	void UMVMBlockMove(void *dest, const void *src, size_t n);
	void UMVMBlockCopy(void *dest, const void *src, size_t n);
	void UMVMAdjustOffset(ItemIndex *Item, int Count, int Adjustment);
	int UMVMFindUnusedItem(ItemIndex *Item, int Count);

	struct HashCodesType
	{
		ArrayIndex SlotSelectorHash;
		ArrayIndex TrancheSelectorHash;
	};

	union KeyHashCodes
	{
		HashCodesType m_HashCodes;
		uint64_t m_xxhash;

		ArrayIndex GetSlotSelectorHash();
		ArrayIndex GetTrancheSelectorHash();
		KeyHashCodes(AccessVectorChar p_Key);
	};
}

#endif