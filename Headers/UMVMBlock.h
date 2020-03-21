//UMVMBlock.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMBLOCK_H
#define UMVMBLOCK_H

#include <map>

//#define ONE_LOOKASIDE_SLOT

namespace UserModeVirtualMemory
{
	class UMVMMemoryMapping;

    
	class  UserModeVirtualMemoryFile
	{
		friend class UMVMVectorInternal;
		friend class LittlePointerBlock;
		friend class LeafBlock;
		friend class BigPointerBlock;

		friend class QuantumBlockPtr;
		friend class StringVector;
		friend class ValueView;
		friend class ThreeMissesBasic;
		friend class ThreeMissesFree;
		friend class ThreeMissesPro;
		friend class ThreeMisses;

		friend class UMVMDAMForMappedBlocks;
		friend class RecordInfo;
		friend class UserModeVirtualMemoryFileHeader;
		friend class ThreeMissesUtilities;

	public:

		static const size_t DefaultMapReservation = 1LL << 42;

		static const int s_DefaultBufferCount = 32000;
		static const int s_MinimumBlockCountForThreeMisses = 100;

		class ThreeMissesFileOptions
		{
			friend class ThreeMissesPro;
			friend class ThreeMissesFree;

		private:
			string m_PrimaryFileName;
			size_t m_PrimaryStorageAllocation;
			string m_SecondaryFileName;
			bool m_ReadOnly;
			UMVMFileType m_FileType;
			bool m_ThrowOnExhaustion;
			bool m_SetNewBlocksToKnownState;
			BigArrayIndex m_SlotCount;
			ArrayIndex m_BlocksPerSlot;
			bool m_IndexFileInRAMWasRequested;
			LPPROGRESS_ROUTINE m_ProgressRoutine;

			//value files only
			ThreeMissesFileOptions(const string& PrimaryFileName,
				const string& SecondaryFileName, bool ReadOnly = false);

		public:
			ThreeMissesFileOptions();

			ThreeMissesFileOptions(const string& PrimaryFileName);

#ifndef FREE_VERSION
		ThreeMissesFileOptions(const string& PrimaryFileName, size_t PrimaryStorageAllocation,
				const string& SecondaryFileName = "", bool ReadOnly = false,
				bool ThrowOnExhaustion = true, bool SetNewBlocksToKnownState = false,
				BigArrayIndex SlotCount = 0, ArrayIndex BlocksPerSlot = 1, bool IndexFileInRAMWasRequested = false,
				LPPROGRESS_ROUTINE ProgressRoutine = nullptr);
#endif

			void SetSlotCount(BigArrayIndex SlotCount);

			string GetPrimaryFileName() const;
			string GetSecondaryFileName() const;
			BigArrayIndex GetSlotCount() const;
			ArrayIndex GetBlocksPerSlot() const;
			size_t GetPrimaryStorageAllocation() const;
			bool GetSetNewBlocksToKnownState() const;
			bool GetReadOnly() const;
			bool GetThrowOnExhaustion() const;
			UMVMFileType GetFileType() const;
			bool IndexFileInRAMWasRequested() const;
			LPPROGRESS_ROUTINE GetProgressRoutine() const;
		};

		UserModeVirtualMemoryFile(const string& FileName);

		UserModeVirtualMemoryFile(bool ReadOnly, const string& FileName);

		~UserModeVirtualMemoryFile();

		UserModeVirtualMemoryFile& operator =(UserModeVirtualMemoryFile&& other) noexcept;

		vector<pair<BlockNumber, string>> GetDirectoryList();
		bool Open();

		void PersistFreeLists(UserModeVirtualMemoryFile* UMVMF);

		void Close();
		void Delete();
		long GetReadCount() { return m_ReadCount; }
		long GetWriteCount() { return m_WriteCount; }
		int64_t GetCounter();
		static int GetBlockSize();

		bool IsReadOnly();

		bool ErrorHasOccurred();
		string GetErrorMessage();

