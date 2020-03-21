//ThreeMisses.inc
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

		void SetupRawSlotBlockOptimizationForNewKVStore(UserModeVirtualMemoryFile* IndexFile, BigArrayIndex SlotCount);

		int GetOptimizationLevel();

		ArrayIndex GetBlockNumberForSlot(ArrayIndex SlotNumber, UserModeVirtualMemoryFile* IndexFile) const;

		void SetBlocksPerSlot(ArrayIndex BlocksPerSlot);

	protected:
		ArrayIndex m_RehashCount;
		int64_t m_TotalRehashTimeInCycles;

	private:
		static ThreeMissesBasic ThreeMissesBasicDefault;

	public:

