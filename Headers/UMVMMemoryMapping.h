//UMVMMemoryMapping.h
//Copyright (c) 2019 by Chrysalis Software Corporation.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef UMVM_MEMORY_MAPPING_H
#define UMVM_MEMORY_MAPPING_H

	namespace UserModeVirtualMemory
	{
		struct UMVMLLFIOResult;

		enum class MemoryMappingMode { read, write };

		class UMVMMemoryMapping
		{

		public:
			static const int64_t AllocationUnit = 1024 * 1024 * 1024;

			class MappingInfo
			{
				friend class UMVMMemoryMapping;
				friend class UMVMMemoryMappingEx;
				friend class ThreeMissesPro;

				MappingInfo();
				MappingInfo(const string& FileName, const string& FileNameDirectory);

				int64_t GetRelativeAddressLimit() const;

			private:
				void SetRelativeAddressLimit(int64_t NewRelativeAddressLimit) const;
				mutable int64_t m_RelativeAddressLimit;
				string m_FileName;
				string m_FileNameDirectory;
				int64_t m_SegmentLogicalFileSize;
				char* m_MemoryMapBaseAddress;
				int64_t m_Reservation;
				bool m_ThreeMissesIndexFile;
			};

		public:
			UMVMMemoryMapping();

			virtual ~UMVMMemoryMapping();

			virtual bool SetupMapping(const string & PrimaryFileName,
				UserModeVirtualMemoryFile* PrimaryFile, MemoryMappingMode FileMode, bool Caching,
				bool ThreeMissesIndexFile, bool IndexFileInRAMWasRequested = false, const string& OverflowFileName = "");
			virtual char* GetAddressInMappedFile(int64_t OffsetInFile, int64_t DataLength, bool Appending) const;

			int64_t size() const;
			void Close();
			void Delete();
			void Flush();
			char* GetBlockAddress(const BlockNumber BN);
			int64_t GetRelativeAddressLimit() const;
			void SetRelativeAddressLimit(int64_t RelativeAddressLimit) const;

			const int64_t s_MaximumReservation = 1LL << 42;
			static const int s_BatchSize = 1024 * 1024;

		protected:
			char * GetAddressInSpecifiedMappedFile(UMVMLLFIOResult* m_LLFIOResult, MappingInfo& MI,
				const string& FileNameForErrorMessage, int64_t OffsetInFile, int64_t DataLength, bool Appending) const;

			void Free(UMVMLLFIOResult*& LLFIOResult) const;
			bool MapFile(UMVMLLFIOResult*& LLFIOResult, MappingInfo& MI, MemoryMappingMode FileMode,
				bool Caching, bool);
			bool DecreaseSegmentSize(UMVMLLFIOResult*& LLFIOResult, int64_t NewSize) const;
			bool IncreaseSegmentSize(UMVMLLFIOResult*& LLFIOResult, MappingInfo& MI, int64_t NewSize) const;

			mutable MappingInfo m_PrimaryMapInfo;
			mutable MappingInfo m_OverflowMapInfo;

			UMVMLLFIOResult* m_PrimaryFileLLFIOResult;
			UMVMLLFIOResult* m_OverflowFileLLFIOResult;

			size_t m_AppendCount;

			UserModeVirtualMemoryFile* m_PrimaryFile;
		};
	};

#endif