		size_t GetFreeQuantumCount();
		string GetPrimaryFileName();
		string GetSecondaryFileName();

#if TRACK_STORAGE_ACCESS_TIMES
		vector<uint64_t> GetReadTimes();
		vector<uint64_t> GetWriteTimes();
#endif

		static int GetSVNRevisionNumber();

		BlockNumber FindObjectByName(const string& p_ObjectName);
		ArrayIndex GetMaximumSimpleVectorElementNumber();
		BlockNumber CreateObjectAndAddToDirectory(const string& Name, BigArrayIndex p_MaxElementCount, bool OverrideMaximumCount);
		BigArrayIndex GetObjectElementCount(BlockNumber p_BPAQuantumNumber);
		bool OpenObject(BlockNumber p_ObjectNumber);
		bool CloseObject(BlockNumber p_ObjectNumber);
		void SetObjectElementCount(BlockNumber p_BPAQuantumNumber, BigArrayIndex NewElementCount);
		BlockNumber FindEmptyBlock();
		QuantumBlockPtr MakeQuantumBlockPtr(BlockNumber p_Index);
		void DeleteLongString(ItemReferenceStruct ElementRefStruct);
		void FreeBlocks(vector<BlockNumber> BlockNumbersToFree);
		void SetRelativeAddressLimitFromQFHS();
		void IncrementRehashedRootSlotCount();
		ArrayIndex GetRehashedRootSlotCount();
		bool DecrementCountOfRootSlotsNotYetRehashedAndReportIfZero();
		void InitializeNumberOfRootSlotsNotYetRehashed();
		bool DeleteObjectByName(const string& p_ObjectName);
		BigArrayIndex GetObjectMaxElementCount(BlockNumber p_BPAQuantumNumber);
		bool RehashingHasStartedButNotFinished();

	private:

//		UserModeVirtualMemoryFile(UMVMFileType FileType, const string& FileName);

		UserModeVirtualMemoryFile(ThreeMissesFileOptions Options);

		BigArrayIndex GetDirectoryEntryNumberByName(const string& p_ObjectName);
		BlockNumber CreateObject(BigArrayIndex p_MaxElementCount, bool OverrideMaximumCount, BlockNumber BlockNumberOfOwner);
		ArrayIndex CalculateNumberOfNestedBigPointerArraysForStringVector();
		BigArrayIndex CalculateMaximumNestedStringElementCount();
		bool DeleteObjectByNameContainingString(const string & p_ObjectNameComponent);

		BlockNumber AllocateMultipleConsecutiveBlocksFromFreedBlockNumberSets(ArrayIndex BlockCount);

		BlockNumber AllocateMultipleConsecutiveBlocksAndReturnTheFirstBlockNumber(ArrayIndex BlockCount);

		vector<BlockNumber> GetEmptyBlocksForSlot(ArrayIndex BlockCount);
		ArrayIndex GetBlockNumberForSlot(ArrayIndex SlotNumber) const;
		void IncreaseObjectElementCountIfNeeded(BlockNumber p_BPAQuantumNumber, BigArrayIndex NewElementCount);
		ValueView GetElementReferenceToLongString(ObjectType OT, ItemReferenceStruct ElementRefStruct);
		bool UpdateLongString(ItemReferenceStruct ElementRefStruct, AccessVectorChar ItemToStore);

		//Begin overflow area public data and functions

		ShortIndirectionInfoStruct AllocateOverflowArea(uint16_t Length);
		ValueView ReadOverflowArea(ShortIndirectionInfoStruct* Info);
		void UpdateOverflowArea(ShortIndirectionInfoStruct* Info, AccessVectorChar NewOverflowContents);
		void FreeOverflowArea(ShortIndirectionInfoStruct* Info);

		ItemReferenceStruct StoreLongString(AccessVectorChar ItemToStore, ObjectType Type);

		//End overflow area public data and functions

		UMVMDAM m_DAM;

		UserModeVirtualMemory::FunctionTimer* m_Timer;

		bool SetUseFreeBlockList(bool NewSetting);

