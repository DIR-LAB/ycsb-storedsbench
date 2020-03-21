//KVStoreFTest.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef KVStoreFTest_H
#define KVStoreFTest_H

namespace UserModeVirtualMemory
{
	class KVStoreFTestBase;

	struct KVStoreFTestDummyType {};

	typedef std::map<string, KVStoreFTestBase*> KVStoreFTestTypeMap;

	class KVStoreFTestBase
	{
	friend class KVStoreFTest;

	public:

		virtual string GetType() = 0;
		static vector<string> GetList();

		virtual ~KVStoreFTestBase();

		static KVStoreFTestBase* FindType(string Type);

		virtual bool PopulateDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly) = 0;
		virtual bool InitializeValueFiles() = 0;
////		virtual @PublicVirtualUserFunctionReturnType3@ @PublicVirtualUserFunction3@(@PublicVirtualUserFunctionParameterListDeclarations3@) = 0;
		virtual string GetDescription() = 0;
		virtual pair<ArrayIndex, ArrayIndex> GetStartingAndEndingTimes() = 0;
		virtual bool AccessDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly) = 0;

	protected:
		static KVStoreFTestTypeMap& GetTypeMap();
		static void AddTypeToMap(KVStoreFTestBase* Type);

		virtual KVStoreFTestBase* GetNewWorker(string Type) = 0;
		
		KVStoreFTestBase();

		int m_Count;
	};


	class KVStoreFTestBasic : public KVStoreFTestBase
	{
		friend class KVStoreFTest;

	protected:
		KVStoreFTestBasic(string Type);
		KVStoreFTestBasic(const KVStoreFTestDummyType&, string Type);
		string GetType();
		~KVStoreFTestBasic();
		KVStoreFTestBase* GetNewWorker(string Type);

		bool PopulateDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly);
		bool InitializeValueFiles();
////		@PublicVirtualUserFunctionReturnType3@ @PublicVirtualUserFunction3@(@PublicVirtualUserFunctionParameterListDeclarations3@);
		string GetDescription();
		pair<ArrayIndex, ArrayIndex> GetStartingAndEndingTimes();
		bool AccessDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly);

		
		string m_KVStoreFTestType;

	protected:
//// @BasicProtectedVariableDeclaration1@;
	};

	class KVStoreFTest : public KVStoreFTestBase
	{
	public:

		KVStoreFTest();
		KVStoreFTest(string Type);
		KVStoreFTest(const KVStoreFTest& Item);
		KVStoreFTest& operator = (const KVStoreFTest& Item);
		~KVStoreFTest();

		string GetType();

		bool PopulateDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly);
		bool InitializeValueFiles();
////		@PublicVirtualUserFunctionReturnType3@ @PublicVirtualUserFunction3@(@PublicVirtualUserFunctionParameterListDeclarations3@);
		string GetDescription();
		pair<ArrayIndex, ArrayIndex> GetStartingAndEndingTimes();
		bool AccessDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly);

	protected:
		KVStoreFTestBase * m_Worker;

	private:
		KVStoreFTestBase * GetNewWorker(string Type);

		void Release();
	};

	extern KVStoreFTestDummyType KVStoreFTestDummy;
};

#endif
