//ThreeMissesCIterFreeGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ThreeMissesCIterFree_H
#define ThreeMissesCIterFree_H

namespace UserModeVirtualMemory
{
	class ThreeMissesCIterFree : public ThreeMissesCIterBasic
	{

	public:
		ThreeMissesCIterFree(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV,	UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIterFree& StringMap);

		ThreeMissesCIterFree(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV,	UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesFree& StringMap);

		ThreeMissesCIterBase operator++();
		value_type operator*() const;
		
		pair<AccessVectorChar,AccessVectorChar> GetKeyAndValueFromIterator() const;

		explicit ThreeMissesCIterFree();

		~ThreeMissesCIterFree();
		
		static const string WorkerType;

		ThreeMissesCIterBase cbegin() const;
		ThreeMissesCIterBase cend() const;

		ThreeMissesCIterBase begin() const;
		ThreeMissesCIterBase end() const;

	protected:

		ThreeMissesCIterBase* GetNewWorker(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIterFree& StringMap);
		ThreeMissesCIterBase* GetNewWorker();

	
	private:
		ThreeMissesCIterFree(const ThreeMissesCIterFree&) = delete;
		ThreeMissesCIterFree& operator=(const ThreeMissesCIterFree&) = delete;
		ThreeMissesCIterFree& operator=(ThreeMissesCIterFree&& other) = delete;
		
#include "ThreeMissesCIterFree.inc"
	};
};

#endif
