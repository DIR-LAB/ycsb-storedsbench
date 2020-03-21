//ThreeMissesHashingGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ThreeMissesHashing_H
#define ThreeMissesHashing_H

namespace UserModeVirtualMemory
{
	class ThreeMissesHashingBase;

	struct ThreeMissesHashingDummyType {};

	typedef std::map<char, ThreeMissesHashingBase*> ThreeMissesHashingTypeMap;
	
	class ThreeMissesHashingBase
	{
	friend class ThreeMissesHashing;
	friend class ThreeMisses;

	public:

		enum TypeID {UnusedType = 0, RobinHoodHashVariableForMappedValueFile = 'M'};
		
		virtual char GetType() = 0;
		static vector<char> GetList();

		virtual ~ThreeMissesHashingBase();

		static ThreeMissesHashingBase* FindType(char Type);

		virtual BigArrayIndex GetInlineRecordLength() = 0;
		virtual ArrayIndex GetAdditionalLengthToAllocateForAV() = 0;
		virtual ArrayIndex GetSlotOccupiedElementCount(AccessVectorChar AV) = 0;
		virtual size_t GetTotalSearchCount() = 0;
		virtual void IncrementSlotOccupiedElementCount(AccessVectorChar AV) = 0;
		virtual ArrayIndex GetSlotAllocationUnit() = 0;
		virtual ValueView SetupHashAV(AccessVectorChar AV, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile* IndexFile) = 0;
////		virtual @PublicVirtualUserFunctionReturnType8@ @PublicVirtualUserFunction8@(@PublicVirtualUserFunctionParameterListDeclarations8@) = 0;
////		virtual @PublicVirtualUserFunctionReturnType9@ @PublicVirtualUserFunction9@(@PublicVirtualUserFunctionParameterListDeclarations9@) = 0;
		virtual void SetUpSlotForwardingToMultipleRawBlockSlotInNewFile(AccessVectorChar OldSlot) = 0;
		virtual void SetAccessVector(AccessVectorChar AV) = 0;
		virtual AccessVectorChar GetAccessVector() = 0;
		virtual bool DeleteElement(const AccessVectorChar Key, KeyHashCodes HashCodes, UserModeVirtualMemoryFile& PrimaryValueUMVMF) = 0;
		virtual uint8_t GetMaximumSearchLength(AccessVectorChar AV) = 0;
		virtual void PutMaximumSearchLength(uint8_t NewMaximumLength, AccessVectorChar AV) = 0;
		virtual ValueView CreateNewIndexForElement(AccessVectorChar Key, KeyHashCodes HashCodes, AccessVectorChar Data, RecordInfo IndirectInfo, UserModeVirtualMemoryFile& PrimaryValueUMVMF) = 0;
		virtual AccessVectorChar FindIndexForExistingElement(AccessVectorChar Key, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile& PrimaryValueUMVMF) = 0;
		virtual size_t GetAVSize() = 0;
		virtual bool RehashSlot(AccessVectorChar OldSlot, AccessVectorChar NewSlot, UserModeVirtualMemoryFile& PrimaryValueUMVMF) = 0;
		virtual RecordInfo GetRecordInfoForExistingElement(AccessVectorChar Key, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile& PrimaryValueUMVMF) = 0;
////		virtual @PublicVirtualUserFunctionReturnType21@ @PublicVirtualUserFunction21@(@PublicVirtualUserFunctionParameterListDeclarations21@) = 0;
		virtual ValueView SetupHashAVInternal(AccessVectorChar AV, size_t StartingPosition, UserModeVirtualMemoryFile* IndexFile) const = 0;

	protected:
		static ThreeMissesHashingTypeMap& GetTypeMap();
		static void AddTypeToMap(ThreeMissesHashingBase* Type);

		virtual ThreeMissesHashingBase* GetNewWorker(AccessVectorChar AV, BigArrayIndex KeyLength, BigArrayIndex DataLength, BigArrayIndex SlotCount, char Type) = 0;
		
		ThreeMissesHashingBase();

		int m_Count;
	};


	class ThreeMissesHashingBasic : public ThreeMissesHashingBase
	{
		friend class ThreeMissesHashing;

