//UMVMCommonTypeDeclarations.h
//Copyright (c) 2019 by Chrysalis Software Corporation.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef UMVM_COMMON_TYPE_DECLARATIONS_H
#define UMVM_COMMON_TYPE_DECLARATIONS_H

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <stdint.h>

typedef uint32_t ArrayIndex;
typedef uint64_t BigArrayIndex;

#ifndef FREE_VERSION
#define USE_MEMORY_MAPPING_EX 1
#endif

// Check windows
#if _WIN64
#define ENV64BIT
#endif

// Check GCC
#if __GNUC__
#if __x86_64__
#define ENV64BIT
#endif
#endif

#ifndef ENV64BIT
#error 64-bit implementation on x86 only!
#endif

#if _MSC_VER
#define UMVM_WINDOWS 1
#else
#define UMVM_WINDOWS 0
#endif

#if UMVM_WINDOWS
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include "UMVMWindowsUtilities.h"
#else
#include <signal.h>
#include <unistd.h>
#include <x86intrin.h>
#include <immintrin.h>
#endif

#include <algorithm>

namespace UserModeVirtualMemory
{
	typedef uint64_t BlockOccupiedLengthType;

	const int BlockSizeBits = 15;
	const BlockOccupiedLengthType BlockSize = 1 << BlockSizeBits; // size of each block
};

#endif