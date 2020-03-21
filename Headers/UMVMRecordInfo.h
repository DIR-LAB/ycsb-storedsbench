#ifndef UMVM_RECORD_INFO_H
#define UMVM_RECORD_INFO_H

namespace UserModeVirtualMemory
{
	enum class RecordType : uint8_t { Unused = 0, Direct = 1, ShortIndirect = 0xfe, LongIndirect = 0xff };
	typedef uint16_t KeyHashType;
	typedef uint8_t DirectLengthType;
	typedef uint16_t ShortIndirectLengthType;
	typedef uint64_t LongIndirectLengthType;

	class RecordInfo
	{
		friend class ThreeMissesHashingRobinHoodHashVariableForMappedValueFile;
	public:
		RecordInfo(AccessVectorChar Key, AccessVectorChar Value, ArrayIndex TrancheHash);
		RecordInfo(AccessVectorChar AV, size_t Index); //instantiate resident info from tranche

		bool NeedToCompareKeys(ArrayIndex TrancheHash);

		ArrayIndex CalculateLongRecordBlockCount(UserModeVirtualMemoryFile& DataUMVMF);

		DirectLengthType GetResidentRecordSize();

		size_t WriteRecord(UserModeVirtualMemoryFile& DataUMVMF, AccessVectorChar AV, size_t Index);
		void DeleteRecord(UserModeVirtualMemoryFile& DataUMVMF);

		AccessVectorChar GetKey(UserModeVirtualMemoryFile& DataUMVMF, AccessVectorChar AV, size_t Index);
		ValueView GetValue(UserModeVirtualMemoryFile& DataUMVMF, AccessVectorChar AV, size_t Index);

		ValueView GetValueIfKeyMatches(UserModeVirtualMemoryFile& DataUMVMF, AccessVectorChar Key, AccessVectorChar AV, size_t Index);

	protected:

		ShortIndirectionInfoStruct m_SIIS;
		ArrayIndex m_FirstBlockNumber;

		RecordType m_RecordType;
		AccessVectorChar m_Key;
		AccessVectorChar m_Value;
		KeyHashType m_KeyHash;

		RecordType SetupKeyAndValueIndirectionInfo(AccessVectorChar Key, AccessVectorChar Value);
		static RecordType CalculateEffectiveRecordType(RecordType TypeToCheck);
		static KeyHashType CalculateKeyHashFromTrancheHash(ArrayIndex TrancheHash);
		static AccessVectorChar GetValueIfDirectKeyMatches(AccessVectorChar Key, AccessVectorChar AV, size_t Index);
		static DirectLengthType GetResidentDirectRecordSize(AccessVectorChar AV, size_t Index);
		static KeyHashType GetKeyHashFromAVForIndirectRecord(AccessVectorChar AV, size_t Index);
		static DirectLengthType GetResidentRecordSizeForIndirectRecord(AccessVectorChar AV, size_t Index);
	};
};

#endif

