//UMVMDAMGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMDAMGenerated_H
#define UMVMDAMGenerated_H

namespace UserModeVirtualMemory
{
	class UMVMDAMBase;
	class UMVMMemoryMapping;

	struct UMVMDAMDummyType {};

	typedef std::map<char, UMVMDAMBase*> UMVMDAMTypeMap;

	class UMVMDAMBase
	{
	friend class UMVMDAM;
	friend class UserModeVirtualMemoryFile;

	public:

		enum TypeID {UnusedType = 0, TemporaryFileInMemory = 'T', ForMappedBlocks = 'M'};
		virtual char GetType() = 0;
		static vector<char> GetList();

		virtual ~UMVMDAMBase();

		static UMVMDAMBase* FindType(char Type);

	virtual char* GetAddressInMappedFile(uint64_t OffsetInFile, uint64_t DataLength, bool Appending) const = 0;
	virtual void SetRelativeAddressLimit(BigArrayIndex RelativeAddressLimit) const = 0;
	virtual BigArrayIndex GetRelativeAddressLimit() const = 0;
	virtual BigArrayIndex GetSize() const = 0;
	virtual void Flush() = 0;
	virtual bool WeCanFlush() = 0;
////	virtual @PublicVirtualUserFunctionReturnType3@ @PublicVirtualUserFunction3@(@PublicVirtualUserFunctionParameterListDeclarations3@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType4@ @PublicVirtualUserFunction4@(@PublicVirtualUserFunctionParameterListDeclarations4@) = 0;
	virtual Block* MakeBlockResident(BlockNumber BlockNumberToMakeResident) = 0;
	virtual AccessVectorChar ReadOverflowArea(ShortIndirectionInfoStruct* Info) = 0;
	virtual void UpdateOverflowArea(ShortIndirectionInfoStruct* Info, AccessVectorChar OverflowBuffer) = 0;
	virtual void Delete() = 0;
////	virtual @PublicVirtualUserFunctionReturnType9@ @PublicVirtualUserFunction9@(@PublicVirtualUserFunctionParameterListDeclarations9@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType10@ @PublicVirtualUserFunction10@(@PublicVirtualUserFunctionParameterListDeclarations10@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType11@ @PublicVirtualUserFunction11@(@PublicVirtualUserFunctionParameterListDeclarations11@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType12@ @PublicVirtualUserFunction12@(@PublicVirtualUserFunctionParameterListDeclarations12@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType13@ @PublicVirtualUserFunction13@(@PublicVirtualUserFunctionParameterListDeclarations13@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType14@ @PublicVirtualUserFunction14@(@PublicVirtualUserFunctionParameterListDeclarations14@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType15@ @PublicVirtualUserFunction15@(@PublicVirtualUserFunctionParameterListDeclarations15@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType16@ @PublicVirtualUserFunction16@(@PublicVirtualUserFunctionParameterListDeclarations16@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType17@ @PublicVirtualUserFunction17@(@PublicVirtualUserFunctionParameterListDeclarations17@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType18@ @PublicVirtualUserFunction18@(@PublicVirtualUserFunctionParameterListDeclarations18@) = 0;
	virtual bool SetupReadOnly(const string& PrimaryFileName, const string& SecondaryFileName, UserModeVirtualMemory::UMVMFileType FileType) = 0;
	virtual bool SetupReadWrite(const string& PrimaryFileName, const string& SecondaryFileName, UserModeVirtualMemory::UMVMFileType FileType, bool Temporary) = 0;
	virtual bool CreateNewFile(const string& PrimaryFileName, const string& SecondaryFileName) = 0;
	virtual void AllocateBuffers() = 0;
////	virtual @PublicVirtualUserFunctionReturnType23@ @PublicVirtualUserFunction23@(@PublicVirtualUserFunctionParameterListDeclarations23@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType24@ @PublicVirtualUserFunction24@(@PublicVirtualUserFunctionParameterListDeclarations24@) = 0;
	virtual void CloseHelper() = 0;
	virtual ValueView TryToMapConsecutiveData(vector<BlockNumber>& BlockNumbers, BlockNumber StartingBlockNumber, LongStringStorageParameters LSP, BigArrayIndex OffsetInBlock, BigArrayIndex ElementSize, ObjectType OT) = 0;
////	virtual @PublicVirtualUserFunctionReturnType27@ @PublicVirtualUserFunction27@(@PublicVirtualUserFunctionParameterListDeclarations27@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType28@ @PublicVirtualUserFunction28@(@PublicVirtualUserFunctionParameterListDeclarations28@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType29@ @PublicVirtualUserFunction29@(@PublicVirtualUserFunctionParameterListDeclarations29@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType30@ @PublicVirtualUserFunction30@(@PublicVirtualUserFunctionParameterListDeclarations30@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType31@ @PublicVirtualUserFunction31@(@PublicVirtualUserFunctionParameterListDeclarations31@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType32@ @PublicVirtualUserFunction32@(@PublicVirtualUserFunctionParameterListDeclarations32@) = 0;
////	virtual @PublicVirtualUserFunctionReturnType33@ @PublicVirtualUserFunction33@(@PublicVirtualUserFunctionParameterListDeclarations33@) = 0;

	protected:
		static UMVMDAMTypeMap& GetTypeMap();
		static void AddTypeToMap(UMVMDAMBase* Type);

		virtual UMVMDAMBase* GetNewWorker(UserModeVirtualMemoryFile* umvmf, char Type) = 0;

		UMVMDAMBase();

		int m_Count;
	};


	class UMVMDAMBasic : public UMVMDAMBase
	{
		friend class UMVMDAM;

	protected:
		UMVMDAMBasic(UserModeVirtualMemoryFile* umvmf, char Type);
		UMVMDAMBasic(const UMVMDAMDummyType&, char Type);
		UMVMDAMBasic(char Type);
		char GetType();
		~UMVMDAMBasic();
		UMVMDAMBase* GetNewWorker(UserModeVirtualMemoryFile* umvmf, char Type);

		char m_UMVMDAMType;

	protected:
	UserModeVirtualMemoryFile* m_umvmf;
	UMVMMemoryMapping* m_MemoryMapping;
	uint64_t m_ReservedSize;
	uint64_t m_CurrentSize;
	bool m_WeCanFlush;
	FILE* m_PrimaryFilePtr;
	FILE* m_SecondaryFilePtr;
	};

	class UMVMDAM : public UMVMDAMBase
	{
	public:

		UMVMDAM(UserModeVirtualMemoryFile* umvmf, char Type);
		UMVMDAM(const UMVMDAM& Item);
		UMVMDAM& operator = (const UMVMDAM& Item);
		~UMVMDAM();

		static UMVMDAMBase::TypeID GetTypeIDByName(const string& TypeIDName);
		char GetType();

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

	protected:
		UMVMDAMBase * m_Worker;

	private:
		UMVMDAMBase * GetNewWorker(UserModeVirtualMemoryFile* umvmf, char Type);

		void Release();
	};

	extern UMVMDAMDummyType UMVMDAMDummy;
};

#endif
