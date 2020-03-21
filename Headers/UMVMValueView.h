//UMVMValueView.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVM_VALUE_VIEW_H
#define UMVM_VALUE_VIEW_H

namespace UserModeVirtualMemory
{
	class ValueView
	{
	public:
		ValueView(UserModeVirtualMemoryFile * UMVMF, AccessVectorChar AV, BlockNumber BlockToBePinned,
			ObjectType Type, bool MappedLongString);

		ValueView(AccessVectorChar AV);
		ValueView(AccessVectorChar AV, RecordInfo RI);

		ValueView();
		~ValueView();
		ValueView(ValueView&& other) noexcept;
		ValueView& operator=(ValueView&& other) noexcept;

		ValueView& operator=(ValueView& other) = delete;
		ValueView(ValueView& other) = delete;

		bool SetAccessVectorSize(size_t NewSize);

		AccessVectorChar GetReadAccessVector() const;
		AccessVectorChar GetUpdateAccessVector() const;
		BlockNumber GetValueViewBlockNumber() const;

		ObjectType GetObjectType() const;

		char* data();

		int	operator==(const ValueView& p_AccessVector) const;

		char & operator[](size_t p_ArrayIndex) const;
		char& operator[](size_t p_ArrayIndex);

		size_t size() const;
		operator bool() const;

		size_t ReservedSize() const;

		bool DataIsMappedLongString() const;
		bool IsWriteable() const;

		RecordInfo GetRecordInfo() const;

	private:
		UserModeVirtualMemoryFile * m_QF;
		AccessVectorChar m_AV;
		TrackableBlockNumber m_BlockNumber;
		ObjectType m_Type;
		bool m_MappedLongString;

		RecordInfo m_RecordInfo;
		//uint64_t m_SequenceNumber;

		void TrapForSequenceNumber();

		static uint64_t s_SequenceNumber;
	};
}

#endif