//UMVMDAMForMappedBlocksGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMDAMForMappedBlocks_H
#define UMVMDAMForMappedBlocks_H

namespace UserModeVirtualMemory
{
	class UMVMDAMForMappedBlocks : public UMVMDAMBasic
	{
	friend class UserModeVirtualMemoryFile;

	public:
		explicit UMVMDAMForMappedBlocks(const UMVMDAMDummyType&, char Type);
		~UMVMDAMForMappedBlocks();

	protected:

		explicit UMVMDAMForMappedBlocks(char Type);

		explicit UMVMDAMForMappedBlocks(UserModeVirtualMemoryFile* umvmf, char Type);

		UMVMDAMForMappedBlocks();

		UMVMDAMBase* GetNewWorker(UserModeVirtualMemoryFile* umvmf, char Type);

		char* GetAddressInMappedFile(uint64_t OffsetInFile, uint64_t DataLength, bool Appending) const;
		void SetRelativeAddressLimit(BigArrayIndex RelativeAddressLimit) const;
		BigArrayIndex GetRelativeAddressLimit() const;
		BigArrayIndex GetSize() const;
		void Flush();
		bool WeCanFlush();
////		@PublicVirtualUserFunctionReturnType3@ @PublicVirtualUserFunction3@(@PublicVirtualUserFunctionParameterListDeclarations3@);
////		@PublicVirtualUserFunctionReturnType4@ @PublicVirtualUserFunction4@(@PublicVirtualUserFunctionParameterListDeclarations4@);
		Block* MakeBlockResident(BlockNumber BlockNumberToMakeResident);
		AccessVectorChar ReadOverflowArea(ShortIndirectionInfoStruct* Info);
		void UpdateOverflowArea(ShortIndirectionInfoStruct* Info, AccessVectorChar OverflowBuffer);
		void Delete();
////		@PublicVirtualUserFunctionReturnType9@ @PublicVirtualUserFunction9@(@PublicVirtualUserFunctionParameterListDeclarations9@);
////		@PublicVirtualUserFunctionReturnType10@ @PublicVirtualUserFunction10@(@PublicVirtualUserFunctionParameterListDeclarations10@);
////		@PublicVirtualUserFunctionReturnType11@ @PublicVirtualUserFunction11@(@PublicVirtualUserFunctionParameterListDeclarations11@);
////		@PublicVirtualUserFunctionReturnType12@ @PublicVirtualUserFunction12@(@PublicVirtualUserFunctionParameterListDeclarations12@);
////		@PublicVirtualUserFunctionReturnType13@ @PublicVirtualUserFunction13@(@PublicVirtualUserFunctionParameterListDeclarations13@);
////		@PublicVirtualUserFunctionReturnType14@ @PublicVirtualUserFunction14@(@PublicVirtualUserFunctionParameterListDeclarations14@);
////		@PublicVirtualUserFunctionReturnType15@ @PublicVirtualUserFunction15@(@PublicVirtualUserFunctionParameterListDeclarations15@);
////		@PublicVirtualUserFunctionReturnType16@ @PublicVirtualUserFunction16@(@PublicVirtualUserFunctionParameterListDeclarations16@);
////		@PublicVirtualUserFunctionReturnType17@ @PublicVirtualUserFunction17@(@PublicVirtualUserFunctionParameterListDeclarations17@);
////		@PublicVirtualUserFunctionReturnType18@ @PublicVirtualUserFunction18@(@PublicVirtualUserFunctionParameterListDeclarations18@);
		bool SetupReadOnly(const string& PrimaryFileName, const string& SecondaryFileName, UserModeVirtualMemory::UMVMFileType FileType);
		bool SetupReadWrite(const string& PrimaryFileName, const string& SecondaryFileName, UserModeVirtualMemory::UMVMFileType FileType, bool Temporary);
		bool CreateNewFile(const string& PrimaryFileName, const string& SecondaryFileName);
		void AllocateBuffers();
////		@PublicVirtualUserFunctionReturnType23@ @PublicVirtualUserFunction23@(@PublicVirtualUserFunctionParameterListDeclarations23@);
////		@PublicVirtualUserFunctionReturnType24@ @PublicVirtualUserFunction24@(@PublicVirtualUserFunctionParameterListDeclarations24@);
		void CloseHelper();
		ValueView TryToMapConsecutiveData(vector<BlockNumber>& BlockNumbers, BlockNumber StartingBlockNumber, LongStringStorageParameters LSP, BigArrayIndex OffsetInBlock, BigArrayIndex ElementSize, ObjectType OT);
////		@PublicVirtualUserFunctionReturnType27@ @PublicVirtualUserFunction27@(@PublicVirtualUserFunctionParameterListDeclarations27@);
////		@PublicVirtualUserFunctionReturnType28@ @PublicVirtualUserFunction28@(@PublicVirtualUserFunctionParameterListDeclarations28@);
////		@PublicVirtualUserFunctionReturnType29@ @PublicVirtualUserFunction29@(@PublicVirtualUserFunctionParameterListDeclarations29@);
////		@PublicVirtualUserFunctionReturnType30@ @PublicVirtualUserFunction30@(@PublicVirtualUserFunctionParameterListDeclarations30@);
////		@PublicVirtualUserFunctionReturnType31@ @PublicVirtualUserFunction31@(@PublicVirtualUserFunctionParameterListDeclarations31@);
////		@PublicVirtualUserFunctionReturnType32@ @PublicVirtualUserFunction32@(@PublicVirtualUserFunctionParameterListDeclarations32@);
////		@PublicVirtualUserFunctionReturnType33@ @PublicVirtualUserFunction33@(@PublicVirtualUserFunctionParameterListDeclarations33@);
#include "UMVMDAMForMappedBlocks.inc"
	};
};

#endif
