//KVStoreFTestAGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef KVStoreFTestAGenerated_H
#define KVStoreFTestAGenerated_H

namespace UserModeVirtualMemory
{
	class KVStoreFTestA : public KVStoreFTestBasic
	{

	public:
		explicit KVStoreFTestA(const KVStoreFTestDummyType&, string Type);
		~KVStoreFTestA();

	protected:

		explicit KVStoreFTestA(string Type);

		KVStoreFTestA();

		KVStoreFTestBase* GetNewWorker(string Type);

		bool PopulateDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly);
		bool InitializeValueFiles();
////		@PublicVirtualUserFunctionReturnType3@ @PublicVirtualUserFunction3@(@PublicVirtualUserFunctionParameterListDeclarations3@);
		string GetDescription();
		pair<ArrayIndex, ArrayIndex> GetStartingAndEndingTimes();
		bool AccessDatabase(const string& ValuePrimaryFileName, const string& ValueOverflowFileName, size_t KeySize, size_t DataSize, size_t BufferForPassIndicatorSize, size_t PreviousCount, size_t Count, ofstream& OutputStream, UserModeVirtualMemoryFile::ThreeMissesFileOptions Options, bool AppendOnly);

////		@PublicUserFunctionReturnType1@ @PublicUserFunction1@(@PublicUserParameterListDeclarations1@);
	};
};

#endif
