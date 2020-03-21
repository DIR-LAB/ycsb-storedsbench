//ThreeMissesTypeDefinitions.cpp
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef THREE_MISSES_TYPE_DEFINITIONS_H
#define THREE_MISSES_TYPE_DEFINITIONS_H

namespace UserModeVirtualMemory
{
	namespace ThreeMissesTypeDefinitions
	{
		typedef uint64_t LengthType;

		struct OccupiedLengthType {
			LengthType _v;
			operator LengthType() { return _v; }
			OccupiedLengthType(LengthType v) : _v(v) {}
		};

		struct MultipleRawBlockSlotRedirectionStruct
		{
			BlockNumber m_StartingBlockNumber;
			ArrayIndex m_BlockCount;
			MultipleRawBlockSlotRedirectionStruct(BlockNumber BN, ArrayIndex BC) : m_StartingBlockNumber(BN), m_BlockCount(BC) {}
		};

		static const unsigned char MultipleRawBlockSlotRedirectMarkerForNewFile = 0x82;
	};

};
#endif

