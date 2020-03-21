//UMVMItemReferenceShortStringGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMItemReferenceShortStringGenerated_H
#define UMVMItemReferenceShortStringGenerated_H

namespace UserModeVirtualMemory
{
	class UMVMItemReferenceShortString : public UMVMItemReferenceBasic
	{
	friend class UserModeVirtualMemoryFile;

	public:
		explicit UMVMItemReferenceShortString(const UMVMItemReferenceDummyType&, char Type);
		~UMVMItemReferenceShortString();

	protected:

		explicit UMVMItemReferenceShortString(char Type);

		explicit UMVMItemReferenceShortString(char Type, ItemReferenceStruct IR);

		UMVMItemReferenceShortString();

		UMVMItemReferenceBase* GetNewWorker(char Type, ItemReferenceStruct IR);

 virtual ObjectType GetUMVMItemReferenceObjectType();
 virtual ItemReferenceStruct GetUMVMItemReferenceStruct();
 virtual BlockNumber GetQuantumNumber();
 virtual ValueView GetValueViewToElementInternal(UserModeVirtualMemoryFile* UMVMF);
 virtual bool PutObjectInternal(UserModeVirtualMemoryFile* UMVMF, BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex, LittlePointerBlock& ThisLittlePointerBlock, ArrayIndex LittlePointerIndex, AccessVectorChar ItemToStore, const vector<string>& p_Vector);
 virtual void DeleteObject(UserModeVirtualMemoryFile* UMVMF, BigPointerBlock& ThisBigPointerBlock);
 virtual ValueView GetValueViewToElementInternalPriority(UserModeVirtualMemoryFile* UMVMF);

#include "UMVMItemReferenceShortString.inc"
	};
};

#endif
