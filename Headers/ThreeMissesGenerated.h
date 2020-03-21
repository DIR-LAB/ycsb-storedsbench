//ThreeMissesGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ThreeMissesGenerated_H
#define ThreeMissesGenerated_H

namespace UserModeVirtualMemory
{
	class ThreeMisses;
	

	class ThreeMissesStringRef
	{
		friend class ThreeMissesCIterPro;
		friend class ThreeMissesCIterFree;

	private:
	ThreeMisses &m_DHA;
	string m_DHAIndex;

	private: //prevent compiler warnings
		ThreeMissesStringRef& operator=(const ThreeMissesStringRef&) = delete;

	public:
		ThreeMissesStringRef(ThreeMisses &p_DHA, const string& p_DHAIndex);
		ThreeMissesStringRef& operator=(const string& p_ME);
		
		operator string();
	};

	class ThreeMissesBase;

	struct ThreeMissesDummyType {};

	typedef std::map<string, ThreeMissesBase*> ThreeMissesTypeMap;

	class ThreeMissesBase
	{
	friend class ThreeMisses;

	public:

		virtual string GetType() const = 0;
		static vector<string> GetList();

		typedef pair<string, string> value_type;

		virtual ThreeMissesCIterBase cbegin() const;
		virtual ThreeMissesCIterBase cend() const;

		virtual ThreeMissesCIterBase begin() const;
		virtual ThreeMissesCIterBase end() const;

		virtual ThreeMissesCIterBase operator++();
		virtual value_type operator *() const;

		virtual value_type Dereference(ThreeMissesCIterBase Iterator) const;
		virtual ThreeMissesCIterBase PostIncrement(ThreeMissesCIterBase& Iterator) const;
		
		virtual pair<AccessVectorChar,AccessVectorChar> GetKeyAndValueFromIterator(ThreeMissesCIterBase Iterator) const;

		virtual ~ThreeMissesBase();

		static ThreeMissesBase* FindType(string Type);
		
	virtual	ValueView SetupHashAVInternal(AccessVectorChar AV, size_t StartingPosition, UserModeVirtualMemoryFile* IndexFile) const = 0;
	virtual	ArrayIndex CalculateSlotNumber(KeyHashCodes KeyHashes) const = 0;
	virtual	UserModeVirtualMemoryFile*  GetDataUMVMFAddress() const = 0;
	virtual	size_t Round(size_t NeededSize) const = 0;
	virtual	UserModeVirtualMemoryFile*  GetIndexPrimaryFile() const = 0;
	virtual	int64_t GetTotalRehashTimeInCycles() const = 0;
	virtual	ArrayIndex GetBlocksPerSlot() const = 0;
	virtual	BigArrayIndex GetSlotCount() const = 0;
	virtual	BigArrayIndex GetRehashedRootSlotCount() const = 0;
 virtual	bool StoreRecord(AccessVectorChar p_Key, AccessVectorChar p_Data) = 0;
 virtual	bool erase(AccessVectorChar Key) = 0;
 virtual	AccessVectorChar GetValue(AccessVectorChar Key) = 0;
 virtual	AccessVectorChar UpdateRecordField(AccessVectorChar ExistingRecordData, size_t OffsetInRecord, AccessVectorChar DataToUseToUpdateRecord, bool Flush=false) = 0;
 virtual	void Flush() = 0;
//// virtual	@PublicVirtualUserFunctionReturnType6@ @PublicVirtualUserFunction6@(@PublicVirtualUserFunctionParameterListDeclarations6@) = 0;
 virtual	bool AppendRecord(AccessVectorChar p_Key, AccessVectorChar p_Data) = 0;
 virtual	void SetAllowDirectAccess() = 0;
 virtual	ArrayIndex GetRehashCount() = 0;
 virtual	void PrecacheIndex() = 0;
 virtual	bool StoreElementInternal(AccessVectorChar Key, AccessVectorChar Value, bool AppendOnly) = 0;
 virtual	void Close() = 0;
 virtual	bool DeleteElementInternal(AccessVectorChar Key, KeyHashCodes HashCodes, RecordInfo& ExistingRecordInfo) = 0;
 virtual	void  InitializeIndexFile(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options) = 0;
 virtual	void  Open(bool NewTable) = 0;
 virtual	void SetMaximumIndexFileSize(BigArrayIndex MaximumFileSize) = 0;
 virtual	void SetMaximumValueFileSize(BigArrayIndex MaximumFileSize) = 0;
//// virtual	@PublicVirtualUserFunctionReturnType18@ @PublicVirtualUserFunction18@(@PublicVirtualUserFunctionParameterListDeclarations18@) = 0;

		static string ThreeMissesPrefix;

	protected:
		static ThreeMissesTypeMap& GetTypeMap();
		static void AddTypeToMap(ThreeMissesBase* Type);

		virtual ThreeMissesBase* GetNewWorker(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options, const string& ValuePrimaryFileName, const string& ValueSecondaryFileName="") = 0;
		virtual ThreeMissesBase* GetNewWorker(const string& IndexPrimaryFileName, const string& ValuePrimaryFileName) = 0;
		virtual ThreeMissesBase* GetNewWorker() = 0;
		
		ThreeMissesBase();
		
		int m_Count;
	};


	class ThreeMissesBasic : public ThreeMissesBase
	{
		friend class ThreeMisses;
		friend class ThreeMissesCIterBase;

