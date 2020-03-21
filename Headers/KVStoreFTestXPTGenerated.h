//KVStoreFTestXPTGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef KVStoreFTestXPTGenerated_H
#define KVStoreFTestXPTGenerated_H

namespace UserModeVirtualMemory
{
	class KVStoreFTestXPT : public KVStoreFTestBasic
	{

	public:
		explicit KVStoreFTestXPT(const KVStoreFTestDummyType&, string Type);
		~KVStoreFTestXPT();

	protected:

		explicit KVStoreFTestXPT(string Type);

		KVStoreFTestXPT();

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
