//ThreeMissesProGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ThreeMissesPro_H
#define ThreeMissesPro_H

namespace UserModeVirtualMemory
{
	class UMVMMProStringRefAV;
	
	class ThreeMissesPro;

	class ThreeMissesPro : public ThreeMissesBasic
	{
		friend class ThreeMissesCIterPro;
		friend class ThreeMissesCIterFree;

	public:
		ThreeMissesPro(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options, const string& ValuePrimaryFileName, const string& ValueSecondaryFileName="");
		ThreeMissesPro(const string& IndexPrimaryFileName, const string& ValuePrimaryFileName);

		ThreeMissesPro(const ThreeMissesDummyType&, string Type);
		
		ThreeMissesPro(string Type);
		
		explicit ThreeMissesPro();

		~ThreeMissesPro();
		
		static const string WorkerType;

		bool StoreRecord(AccessVectorChar p_Key, AccessVectorChar p_Data);
		bool erase(AccessVectorChar Key);
		AccessVectorChar GetValue(AccessVectorChar Key);
		AccessVectorChar UpdateRecordField(AccessVectorChar ExistingRecordData, size_t OffsetInRecord, AccessVectorChar DataToUseToUpdateRecord, bool Flush=false);
		void Flush();
////		@PublicVirtualUserFunctionReturnType6@ @PublicVirtualUserFunction6@(@PublicVirtualUserFunctionParameterListDeclarations6@);
		bool AppendRecord(AccessVectorChar p_Key, AccessVectorChar p_Data);
		void SetAllowDirectAccess();
		ArrayIndex GetRehashCount();
		void PrecacheIndex();
		bool StoreElementInternal(AccessVectorChar Key, AccessVectorChar Value, bool AppendOnly);
		void Close();
		bool DeleteElementInternal(AccessVectorChar Key, KeyHashCodes HashCodes, RecordInfo& ExistingRecordInfo);
		void  InitializeIndexFile(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options);
		void  Open(bool NewTable);
		void SetMaximumIndexFileSize(BigArrayIndex MaximumFileSize);
		void SetMaximumValueFileSize(BigArrayIndex MaximumFileSize);
////		@PublicVirtualUserFunctionReturnType18@ @PublicVirtualUserFunction18@(@PublicVirtualUserFunctionParameterListDeclarations18@);
		ValueView SetupHashAVInternal(AccessVectorChar AV, size_t StartingPosition, UserModeVirtualMemoryFile* IndexFile) const;
		ArrayIndex CalculateSlotNumber(KeyHashCodes KeyHashes) const;
		UserModeVirtualMemoryFile*  GetDataUMVMFAddress() const;
		size_t Round(size_t NeededSize) const;
		UserModeVirtualMemoryFile*  GetIndexPrimaryFile() const;
		int64_t GetTotalRehashTimeInCycles() const;
		ArrayIndex GetBlocksPerSlot() const;
		BigArrayIndex GetSlotCount() const;
		BigArrayIndex GetRehashedRootSlotCount() const;

		ThreeMissesCIterBase cbegin() const;
		ThreeMissesCIterBase cend() const;

		ThreeMissesCIterBase begin() const;
		ThreeMissesCIterBase end() const;
		
		ThreeMissesCIterBase operator++();
		value_type operator*() const;
		
		value_type Dereference(ThreeMissesCIterBase Iterator) const;
		ThreeMissesCIterBase PostIncrement(ThreeMissesCIterBase& Iterator) const;
		
		pair<AccessVectorChar,AccessVectorChar> GetKeyAndValueFromIterator(ThreeMissesCIterBase Iterator) const;

	protected:

		ThreeMissesBase* GetNewWorker(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options, const string& ValuePrimaryFileName, const string& ValueSecondaryFileName="");
		ThreeMissesBase* GetNewWorker(const string& IndexPrimaryFileName, const string& ValuePrimaryFileName);
		ThreeMissesBase* GetNewWorker();

	private:
		ThreeMissesPro(const ThreeMissesPro&) = delete;
		ThreeMissesPro& operator=(const ThreeMissesPro&) = delete;
		ThreeMissesPro& operator=(ThreeMissesPro&& other) = delete;
		
#include "ThreeMissesPro.inc"
	};
};

#endif
