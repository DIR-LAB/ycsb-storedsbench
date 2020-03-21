//UMVMBlocki.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef BLOCKI_H
#define BLOCKI_H

#include "UMVMTypes.h"
#include "UMVMObjectType.h"
#include "UMVMItemReferenceDefinitions.h"
#include "UMVMItemRef.h"

namespace UserModeVirtualMemory
{
	static const int PartialFlushMaxCount = 16;
	static const int CacheLineLength = 64;

	const int FreeSpaceUniqueValues = 1 << QuantumFreeSpaceBits;
	const ArrayIndex Margin = 128;
	const uint16_t AvailableQuantum = FreeSpaceUniqueValues - 1;
	const ArrayIndex BadArrayIndex = ArrayIndex(-1);
	const BigArrayIndex BadBigArrayIndex = BigArrayIndex(-1);
	const BlockNumber NoBlockNumber = BlockNumber(0);
	const int BigBlockBufferCount = 1024; 
	const int StartingQuantumOffset = BlockSize;
	const int QuantumStartingBlock = 1;

	const size_t PhysicalMemoryMargin = 1000 * 1000 * 1000ULL;

	const uint64_t TimestampHighBit = UINT64_C(1) << 63;

	struct LongStringStorageParameters
	{
		ArrayIndex m_HeaderLength;
		ArrayIndex m_HeaderNeedsThisManyBlocks;
		ArrayIndex m_EntireStringNeedsThisManyBlocks;
	};

	struct ItemIndex
	{
		uint16_t m_Offset : 15;
		uint16_t m_InUse : 1;
	};

	typedef AccessVector<ItemReferenceStruct> LittlePointerArray;

	struct UserModeVirtualMemoryFileHeaderStruct
	{
		char FileFormatVersion[64];
		BlockNumber MainDirectoryStartingQuantumNumber;
		BlockNumber NextQuantumNumberInFile;
		char RelativeItemBits;
		char QuantumNumberBits;
		char QuantumFreeSpaceBits;
		char ThreeMissesFileType;
		bool TransactionInProgress;
		ArrayIndex CurrentBlockNumberForShortOverflowElements;
		ArrayIndex OffsetInCurrentBlockForShortOverflowElements;
		ArrayIndex ReservedBufferCount;
		ArrayIndex ContiguousBufferCount;
		ArrayIndex BufferCount;
		BigArrayIndex RelativeAddressLimit;
		BigArrayIndex SlotCount;
		ArrayIndex RehashedRootSlotCount;
		ArrayIndex BaseToCalculateRawBlockNumberForSlot;
		int NumberOfRootSlotsNotYetRehashed;
		ArrayIndex BlocksPerSlot;
		UserModeVirtualMemoryFileHeaderStruct()
			: MainDirectoryStartingQuantumNumber(0), NextQuantumNumberInFile(0), RelativeItemBits(0), QuantumNumberBits(0)
			, QuantumFreeSpaceBits(0), ThreeMissesFileType(0), TransactionInProgress(false)
			, CurrentBlockNumberForShortOverflowElements(0)
			, OffsetInCurrentBlockForShortOverflowElements(0), FileFormatVersion()
			, ReservedBufferCount(0), ContiguousBufferCount(0), BufferCount(0)
			, RelativeAddressLimit(0), SlotCount(0), RehashedRootSlotCount(0), BaseToCalculateRawBlockNumberForSlot(0)
			, NumberOfRootSlotsNotYetRehashed(0), BlocksPerSlot(1)
		{}
	};

	class UMVMDAM;

	class UserModeVirtualMemoryFileHeader
	{
	private:
		struct UMVMFHData
		{
			char m_FileFormatVersion[8];
			BlockNumber m_MainDirectoryStartingQuantumNumber;
			BlockNumber m_NextQuantumNumberInFile;
			char m_RelativeItemBits;
			char m_QuantumNumberBits;
			char m_QuantumFreeSpaceBits;
			char m_ThreeMissesFileType;
			bool m_TransactionInProgress;
			ArrayIndex m_CurrentBlockNumberForShortOverflowElements;
			BlockOccupiedLengthType m_OffsetInCurrentBlockForShortOverflowElements;
			ArrayIndex m_ContiguousBufferCount;
			BigArrayIndex m_RelativeAddressLimit;
			BigArrayIndex m_SlotCount;
			ArrayIndex m_RehashedRootSlotCount;
			ArrayIndex m_BaseToCalculateRawBlockNumberForSlot;
			int m_NumberOfRootSlotsNotYetRehashed;
			ArrayIndex m_BlocksPerSlot;

		};