	protected:
		ThreeMissesHashingBasic(AccessVectorChar AV, BigArrayIndex KeyLength, BigArrayIndex DataLength, BigArrayIndex SlotCount, char Type);
		ThreeMissesHashingBasic(const ThreeMissesHashingDummyType&, char Type);
		ThreeMissesHashingBasic(char Type);
		char GetType();
		~ThreeMissesHashingBasic();
		ThreeMissesHashingBase* GetNewWorker(AccessVectorChar AV, BigArrayIndex KeyLength, BigArrayIndex DataLength, BigArrayIndex SlotCount, char Type);

		void SetAccessVector(AccessVectorChar AV);
		AccessVectorChar GetAsKey(AccessVectorChar AV, size_t& Index);
		void PutAsKey(AccessVectorChar Key, AccessVectorChar AV, size_t& Index);
		char GetAsOccupiedFlag(AccessVectorChar AV, size_t& Index);
		void PutAsOccupiedFlag(char NewFlag, AccessVectorChar AV, size_t& Index);
		size_t GetTotalSearchCount();
		uint8_t GetMaximumSearchLength(AccessVectorChar AV);
		size_t GetAVSize();
		bool DeleteElement(const AccessVectorChar Key, KeyHashCodes HashCodes, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		void PutMaximumSearchLength(uint8_t NewMaximumLength, AccessVectorChar AV);
		ArrayIndex GetSlotAllocationUnit();
		BigArrayIndex GetInlineRecordLength();
		AccessVectorChar GetAccessVector();
		
		char m_ThreeMissesHashingType;

	protected:
 		BigArrayIndex m_KeyLength;
 		BigArrayIndex m_DataLength;
 		BigArrayIndex m_TotalElementLength;
 		BigArrayIndex m_SlotCount;
 		AccessVectorChar m_AV;
 		size_t m_Index;
 		size_t m_TotalSearchCount;
	};

	class ThreeMissesHashing : public ThreeMissesHashingBase
	{
	public:

		ThreeMissesHashing(ThreeMissesHashingBase::TypeID Type);
		ThreeMissesHashing(AccessVectorChar AV, BigArrayIndex KeyLength, BigArrayIndex DataLength, BigArrayIndex SlotCount, char Type);
		ThreeMissesHashing(const ThreeMissesHashing& Item);
		ThreeMissesHashing& operator = (const ThreeMissesHashing& Item);
		~ThreeMissesHashing();
		
		ThreeMissesHashingBase* GetWorker();

		char GetType();

