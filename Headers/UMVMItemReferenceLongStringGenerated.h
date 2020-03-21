//UMVMItemReferenceLongStringGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMItemReferenceLongStringGenerated_H
#define UMVMItemReferenceLongStringGenerated_H

namespace UserModeVirtualMemory
{
	class UMVMItemReferenceLongString : public UMVMItemReferenceBasic
	{
	friend class UserModeVirtualMemoryFile;

	public:
		explicit UMVMItemReferenceLongString(const UMVMItemReferenceDummyType&, char Type);
		~UMVMItemReferenceLongString();

	protected:

		explicit UMVMItemReferenceLongString(char Type);

		explicit UMVMItemReferenceLongString(char Type, ItemReferenceStruct IR);

		UMVMItemReferenceLongString();

		UMVMItemReferenceBase* GetNewWorker(char Type, ItemReferenceStruct IR);

 virtual ObjectType GetUMVMItemReferenceObjectType();
 virtual ItemReferenceStruct GetUMVMItemReferenceStruct();
 virtual BlockNumber GetQuantumNumber();
 virtual ValueView GetValueViewToElementInternal(UserModeVirtualMemoryFile* UMVMF);
 virtual bool PutObjectInternal(UserModeVirtualMemoryFile* UMVMF, BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex, LittlePointerBlock& ThisLittlePointerBlock, ArrayIndex LittlePointerIndex, AccessVectorChar ItemToStore, const vector<string>& p_Vector);
 virtual void DeleteObject(UserModeVirtualMemoryFile* UMVMF, BigPointerBlock& ThisBigPointerBlock);
 virtual ValueView GetValueViewToElementInternalPriority(UserModeVirtualMemoryFile* UMVMF);

#include "UMVMItemReferenceLongString.inc"
	};
};

#endif
