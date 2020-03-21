//UMVMItemReferenceGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMItemReference_H
#define UMVMItemReference_H

namespace UserModeVirtualMemory
{
	class UMVMItemReferenceBase;

	struct UMVMItemReferenceDummyType {};

	typedef std::map<char, UMVMItemReferenceBase*> UMVMItemReferenceTypeMap;

	class UMVMItemReferenceBase
	{
	friend class UMVMItemReference;
	friend class UserModeVirtualMemoryFile;

	public:

	//Note: this MUST MATCH the definition of TypeID in UMVMObjectType.h
	//  Otherwise, terrible things will happen!
     enum TypeID {NullItem = 0, RawBlock, ShortString, LongString, MultipleRawBlockSlot};
    
		virtual char GetType() = 0;
		static vector<char> GetList();

		virtual ~UMVMItemReferenceBase();

		static UMVMItemReferenceBase* FindType(char Type);

 virtual ObjectType GetUMVMItemReferenceObjectType() = 0;
 virtual ItemReferenceStruct GetUMVMItemReferenceStruct() = 0;
 virtual BlockNumber GetQuantumNumber() = 0;
 virtual ValueView GetValueViewToElementInternal(UserModeVirtualMemoryFile* UMVMF) = 0;
 virtual bool PutObjectInternal(UserModeVirtualMemoryFile* UMVMF, BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex, LittlePointerBlock& ThisLittlePointerBlock, ArrayIndex LittlePointerIndex, AccessVectorChar ItemToStore, const vector<string>& p_Vector) = 0;
 virtual void DeleteObject(UserModeVirtualMemoryFile* UMVMF, BigPointerBlock& ThisBigPointerBlock) = 0;
 virtual ValueView GetValueViewToElementInternalPriority(UserModeVirtualMemoryFile* UMVMF) = 0;

	protected:
		static UMVMItemReferenceTypeMap& GetTypeMap();
		static void AddTypeToMap(UMVMItemReferenceBase* Type);

		virtual UMVMItemReferenceBase* GetNewWorker(char Type, ItemReferenceStruct IR) = 0;

////		virtual @ProtectedUserFunctionReturnType1@ @ProtectedUserFunction1@(@ProtectedUserFunctionParameterList1@) = 0;
////		virtual @ProtectedUserFunctionReturnType2@ @ProtectedUserFunction2@(@ProtectedUserFunctionParameterList2@) = 0;
////		virtual @ProtectedUserFunctionReturnType3@ @ProtectedUserFunction3@(@ProtectedUserFunctionParameterList3@) = 0;

		UMVMItemReferenceBase();

		int m_Count;
	};


	class UMVMItemReferenceBasic : public UMVMItemReferenceBase
	{
		friend class UMVMItemReference;

	protected:
		UMVMItemReferenceBasic(char Type, ItemReferenceStruct IR);
		UMVMItemReferenceBasic(const UMVMItemReferenceDummyType&, char Type);
		UMVMItemReferenceBasic(char Type);
		char GetType();
		~UMVMItemReferenceBasic();
		UMVMItemReferenceBase* GetNewWorker(char Type, ItemReferenceStruct IR);

		char m_UMVMItemReferenceType;

	protected:
		ItemReferenceStruct m_IR;
////		@BasicProtectedVariableDeclaration2@;
////		@BasicProtectedVariableDeclaration3@;
////		@BasicProtectedVariableDeclaration4@;
////		@BasicProtectedFunctionReturnType1@ @BasicProtectedFunction1@(@BasicProtectedFunctionParameterListDeclarations1@);
		bool PutObjectInternalHelper(UserModeVirtualMemoryFile * UMVMF, LittlePointerBlock & ThisLittlePointerBlock, ArrayIndex LittlePointerIndex, AccessVectorChar ItemToStore);
	};

	class UMVMItemReference : public UMVMItemReferenceBase
	{
	public:

		UMVMItemReference(ItemReferenceStruct IR);
		UMVMItemReference(const UMVMItemReference& Item);
		UMVMItemReference& operator = (const UMVMItemReference& Item);
		~UMVMItemReference();

		char GetType();

 virtual ObjectType GetUMVMItemReferenceObjectType();
 virtual ItemReferenceStruct GetUMVMItemReferenceStruct();
 virtual BlockNumber GetQuantumNumber();
 virtual ValueView GetValueViewToElementInternal(UserModeVirtualMemoryFile* UMVMF);
 virtual bool PutObjectInternal(UserModeVirtualMemoryFile* UMVMF, BlockNumber p_BPAQuantumNumber, BigArrayIndex p_ElementIndex, LittlePointerBlock& ThisLittlePointerBlock, ArrayIndex LittlePointerIndex, AccessVectorChar ItemToStore, const vector<string>& p_Vector);
 virtual void DeleteObject(UserModeVirtualMemoryFile* UMVMF, BigPointerBlock& ThisBigPointerBlock);
 virtual ValueView GetValueViewToElementInternalPriority(UserModeVirtualMemoryFile* UMVMF);
		
////		@ManagerPublicUserFunctionReturnType1@ @ManagerPublicUserFunction1@(@ManagerPublicUserFunctionParameterListDeclarations1@);
////		@ManagerPublicUserFunctionReturnType2@ @ManagerPublicUserFunction2@(@ManagerPublicUserFunctionParameterListDeclarations2@);

	protected:
		UMVMItemReferenceBase * m_Worker;

	private:
		UMVMItemReferenceBase * GetNewWorker(char Type, ItemReferenceStruct IR);
////		@ManagerPrivateFunctionReturnType1@ @ManagerPrivateFunction1@(@ManagerPrivateFunctionParameterListDeclarations1@);
////		@ManagerPrivateFunctionReturnType2@ @ManagerPrivateFunction2@(@ManagerPrivateFunctionParameterListDeclarations2@);
////		@ManagerPrivateFunctionReturnType3@ @ManagerPrivateFunction3@(@ManagerPrivateFunctionParameterListDeclarations3@);

		void Release();
	};

	extern UMVMItemReferenceDummyType UMVMItemReferenceDummy;
};

#endif
