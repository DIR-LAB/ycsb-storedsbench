//UMVMItemRef.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVM_ITEM_REF_H
#define UMVM_ITEM_REF_H

namespace UserModeVirtualMemory
{

	class ItemReferenceStruct
	{
	public:
		struct UMVMItemReferenceData
		{
			uint32_t m_BlockNumber;
			uint16_t m_UserInfo;
			uint16_t m_ItemNumber : 11;
			ObjectType m_Type : 4;
			uint16_t m_Spare : 1;
		};

		struct LongStringReferenceData
		{//this MUST have the same item layout as UMVMItemReferenceData
			uint32_t m_StartingBlockNumber;
			uint16_t m_Spare1;
			uint16_t m_Spare2 : 11;
			ObjectType m_Type : 4;
			uint16_t m_Spare3 : 1;

		};

	public:

		union
		{
			UMVMItemReferenceData m_IRD;
			LongStringReferenceData m_LSRD;
			uint64_t m_Data;
		};

	public:

		ItemReferenceStruct();

		ItemReferenceStruct(BlockNumber QN, size_t QuantumFreeSpace, size_t ItemNumber, ObjectType Type);

		ItemReferenceStruct(BlockNumber QN, ObjectType Type);

		int	GetItemNumber();
		BlockNumber GetQuantumNumber() const;
		ArrayIndex GetObjectModifiableElementLength();
		uint16_t GetUserInfo();
		ObjectType GetObjectType();
	};
}
#endif