		UMVMFHData* m_Data;

	public:

		void CheckBase() const;

		void SetFileFormatVersion(char* FileFormatVersion)
        {
#if UMVM_WINDOWS
            strcpy_s(m_Data->m_FileFormatVersion, FileFormatVersion); 
#else
            strcpy(m_Data->m_FileFormatVersion, FileFormatVersion); 
#endif
            CheckBase();
            
        }
		void SetMainDirectoryStartingQuantumNumber(BlockNumber MainDirectoryStartingQuantumNumber) { m_Data->m_MainDirectoryStartingQuantumNumber = MainDirectoryStartingQuantumNumber; CheckBase(); }
		void SetNextQuantumNumberInFile(BlockNumber NextQuantumNumberInFile) { m_Data->m_NextQuantumNumberInFile = NextQuantumNumberInFile; CheckBase(); }
		void SetRelativeItemBits(char RelativeItemBits) { m_Data->m_RelativeItemBits = RelativeItemBits; CheckBase(); }
		void SetQuantumNumberBits(char QuantumNumberBits) { m_Data->m_QuantumNumberBits = QuantumNumberBits; CheckBase(); }
		void SetQuantumFreeSpaceBits(char QuantumFreeSpaceBits) { m_Data->m_QuantumFreeSpaceBits = QuantumFreeSpaceBits; CheckBase(); }
		void SetThreeMissesFileType(char ThreeMissesFileType) { m_Data->m_ThreeMissesFileType = ThreeMissesFileType; CheckBase(); }
		void SetTransactionInProgress(bool TransactionInProgress) { m_Data->m_TransactionInProgress = TransactionInProgress; CheckBase(); }
		void SetCurrentBlockNumberForShortOverflowElements(ArrayIndex CurrentBlockNumberForShortOverflowElements) { m_Data->m_CurrentBlockNumberForShortOverflowElements = CurrentBlockNumberForShortOverflowElements; CheckBase(); }
		void SetOffsetInCurrentBlockForShortOverflowElements(BlockOccupiedLengthType OffsetInCurrentBlockForShortOverflowElements) { m_Data->m_OffsetInCurrentBlockForShortOverflowElements = OffsetInCurrentBlockForShortOverflowElements; CheckBase(); }
		void SetContiguousBufferCount(ArrayIndex ContiguousBufferCount) { m_Data->m_ContiguousBufferCount = ContiguousBufferCount; CheckBase(); }
		void SetRelativeAddressLimit(BigArrayIndex RelativeAddressLimit) { m_Data->m_RelativeAddressLimit = RelativeAddressLimit; CheckBase(); }
		void SetSlotCount(BigArrayIndex SlotCount) { m_Data->m_SlotCount = SlotCount; CheckBase(); }
		void SetRehashedRootSlotCount(ArrayIndex RehashedRootSlotCount) { m_Data->m_RehashedRootSlotCount = RehashedRootSlotCount; CheckBase(); }

		void InitializeBaseToCalculateRawBlockNumberForSlot()
		{
			m_Data->m_BaseToCalculateRawBlockNumberForSlot = m_Data->m_NextQuantumNumberInFile; 
		}

		void SetNumberOfRootSlotsNotYetRehashed(int NumberOfRootSlotsNotYetRehashed) { CheckBase(); m_Data->m_NumberOfRootSlotsNotYetRehashed = NumberOfRootSlotsNotYetRehashed; }
		void SetBlocksPerSlot(ArrayIndex BlocksPerSlot) { CheckBase(); m_Data->m_BlocksPerSlot = BlocksPerSlot; }

