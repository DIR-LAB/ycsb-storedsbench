//UMVMTypes.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMTYPES_H
#define UMVMTYPES_H

#define INLINE

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <string.h>

using std::unordered_map;
using std::map;
using std::multimap;
//using std::list;

using std::ostream;
using std::istream;
using std::ofstream;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::ios_base;
using std::min;
using std::setfill;
using std::setw;
using std::ifstream;
using std::getline;
using std::vector;
using std::stringstream;
using std::pair;
using std::set;

namespace UserModeVirtualMemory
{
#define TRUE 1
#define FALSE 0
}

#include "UMVMAccessVector.h"

namespace UserModeVirtualMemory
{
	struct ItemIndex;
	union Block;
	class BaseBlock;
	class QuantumBlock;
	class BigPointerBlock;
	class LittlePointerBlock;
	class LeafBlock;
	class LeafBlockPtr;
	class QuantumBlockPtr;
	class UserModeVirtualMemoryFile;
	class ValueView;

	typedef unsigned short uint16_t;

	class TrackableBigArrayIndex
	{
	public:
		TrackableBigArrayIndex(BigArrayIndex Value);
		operator BigArrayIndex() const;
		BigArrayIndex operator =(BigArrayIndex NewValue);
	private:
		BigArrayIndex m_Value;
	};

	class TrackableBlockNumber
	{
	public:

		TrackableBlockNumber();

		bool operator!=(BlockNumber& other) const;
		bool operator!=(ArrayIndex other) const;

		TrackableBlockNumber(BlockNumber Value);
		operator BlockNumber() const;
		operator ArrayIndex() const;
		TrackableBlockNumber operator =(BlockNumber NewValue);

	private:
		BlockNumber m_Value;
	};

	typedef unsigned short RelativeItemNumber;

	struct FileOffset
	{
		static const int Size = 6;
		char bytes[Size]; //48 bits of offset should be enough for anyone
		FileOffset(BigArrayIndex Index);
		FileOffset();
		operator BigArrayIndex();
	};

	typedef AccessVector<BlockNumber> BigPointerArray;

	struct ShortIndirectionInfoStruct
	{
		ArrayIndex m_BlockNumber;
		uint16_t m_Length;
		uint16_t m_OffsetInBlock;

		ShortIndirectionInfoStruct()
			: m_BlockNumber(0), m_Length(0), m_OffsetInBlock(0)
		{}

		ShortIndirectionInfoStruct(ArrayIndex ThisBlockNumber, BlockOccupiedLengthType Length, BlockOccupiedLengthType OffsetInBlock)
			: m_BlockNumber(ThisBlockNumber), m_Length(static_cast<uint16_t>(Length)), m_OffsetInBlock(static_cast<uint16_t>(OffsetInBlock))
		{}

		ArrayIndex GetBlockNumber() { return m_BlockNumber; }
		uint16_t GetLength() { return m_Length; }
		uint16_t GetOffset() { return m_OffsetInBlock; }
	};


	struct ConfigurationDetails
	{
		int OptimizationLevel;
		BigArrayIndex TotalSearchCount;
		BigArrayIndex SlotCount;
		size_t Rounding;
		ConfigurationDetails()
			: OptimizationLevel(0)
			, TotalSearchCount(0)
			, SlotCount(0)
			, Rounding(0)
		{}
	};

	typedef uint8_t RHVHeaderByteType;

	static const RHVHeaderByteType RHVDirectElementIndicatorByte = 0;
	static const RHVHeaderByteType RHVLargeOverflowElementIndicatorByte = 0xFE;
	static const RHVHeaderByteType RHVSmallOverflowElementIndicatorByte = 0xFF;
}

#endif
