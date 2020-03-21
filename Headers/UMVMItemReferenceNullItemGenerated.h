//UMVMItemReferenceNullItemGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMItemReferenceNullItemGenerated_H
#define UMVMItemReferenceNullItemGenerated_H

namespace UserModeVirtualMemory
{
	class UMVMItemReferenceNullItem : public UMVMItemReferenceBasic
	{
	friend class UserModeVirtualMemoryFile;

	public:
		explicit UMVMItemReferenceNullItem(const UMVMItemReferenceDummyType&, char Type);
		~UMVMItemReferenceNullItem();

	protected:

		explicit UMVMItemReferenceNullItem(char Type);

		explicit UMVMItemReferenceNullItem(char Type, ItemReferenceStruct IR);

		UMVMItemReferenceNullItem();

		UMVMItemReferenceBase* GetNewWorker(char Type, ItemReferenceStruct IR);

 virtual ObjectType GetUMVMItemReferenceObjectType();
 virtual ItemReferenceStruct GetUMVMItemReferenceStruct();
 virtual BlockNumber GetQuantumNumber();
 virtual ValueView GetValueViewToElementInternal(UserModeVirtualMemoryFile* UMVMF);
 virtual bool PutObjectInternal(UserModeVirtualMemoryFile* UMVMF, BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex, LittlePointerBlock& ThisLittlePointerBlock, ArrayIndex LittlePointerIndex, AccessVectorChar ItemToStore, const vector<string>& p_Vector);
 virtual void DeleteObject(UserModeVirtualMemoryFile* UMVMF, BigPointerBlock& ThisBigPointerBlock);
 virtual ValueView GetValueViewToElementInternalPriority(UserModeVirtualMemoryFile* UMVMF);

#include "UMVMItemReferenceNullItem.inc"
	};
};

#endif