		char* GetFileFormatVersion() const { CheckBase(); return m_Data->m_FileFormatVersion; }
		BlockNumber GetMainDirectoryStartingQuantumNumber() const { CheckBase(); return m_Data->m_MainDirectoryStartingQuantumNumber; }
		BlockNumber GetNextQuantumNumberInFile() const { CheckBase(); return m_Data->m_NextQuantumNumberInFile; }
		char GetRelativeItemBits() const { CheckBase(); return m_Data->m_RelativeItemBits; }
		char GetQuantumNumberBits() const { CheckBase(); return m_Data->m_QuantumNumberBits; }
		char GetQuantumFreeSpaceBits() const { CheckBase(); return m_Data->m_QuantumFreeSpaceBits; }
		char GetThreeMissesFileType() const { CheckBase(); return m_Data->m_ThreeMissesFileType; }
		bool GetTransactionInProgress() const { CheckBase(); return m_Data->m_TransactionInProgress; }
		ArrayIndex GetCurrentBlockNumberForShortOverflowElements() const { CheckBase(); return m_Data->m_CurrentBlockNumberForShortOverflowElements; }
		BlockOccupiedLengthType GetOffsetInCurrentBlockForShortOverflowElements() const { CheckBase(); return m_Data->m_OffsetInCurrentBlockForShortOverflowElements; }
		ArrayIndex GetContiguousBufferCount() const { CheckBase(); return m_Data->m_ContiguousBufferCount; }
		BigArrayIndex GetRelativeAddressLimit() const { CheckBase(); return m_Data->m_RelativeAddressLimit; }
		BigArrayIndex GetSlotCount() { CheckBase(); return m_Data->m_SlotCount; }
		ArrayIndex GetRehashedRootSlotCount() const { CheckBase(); return m_Data->m_RehashedRootSlotCount; }
		ArrayIndex GetBaseToCalculateRawBlockNumberForSlot() const { CheckBase(); return m_Data->m_BaseToCalculateRawBlockNumberForSlot; }
		int GetNumberOfRootSlotsNotYetRehashed() const { CheckBase(); return m_Data->m_NumberOfRootSlotsNotYetRehashed; }
		ArrayIndex GetBlocksPerSlot() const { CheckBase(); return m_Data->m_BlocksPerSlot; }

		BlockNumber GetNextQuantumNumberAndPostIncrement() { CheckBase(); return m_Data->m_NextQuantumNumberInFile++; }

		UserModeVirtualMemoryFileHeader()
			: m_Data(nullptr)
		{
		}

		void BaseHeader(UMVMDAM& DataAccessManager);
	};

	struct BigArrayHeader
	{
		BigArrayIndex m_ElementCount;
		BigArrayIndex m_MaxElementCount;
		BlockNumber m_LastQuantumAddedTo;
		BlockNumber m_LastQuantumDeletedFrom;
		BlockNumber m_LastNewQuantumAllocated;
		BigArrayHeader()
			: m_ElementCount(0), m_MaxElementCount(0), m_LastQuantumAddedTo(0), m_LastQuantumDeletedFrom(0)
			, m_LastNewQuantumAllocated(0)
			{}
	};

	struct QuantumBlockHeader
	{
		BigArrayIndex m_ItemCount;
	};

	const ArrayIndex MaxItemSize = BlockSize - (sizeof(QuantumBlockHeader) + Margin);

	const int UMVMItemReferencesPerBlock = BlockSize / sizeof(ItemReferenceStruct);

	const ArrayIndex QuantumNumbersPerBlock = (MaxItemSize-sizeof(BigArrayHeader)) / sizeof(BlockNumber);

	struct QuantumBlockStruct
	{
		QuantumBlockHeader m_QuantumBlockHeader;
		ItemIndex m_ItemIndex[1]; // place holder for item index array
	};

	union Block
	{
		char m_Data[BlockSize];
		char m_QuantumVersionInfo[64];
		QuantumBlockStruct m_QuantumBlockData;
	};

	class BaseBlock
	{
	protected:
		Block *m_BlockBuffer;
		BlockNumber m_BlockNumber;
		UserModeVirtualMemoryFile *m_QF;
		BaseBlock();
		BaseBlock(const BlockNumber BN, UserModeVirtualMemoryFile* UMVMF) : m_BlockNumber(BN), m_QF(UMVMF), m_BlockBuffer(nullptr) {}
	};

	class QuantumBlockPtr;

	class QuantumBlock : public BaseBlock
	{
		friend QuantumBlockPtr;
	protected:
		ItemIndex *m_ItemIndex;

		struct GetItemInternalSetupReturnType
		{
			char* StartingLocation;
			size_t EndingPositionInBlock;
			size_t Length;
		};

