//UMVMCommon.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMCOMMON_H
#define UMVMCOMMON_H

namespace UserModeVirtualMemory {};
namespace UMVM = UserModeVirtualMemory;

#define USE_INTEL_CRC 0
#define TRACK_BLOCK_RELOADS 0
#define TRACK_STORAGE_ACCESS_TIMES 0
#define REMOVE_FREE_BLOCKS_ON_CLOSE 0
#define TRACK_TOTAL_SEARCH_COUNT 0

#if UMVM_WINDOWS
#define ENABLE_PRAGMAS_TO_DISABLE_OPTIMIZATION 0
#else
#define ENABLE_PRAGMAS_TO_DISABLE_OPTIMIZATION 0
#endif

#define TRACK_MAXIMUM_PROBE_LENGTH 0

#include "UMVMCommonTypeDeclarations.h"

#include "UMVMBlockNumber.h"

#include "UMVMTypes.h"

namespace UserModeVirtualMemory
{
    class ThreeMissesCIterBase;
    class ThreeMissesCIterBasic;
    class UMVMItemReferenceRawBlock;
	class ThreeMissesPro;
	extern int s_RevisionNumber;
	extern int64_t GlobalElementNumber;
	enum class UMVMFileType { NormalUMVMFile = 'N', ThreeMissesIndexFile = 'I',	ThreeMissesValueFile = 'V'};
  
}

#include <vector>
#include <map>
#include <string>
using std::map;
using std::string;
using std::vector;

#if UMVM_WINDOWS
#else
typedef unsigned long DWORD;
typedef long LONG;
typedef long long LONGLONG;
typedef void* HANDLE;
typedef void* LPVOID;

typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		LONG  HighPart;
	} DUMMYSTRUCTNAME;
	struct {
		DWORD LowPart;
		LONG  HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER;

typedef DWORD(*LPPROGRESS_ROUTINE)(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData
	);
#endif


#include "ThreeMissesRecordInfo.h"
#include "CPUID.h"
#include "UMVMFunctionTimer.h"
#include "UMVMObjectType.h"
#include "UMVMItemRef.h"
#include "UMVMBlocki.h"
#include "UMVMVectorInternal.h"
#include "UMVMDAMGenerated.h"
#include "UMVMBlock.h"
#include "UMVMValueView.h"
#include "UMVMUtility.h"
#include "UMVMVector.h"
#include "UMVMStringVector.h"
#include "UMVMExceptions.h"
#include "UMVMGeneralUtilities.h"
#include "UMVMItemReferenceGenerated.h"

#include "UMVMVectorTypes.h"
#include "ThreeMissesTypeDefinitions.h"
#include "ThreeMissesHashingGenerated.h"

#include "ThreeMissesGenerated.h"
#include "ThreeMissesCIterGenerated.h"

#endif