	protected:
		ThreeMissesBasic(string Type, const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options);
		ThreeMissesBasic(const ThreeMissesDummyType&, string Type, const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options);
		string GetType() const;
		~ThreeMissesBasic();
		ThreeMissesBase* GetNewWorker(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options, const string& ValuePrimaryFileName, const string& ValueSecondaryFileName="");
		ThreeMissesBase* GetNewWorker(const string& IndexPrimaryFileName, const string& ValuePrimaryFileName);
		ThreeMissesBase* GetNewWorker();

	ValueView SetupHashAVInternal(AccessVectorChar AV, size_t StartingPosition, UserModeVirtualMemoryFile* IndexFile) const;
	ArrayIndex CalculateSlotNumber(KeyHashCodes KeyHashes) const;
	UserModeVirtualMemoryFile*  GetDataUMVMFAddress() const;
	size_t Round(size_t NeededSize) const;
	UserModeVirtualMemoryFile*  GetIndexPrimaryFile() const;
	int64_t GetTotalRehashTimeInCycles() const;
	ArrayIndex GetBlocksPerSlot() const;
	BigArrayIndex GetSlotCount() const;
	BigArrayIndex GetRehashedRootSlotCount() const;
	bool StoreRecord(AccessVectorChar p_Key, AccessVectorChar p_Data);
	bool erase(AccessVectorChar Key);
	AccessVectorChar GetValue(AccessVectorChar Key);
	AccessVectorChar UpdateRecordField(AccessVectorChar ExistingRecordData, size_t OffsetInRecord, AccessVectorChar DataToUseToUpdateRecord, bool Flush=false);
	void Flush();
////	@PublicVirtualUserFunctionReturnType6@ @PublicVirtualUserFunction6@(@PublicVirtualUserFunctionParameterListDeclarations6@);
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
////	@PublicVirtualUserFunctionReturnType18@ @PublicVirtualUserFunction18@(@PublicVirtualUserFunctionParameterListDeclarations18@);
		
		string m_ThreeMissesType;

	protected:
		UserModeVirtualMemoryFile* m_IndexPrimaryFile;
		mutable UserModeVirtualMemoryFile* m_RehashingTarget;
			
#include "ThreeMisses.inc"		

	};

	class ThreeMisses : public ThreeMissesBase
	{
	public:

		ThreeMisses();
		ThreeMisses(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options, const string& ValuePrimaryFileName, const string& ValueSecondaryFileName="");
		ThreeMisses(const string& IndexPrimaryFileName, const string& ValuePrimaryFileName);
		ThreeMisses(const ThreeMisses& Item);
		ThreeMisses& operator = (const ThreeMisses& Item);
		~ThreeMisses();

		string GetType() const;
		
		static const string IndexParameterVectorName;
		static const string ValueParameterVectorName;
		
	ValueView SetupHashAVInternal(AccessVectorChar AV, size_t StartingPosition, UserModeVirtualMemoryFile* IndexFile) const;
	ArrayIndex CalculateSlotNumber(KeyHashCodes KeyHashes) const;
	UserModeVirtualMemoryFile*  GetDataUMVMFAddress() const;
	size_t Round(size_t NeededSize) const;
	UserModeVirtualMemoryFile*  GetIndexPrimaryFile() const;
	int64_t GetTotalRehashTimeInCycles() const;
	ArrayIndex GetBlocksPerSlot() const;
	BigArrayIndex GetSlotCount() const;
	BigArrayIndex GetRehashedRootSlotCount() const;
	bool StoreRecord(AccessVectorChar p_Key, AccessVectorChar p_Data);
	bool erase(AccessVectorChar Key);
	AccessVectorChar GetValue(AccessVectorChar Key);
	AccessVectorChar UpdateRecordField(AccessVectorChar ExistingRecordData, size_t OffsetInRecord, AccessVectorChar DataToUseToUpdateRecord, bool Flush=false);
	void Flush();
////	@PublicVirtualUserFunctionReturnType6@ @PublicVirtualUserFunction6@(@PublicVirtualUserFunctionParameterListDeclarations6@);
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
////	@PublicVirtualUserFunctionReturnType18@ @PublicVirtualUserFunction18@(@PublicVirtualUserFunctionParameterListDeclarations18@);

		ThreeMissesCIterBase cbegin() const;
		ThreeMissesCIterBase cend() const;

		ThreeMissesCIterBase begin() const;
		ThreeMissesCIterBase end() const;

		ThreeMissesCIterBase operator++();
		value_type operator *() const;

////		@PublicManagerFunctionReturnType1@ @PublicManagerFunction1@(@PublicManagerFunctionParameterListDeclarations1@);
////		@PublicManagerFunctionReturnType2@ @PublicManagerFunction2@(@PublicManagerFunctionParameterListDeclarations2@);

	ThreeMissesStringRef operator[](const string& p_Index);
	ThreeMissesStringRef operator[](size_t p_Index);

	protected:
		ThreeMissesBase * m_Worker;

	private:
		ThreeMissesBase * GetNewWorker(const UserModeVirtualMemoryFile::ThreeMissesFileOptions& Options, const string& ValuePrimaryFileName, const string& ValueSecondaryFileName="");
		ThreeMissesBase * GetNewWorker(const string& IndexPrimaryFileName, const string& ValuePrimaryFileName);
		ThreeMissesBase * GetNewWorker();
		
		void Release();
	};

	extern ThreeMissesDummyType ThreeMissesDummy;
};

#endif
