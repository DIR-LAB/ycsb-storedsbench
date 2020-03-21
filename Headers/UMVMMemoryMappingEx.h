//UMVMMemoryMappingEx.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVM_MEMORY_MAPPING_EX_H
#define UMVM_MEMORY_MAPPING_EX_H

	namespace UserModeVirtualMemory
	{
		class UMVMMemoryMappingEx : public UMVMMemoryMapping
		{
		public:
			UMVMMemoryMappingEx();
			~UMVMMemoryMappingEx();

			bool SetupMapping(const string & PrimaryFileName, UserModeVirtualMemoryFile* PrimaryFile,
				MemoryMappingMode FileMode, bool Caching, bool InitializeOnAllocationDueToLinuxBug,
				bool IndexFileInRAMWasRequested = false, const string& OverflowFileName = "");
			char* GetAddressInMappedFile(int64_t OffsetInFile, int64_t DataLength, bool Appending) const;
		};

	};

#endif