		ObjectType PutObjectExceptionHelper(std::exception& x);

		void DeleteObjectByBPAQuantumNumber(BlockNumber BPAQuantumNumber);

		bool IsObjectOpen(BlockNumber p_ObjectNumber);
		BigArrayIndex FindObjectDirectoryEntryNumberByName(const string& p_ObjectName);
		vector<pair<BlockNumber, string>> FindObjectsWithNameContainingString(const string & p_ObjectNameComponent);
		void AddObjectToDirectory(const string& Name, BlockNumber BPAQuantumNumber);
		void SetUpLittlePointerBlock(BlockNumber NewQuantum, BlockNumber BlockNumberOfOwner);

		BigArrayIndex FindAvailableObject();
		std::set<BlockNumber> GetListOfAllQuantaUsedByObject(BlockNumber p_BPAQuantumNumber, size_t MaximumBlockCount = UINT_MAX);

		string GetObjectModifiableElement(BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex);

		ValueView GetElementReferenceToReadableElement(BlockNumber p_BPAQuantumNumber,
			BigArrayIndex p_ElementIndex);

		ValueView GetElementReferenceToReadableElementPriority(BlockNumber p_BPAQuantumNumber,
			BigArrayIndex p_ElementIndex);

		ValueView GetValueViewToElementInternal(BlockNumber p_BPAQuantumNumber,
			BigArrayIndex p_ElementIndex, bool ToBeWritten);

		/*__declspec(noinline)*/ ValueView GetValueViewToElementInternalPriority(
			BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex);

		ValueView GetElementReferenceToObjectModifiableElement(BlockNumber p_BPAQuantumNumber,
			BigArrayIndex p_ElementIndex);

		ValueView GetElementReferenceToObjectModifiableElementPriority(BlockNumber p_BPAQuantumNumber,
			BigArrayIndex p_ElementIndex);

		ObjectType GetObjectModifiableElementType(BlockNumber p_BPAQuantumNumber, ArrayIndex p_ElementIndex);

		ItemReferenceStruct GetUMVMItemReferenceFromLittlePointerIndex(BlockNumber LPAQuantumNumber, BigArrayIndex p_ElementIndex);
		void PutUMVMItemReferenceIntoLittlePointerIndex(BlockNumber LPAQuantumNumber, size_t p_ElementIndex, ItemReferenceStruct IR);

		ObjectType PutObjectElement(BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex,
			AccessVectorChar p_Element);

		void AllocateRawBlock(BlockNumber p_BPAQuantumNumber, ArrayIndex p_ElementIndex);

		BlockNumber GetNewBlockForItem();

		BlockNumber FindSpaceForItem(size_t ItemSize, BigPointerBlock &ThisBigPointerBlock,
			BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex, BlockNumber OldLeafBlockNumber);

		ObjectType PutObjectInternal(BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex,
			AccessVectorChar p_Element, const vector<string>& p_Vector);

		UserModeVirtualMemoryFile(const UserModeVirtualMemoryFile&) = delete;
		UserModeVirtualMemoryFile& operator=(const UserModeVirtualMemoryFile&) = delete;

		//Begin LongString structures and functions

		LongStringStorageParameters CalculateLongStringStorageParameters(BigArrayIndex LongStringSize);

		struct SetupLongStringHeaderReturnType
		{
			bool Found;
			vector<BlockNumber> BlockNumbers;
			BlockNumber StartingBlockNumber;
			LongStringStorageParameters LSP;
			BigArrayIndex OffsetInBlock;
			BigArrayIndex ElementSize;
			ItemReferenceStruct UIRS;
			SetupLongStringHeaderReturnType() : Found(false), BlockNumbers(), StartingBlockNumber(0), LSP()
				, OffsetInBlock(0), ElementSize(0), UIRS() {}
		};

		void StoreDataIntoLongString(SetupLongStringHeaderReturnType& StorageInfo, AccessVectorChar ItemToStore);

		SetupLongStringHeaderReturnType SetupLongStringHeader(ItemReferenceStruct ElementRefStruct);

