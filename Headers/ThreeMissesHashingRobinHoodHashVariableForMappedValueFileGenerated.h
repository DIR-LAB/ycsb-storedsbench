//ThreeMissesHashingRobinHoodHashVariableForMappedValueFileGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ThreeMissesHashingRobinHoodHashVariableForMappedValueFile_H
#define ThreeMissesHashingRobinHoodHashVariableForMappedValueFile_H

namespace UserModeVirtualMemory
{
	using namespace ThreeMissesTypeDefinitions;
	class ThreeMissesHashingRobinHoodHashVariableForMappedValueFile : public ThreeMissesHashingBasic
	{
	friend class ThreeMisses;

	public:
		explicit ThreeMissesHashingRobinHoodHashVariableForMappedValueFile(const ThreeMissesHashingDummyType&, char Type);
		~ThreeMissesHashingRobinHoodHashVariableForMappedValueFile();

	protected:

		explicit ThreeMissesHashingRobinHoodHashVariableForMappedValueFile(char Type);

		explicit ThreeMissesHashingRobinHoodHashVariableForMappedValueFile(AccessVectorChar AV, BigArrayIndex KeyLength, BigArrayIndex DataLength, BigArrayIndex SlotCount, char Type);

		ThreeMissesHashingRobinHoodHashVariableForMappedValueFile();

		ThreeMissesHashingBase* GetNewWorker(AccessVectorChar AV, BigArrayIndex KeyLength, BigArrayIndex DataLength, BigArrayIndex SlotCount, char Type);

		BigArrayIndex GetInlineRecordLength();
		ArrayIndex GetAdditionalLengthToAllocateForAV();
		ArrayIndex GetSlotOccupiedElementCount(AccessVectorChar AV);
		size_t GetTotalSearchCount();
		void IncrementSlotOccupiedElementCount(AccessVectorChar AV);
		ArrayIndex GetSlotAllocationUnit();
		ValueView SetupHashAV(AccessVectorChar AV, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile* IndexFile);
////		@PublicVirtualUserFunctionReturnType8@ @PublicVirtualUserFunction8@(@PublicVirtualUserFunctionParameterListDeclarations8@);
////		@PublicVirtualUserFunctionReturnType9@ @PublicVirtualUserFunction9@(@PublicVirtualUserFunctionParameterListDeclarations9@);
		void SetUpSlotForwardingToMultipleRawBlockSlotInNewFile(AccessVectorChar OldSlot);
		void SetAccessVector(AccessVectorChar AV);
		AccessVectorChar GetAccessVector();
		bool DeleteElement(const AccessVectorChar Key, KeyHashCodes HashCodes, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		uint8_t GetMaximumSearchLength(AccessVectorChar AV);
		void PutMaximumSearchLength(uint8_t NewMaximumLength, AccessVectorChar AV);
		ValueView CreateNewIndexForElement(AccessVectorChar Key, KeyHashCodes HashCodes, AccessVectorChar Data, RecordInfo IndirectInfo, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		AccessVectorChar FindIndexForExistingElement(AccessVectorChar Key, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		size_t GetAVSize();
		bool RehashSlot(AccessVectorChar OldSlot, AccessVectorChar NewSlot, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		RecordInfo GetRecordInfoForExistingElement(AccessVectorChar Key, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
////		@PublicVirtualUserFunctionReturnType21@ @PublicVirtualUserFunction21@(@PublicVirtualUserFunctionParameterListDeclarations21@);
		ValueView SetupHashAVInternal(AccessVectorChar AV, size_t StartingPosition, UserModeVirtualMemoryFile* IndexFile) const;

////		@PublicUserFunctionReturnType1@ @PublicUserFunction1@(@PublicUserParameterListDeclarations1@);
		#include "ThreeMissesHashingRobinHoodHashVariableForMappedValueFile.inc"
	};
};

#endif