		GetItemInternalSetupReturnType GetItemInternalSetup(ArrayIndex p_ItemNumber);

	public:

		QuantumBlock();
		QuantumBlock(const BlockNumber BN, UserModeVirtualMemoryFile* UMVMF);

		void SetItemCount(ArrayIndex p_NewItemCount);
		void SetToKnownState();
		ArrayIndex GetItemCount();
		AccessVectorChar GetModifiableItem(ArrayIndex p_ItemNumber);
		AccessVectorChar GetReadableItem(ArrayIndex p_ItemNumber);
		ArrayIndex GetItemLength(ArrayIndex p_ItemNumber);

		void ReplaceItem(ArrayIndex p_ItemNumber, AccessVectorChar p_NewItem);
		ItemIndex GetItemIndexEntry(ArrayIndex p_ItemNumber);
		ArrayIndex CalculateFreeSpace();
		void DeleteItem(ArrayIndex p_ItemNumber);
		ArrayIndex AddItem(AccessVectorChar Item, bool FillAndExtendToReservedSize = false, char Fill = 0);
		void Clear();
		void ClearHeader();
		void TidyBlock();

		char* GetBufferAddress();

		void CheckItemIndex();
		void CheckItemIndex(ArrayIndex ItemNumber);

	protected:
		AccessVectorChar GetItemInternal(ArrayIndex p_ItemNumber);
	};

	class BigPointerBlock : public QuantumBlock
	{
	protected:
		BigArrayHeader *m_BigArrayHeader;
		BigPointerArray m_BigPointerArray;

	public:
		BigPointerBlock();
		BigPointerBlock& operator =(const BigPointerBlock& other);
		BigPointerBlock(const BlockNumber BN, UserModeVirtualMemoryFile* UMVMF, bool Initial=false);
		~BigPointerBlock();

		BigPointerArray &GetBigPointerArray() { return m_BigPointerArray; };
		void SetBigArrayHeader();
		void SetBigPointerArray();

		BigArrayIndex GetBigArrayElementCount();
		void SetBigArrayElementCount(BigArrayIndex p_ElementCount);
		BigArrayIndex GetBigArrayMaxElementCount();
		void SetBigArrayMaxElementCount(BigArrayIndex p_ElementCount);

		BlockNumber GetBigArrayElement(BigArrayIndex p_Index) const;
		void SetBigArrayElement(BigArrayIndex p_Index, BlockNumber p_Element);

		BlockNumber GetLastQuantumAddedTo();
		void SetLastQuantumAddedTo(BlockNumber p_QuantumNumber);

		BlockNumber GetLastQuantumDeletedFrom();
		void SetLastQuantumDeletedFrom(BlockNumber p_QuantumNumber);

		BlockNumber GetLastNewQuantumAllocated();
		void SetLastNewQuantumAllocated(BlockNumber p_QuantumNumber);
	};

	class LittlePointerBlock : public QuantumBlock
	{
	protected:
		LittlePointerArray m_LittlePointerArray;
		void SetLittlePointerArray();
		void ClearLittleArray();
	public:
	LittlePointerBlock(const BlockNumber BN, UserModeVirtualMemoryFile* UMVMF);
		~LittlePointerBlock();
	/*__declspec(noinline)*/ ItemReferenceStruct GetLittleArrayElement(BlockOccupiedLengthType p_Index);
		AccessVector<ItemReferenceStruct> GetLittleArrayAccessVector();
		void SetLittleArrayElement(ArrayIndex p_Index, ItemReferenceStruct p_UMVMItemReference);
	};

	class LeafBlock : public QuantumBlock
	{
	public:
		LeafBlock();
//		LeafBlock& operator =(const LeafBlock& other);
		LeafBlock(const BlockNumber BN, UserModeVirtualMemoryFile* UMVMF);
		~LeafBlock();
	private:
		static int s_SequenceNumber;
		int m_SequenceNumber;
	};

	class QuantumBlockPtr
	{
	protected:
		QuantumBlock m_QuantumBlock;
	public:
		QuantumBlockPtr(const BlockNumber p_BlockNumber, UserModeVirtualMemoryFile *p_BlockManager);
		QuantumBlockPtr();
		QuantumBlock *operator->();
	};

}
#endif