		BigArrayIndex GetInlineRecordLength();
		ArrayIndex GetAdditionalLengthToAllocateForAV();
		ArrayIndex GetSlotOccupiedElementCount(AccessVectorChar AV);
		size_t GetTotalSearchCount();
		void IncrementSlotOccupiedElementCount(AccessVectorChar AV);
		ArrayIndex GetSlotAllocationUnit();
		ValueView SetupHashAV(AccessVectorChar AV, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile* IndexFile);
////		@PublicVirtualUserFunctionReturnType8@ @PublicVirtualUserFunction8@(@PublicVirtualUserFunctionParameterListDeclarations8@);
////		@PublicVirtualUserFunctionReturnType9@ @PublicVirtualUserFunction9@(@PublicVirtualUserFunctionParameterListDeclarations9@);
		void SetUpSlotForwardingToMultipleRawBlockSlotInNewFile(AccessVectorChar OldSlot);
		void SetAccessVector(AccessVectorChar AV);
		AccessVectorChar GetAccessVector();
		bool DeleteElement(const AccessVectorChar Key, KeyHashCodes HashCodes, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		uint8_t GetMaximumSearchLength(AccessVectorChar AV);
		void PutMaximumSearchLength(uint8_t NewMaximumLength, AccessVectorChar AV);
		ValueView CreateNewIndexForElement(AccessVectorChar Key, KeyHashCodes HashCodes, AccessVectorChar Data, RecordInfo IndirectInfo, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		AccessVectorChar FindIndexForExistingElement(AccessVectorChar Key, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		size_t GetAVSize();
		bool RehashSlot(AccessVectorChar OldSlot, AccessVectorChar NewSlot, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
		RecordInfo GetRecordInfoForExistingElement(AccessVectorChar Key, ArrayIndex TrancheHashCode, UserModeVirtualMemoryFile& PrimaryValueUMVMF);
////		@PublicVirtualUserFunctionReturnType21@ @PublicVirtualUserFunction21@(@PublicVirtualUserFunctionParameterListDeclarations21@);
		ValueView SetupHashAVInternal(AccessVectorChar AV, size_t StartingPosition, UserModeVirtualMemoryFile* IndexFile) const;
////		@PublicVirtualUserConstFunctionReturnType2@ @PublicVirtualUserConstFunction2@(@PublicVirtualUserConstFunctionParameterListDeclarations2@) const;
////		@PublicVirtualUserConstFunctionReturnType3@ @PublicVirtualUserConstFunction3@(@PublicVirtualUserConstFunctionParameterListDeclarations3@) const;
////		@PublicVirtualUserConstFunctionReturnType4@ @PublicVirtualUserConstFunction4@(@PublicVirtualUserConstFunctionParameterListDeclarations4@) const;
////		@PublicVirtualUserConstFunctionReturnType5@ @PublicVirtualUserConstFunction5@(@PublicVirtualUserConstFunctionParameterListDeclarations5@) const;
////		@PublicVirtualUserConstFunctionReturnType6@ @PublicVirtualUserConstFunction6@(@PublicVirtualUserConstFunctionParameterListDeclarations6@) const;
////		@PublicVirtualUserConstFunctionReturnType7@ @PublicVirtualUserConstFunction7@(@PublicVirtualUserConstFunctionParameterListDeclarations7@) const;
////		@PublicVirtualUserConstFunctionReturnType8@ @PublicVirtualUserConstFunction8@(@PublicVirtualUserConstFunctionParameterListDeclarations8@) const;
////		@PublicVirtualUserConstFunctionReturnType9@ @PublicVirtualUserConstFunction9@(@PublicVirtualUserConstFunctionParameterListDeclarations9@) const;
////		@PublicVirtualUserConstFunctionReturnType10@ @PublicVirtualUserConstFunction10@(@PublicVirtualUserConstFunctionParameterListDeclarations10@) const;
////		@PublicVirtualUserConstFunctionReturnType11@ @PublicVirtualUserConstFunction11@(@PublicVirtualUserConstFunctionParameterListDeclarations11@) const;
////		@PublicVirtualUserConstFunctionReturnType12@ @PublicVirtualUserConstFunction12@(@PublicVirtualUserConstFunctionParameterListDeclarations12@) const;
////		@PublicVirtualUserConstFunctionReturnType13@ @PublicVirtualUserConstFunction13@(@PublicVirtualUserConstFunctionParameterListDeclarations13@) const;
////		@PublicVirtualUserConstFunctionReturnType14@ @PublicVirtualUserConstFunction14@(@PublicVirtualUserConstFunctionParameterListDeclarations14@) const;
////		@PublicVirtualUserConstFunctionReturnType15@ @PublicVirtualUserConstFunction15@(@PublicVirtualUserConstFunctionParameterListDeclarations15@) const;
////		@PublicVirtualUserConstFunctionReturnType16@ @PublicVirtualUserConstFunction16@(@PublicVirtualUserConstFunctionParameterListDeclarations16@) const;
////		@PublicVirtualUserConstFunctionReturnType17@ @PublicVirtualUserConstFunction17@(@PublicVirtualUserConstFunctionParameterListDeclarations17@) const;
////		@PublicVirtualUserConstFunctionReturnType18@ @PublicVirtualUserConstFunction18@(@PublicVirtualUserConstFunctionParameterListDeclarations18@) const;
////		@PublicVirtualUserConstFunctionReturnType19@ @PublicVirtualUserConstFunction19@(@PublicVirtualUserConstFunctionParameterListDeclarations19@) const;
////		@PublicVirtualUserConstFunctionReturnType20@ @PublicVirtualUserConstFunction20@(@PublicVirtualUserConstFunctionParameterListDeclarations20@) const;
////		@PublicVirtualUserConstFunctionReturnType21@ @PublicVirtualUserConstFunction21@(@PublicVirtualUserConstFunctionParameterListDeclarations21@) const;

	protected:
		ThreeMissesHashingBase * m_Worker;

	private:
		ThreeMissesHashingBase * GetNewWorker(AccessVectorChar AV, BigArrayIndex KeyLength, BigArrayIndex DataLength, BigArrayIndex SlotCount, char Type);

		void Release();
	};

	extern ThreeMissesHashingDummyType ThreeMissesHashingDummy;
};

#endif
