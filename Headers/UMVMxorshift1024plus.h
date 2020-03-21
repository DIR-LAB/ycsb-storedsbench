//UMVMXorshift1024plus.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

//Source code adapted from http://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf (figure 2 on page 10).

#ifndef XORSHIFT_1024_PLUS_H
#define XORSHIFT_1024_PLUS_H

namespace UserModeVirtualMemory
{
	class xorshift1024plus
	{
	public:
		xorshift1024plus();
		xorshift1024plus(AccessVectorChar data);
		uint32_t GenerateHash();
		uint64_t xorshift1024plus_next();
		static const size_t Entries = 16;

		static string TestData;
		static const size_t NumberOfDistinctTestEntries = 1000000; //deliberately not a power of two for randomness

#if UMVM_WINDOWS
		_declspec(noinline) 
#endif
		static void GenerateTestData(size_t DataEntryLength);
		static AccessVectorChar GetTestData(size_t DataEntryLength, size_t EntryNumber, bool VariableLength = false);

	private:
		uint64_t s[Entries];
		int p;
	};

};

#endif
