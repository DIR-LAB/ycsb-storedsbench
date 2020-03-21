//ThreeMissesCIterProGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ThreeMissesCIterPro_H
#define ThreeMissesCIterPro_H

namespace UserModeVirtualMemory
{
	class ThreeMissesCIterPro : public ThreeMissesCIterBasic
	{

	public:
		ThreeMissesCIterPro(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV,	UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIterPro& StringMap);

		ThreeMissesCIterPro(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV,	UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesPro& StringMap);

		ThreeMissesCIterBase operator++();
		value_type operator*() const;
		
		pair<AccessVectorChar,AccessVectorChar> GetKeyAndValueFromIterator() const;

		explicit ThreeMissesCIterPro();

		~ThreeMissesCIterPro();
		
		static const string WorkerType;

		ThreeMissesCIterBase cbegin() const;
		ThreeMissesCIterBase cend() const;

		ThreeMissesCIterBase begin() const;
		ThreeMissesCIterBase end() const;

	protected:

		ThreeMissesCIterBase* GetNewWorker(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIterPro& StringMap);
		ThreeMissesCIterBase* GetNewWorker();

	
	private:
		ThreeMissesCIterPro(const ThreeMissesCIterPro&) = delete;
		ThreeMissesCIterPro& operator=(const ThreeMissesCIterPro&) = delete;
		ThreeMissesCIterPro& operator=(ThreeMissesCIterPro&& other) = delete;
		
#include "ThreeMissesCIterPro.inc"
	};
};

#endif