		SetupLongStringHeaderReturnType AllocateStorageForLongString(size_t ElementSize, ObjectType Type);

		//End LongString functions

		//Variables

		ThreeMissesFileOptions m_Options;
		static int s_FileOpenSequence;

		map<BlockNumber, bool> m_OpenObjects;

		string m_PrimaryFileName;
		string m_SecondaryFileName;
		bool m_OpenSucceeded;
		bool m_NewFileWasCreated;

		UMVMFileType m_FileType;
		bool m_IndexFileInRAMWasRequested;

		static LPPROGRESS_ROUTINE m_ProgressRoutine;
		static LPPROGRESS_ROUTINE GetProgressRoutine();

		bool m_ReadOnly;

		BigArrayIndex m_SlotSize;

		bool m_ExceptionThrownSoDontTryToFlush;

		bool m_UseFreeBlockList;

		bool m_Closed;
		int m_FileOpenSequence;

		bool m_SetNewBlocksToKnownState;
		bool m_TrashBlocksWhenTheyAreUnpinned;

		string m_ErrorMessage;

		//Begin DataAccessManager functions

	public:

		bool NewFileWasCreated();
		void SetExceptionThrown();

		bool WeCanFlush();
		void Flush(bool Partial = false);

		ThreeMissesFileOptions GetOptions();
		size_t GetLogicalFileSize();

		BigArrayIndex GetSlotCount();
		ArrayIndex GetBlocksPerSlot();

	private:

		Block *MakeBlockResident(BlockNumber p_BlockNumber);
		
		BlockNumber FindSpaceForItem(BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex,
			size_t p_ElementSize);

		void IncreaseBlockCount(ArrayIndex NewBlockCount);

		//End DataAccessManager functions

		//Begin DataAccessManager data

		bool m_BufferAllocated;

		Block* m_BeginningOfContiguousBuffer;

		size_t m_PrimaryStorageAllocation;

		Block* m_LastBufferReferenced;

		ArrayIndex m_LastBlockNumberReferenced;

		ArrayIndex m_LastBufferReused;
		ArrayIndex m_LastBufferNumberReferenced;

		BlockNumber m_LastBigBlockNumberReferenced;
		Block* m_BlockBufferForLastBigBlockNumberReferenced;

		int64_t m_TotalCounter;
		long m_ReadCount;
		long m_WriteCount;

		map<ArrayIndex, set<BlockNumber>> m_FreedBlockNumberSets;
		map<BlockNumber, ArrayIndex> m_FreedBlockMap;
		map<BlockNumber, map<ArrayIndex, BlockNumber>> m_FreeSpaceMaps;

		struct OverflowAreaFreeListStruct
		{
			BlockNumber m_BlockNumber;
			ArrayIndex m_Offset;
		};

		map<ArrayIndex, vector<OverflowAreaFreeListStruct>> m_OverflowAreaFreeListMap;

//		UserModeVirtualMemoryFileHeaderStruct m_QFHS;
		UserModeVirtualMemoryFileHeader m_UFHS;

		void OpenExceptionHelper();

#if TRACK_STORAGE_ACCESS_TIMES
		vector<uint64_t> m_WriteTimes;
		vector<uint64_t> m_ReadTimes;
#endif

#if TRACK_BLOCK_RELOADS
		map<ArrayIndex, ArrayIndex> m_BlockReloadCounts;
		set<int64_t> m_BlockReloadCountsForEachBuffer;
		int64_t CalculateBlockReloadInfo(ArrayIndex p_BufferNumber, ArrayIndex p_BlockReloadCount);
		pair<ArrayIndex, ArrayIndex> SplitBlockReloadInfo(int64_t Info);
		map<ArrayIndex, ArrayIndex> GetBlockReloadCounts();
#endif

		//End DataAccessManager data


		//Begin memory mapping

		uint64_t m_DesiredMapReservation;
		bool m_ThrowOnExhaustion;
		//End memory mapping

	};
};
#endif
