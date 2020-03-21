//UMVMItemReferenceRawBlockGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMItemReferenceRawBlockGenerated_H
#define UMVMItemReferenceRawBlockGenerated_H

namespace UserModeVirtualMemory
{
	class UMVMItemReferenceRawBlock : public UMVMItemReferenceBasic
	{
	friend class UserModeVirtualMemoryFile;

	public:
		explicit UMVMItemReferenceRawBlock(const UMVMItemReferenceDummyType&, char Type);
		~UMVMItemReferenceRawBlock();

	protected:

		explicit UMVMItemReferenceRawBlock(char Type);

		explicit UMVMItemReferenceRawBlock(char Type, ItemReferenceStruct IR);

		UMVMItemReferenceRawBlock();

		UMVMItemReferenceBase* GetNewWorker(char Type, ItemReferenceStruct IR);

 virtual ObjectType GetUMVMItemReferenceObjectType();
 virtual ItemReferenceStruct GetUMVMItemReferenceStruct();
 virtual BlockNumber GetQuantumNumber();
 virtual ValueView GetValueViewToElementInternal(UserModeVirtualMemoryFile* UMVMF);
 virtual bool PutObjectInternal(UserModeVirtualMemoryFile* UMVMF, BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex, LittlePointerBlock& ThisLittlePointerBlock, ArrayIndex LittlePointerIndex, AccessVectorChar ItemToStore, const vector<string>& p_Vector);
 virtual void DeleteObject(UserModeVirtualMemoryFile* UMVMF, BigPointerBlock& ThisBigPointerBlock);
 virtual ValueView GetValueViewToElementInternalPriority(UserModeVirtualMemoryFile* UMVMF);

#include "UMVMItemReferenceRawBlock.inc"
	};
};

#endif
