//ThreeMissesUtilities.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef THREE_MISSES_UTILITIES_H
#define THREE_MISSES_UTILITIES_H

namespace UserModeVirtualMemory
{
	class ThreeMissesUtilities
	{
	public:
		static const size_t FilePositionBufferSize = 8;
		static const size_t FilePositionSize = 6;  //48 bits of file position should be enough for anyone but we have to initialize the whole thing
		static const size_t MinimumKeySize = 5;

		static uint64_t StartingWallTime;

		static bool FileExists(const string& FullFilePath);
		static bool DisplayProgress(ofstream& OutputStream, bool Update, uint64_t Latency,
			bool Initialize, bool Finalize, bool CheckEveryTime, int RecordsPerCall = 1);

		static void DisplayFileSize(ofstream & OutputStream, UserModeVirtualMemoryFile & umvmf, const string & Type);
		static bool IsKeyPressed(char Key);

#if ENABLE_PRAGMAS_TO_DISABLE_OPTIMIZATION
		__declspec(noinline)
#endif
		static BigArrayIndex KVStoreSlotOptimizationCalculator(UserModeVirtualMemoryFile* UMVMF);
		static size_t CalculateHashTableDataEntrySize(bool DataOutOfLine, size_t DataSize);

		static void SetUpKey(string& Key, int64_t KeySize, int64_t Seed, bool Variable, bool GenerateASCIIKeys);
		static size_t CalculateMaximumKeySizeForTest(size_t TypicalKeySize);
		static vector<size_t> GeneratePermutedIndices(size_t Count, ofstream& OutputStream);

		static string GetTestVectorName();

		static bool AccessVectorSwap(AccessVectorChar A, AccessVectorChar B);

		static void SetStringToConcatenationOfPassIndicatorAndTestData(string& Result, const string& BufferFilledWithPassIndicator, AccessVectorChar TestData);

		struct KVStoreParameterDescription
		{
			bool m_Mandatory;
			bool m_IncludeInStatsFileName;
			bool m_ValueIsNumeric;
			size_t* m_NumericValue;
			string* m_StringValue;
			bool m_ValueAssigned;
			string m_LongDescription;
			KVStoreParameterDescription(string LongDescription, bool Mandatory, bool IncludeInStatsFileName, bool ValueIsNumeric,
				string* StringVariable, size_t* NumericVariable) :
				m_LongDescription(LongDescription), m_Mandatory(Mandatory), m_IncludeInStatsFileName(IncludeInStatsFileName),
				m_ValueIsNumeric(ValueIsNumeric), m_StringValue(StringVariable), m_NumericValue(NumericVariable), m_ValueAssigned(false)
			{}
		};

		struct KVMainProgramParameters
		{
			string IndexPrimaryFileName;
			string IndexSecondaryFileName;
			string ValuePrimaryFileName;
			string ValueSecondaryFileName;

			string IndexPrimaryAccessMethod;
			size_t IndexPrimaryStorageAllocation;
			size_t IndexPrimaryStorageLimit;
			size_t ValuePrimaryStorageLimit;

			string Operation;

			size_t RecordCount;
			size_t StartingRecordCountForAppend;
			size_t KeyLength;
			size_t ValueLength;

			size_t TypicalKeyLength;
			size_t TypicalValueLength;

			size_t MaximumKeyLength;
			size_t MaximumValueLength;

			size_t FlushUpdates;

			size_t TestWarmupTime;
			size_t TestRunTime;

			size_t UpdateFrequencyInEighths;
			size_t UpdateWithSameData;

			size_t AllocationMultiplierInTenths;

			size_t PermuteIndices;
			size_t FixedLengthForAllTypes;

			size_t SlotCount;
			size_t GenerateASCIIKeys;
			size_t AutoWarmup;
			size_t PrecacheIndex;

			size_t RequireDAXVolumes;

			bool Valid;

			vector<string> ContentsForStatFileName;
			map<string, ThreeMissesUtilities::KVStoreParameterDescription> ParameterDescriptions;

			KVMainProgramParameters(int argc, char* argv[]);
		};
	};

};

#endif
