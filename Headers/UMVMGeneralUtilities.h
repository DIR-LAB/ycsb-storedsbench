//UMVMGeneralUtilities.h
//Copyright (c) 2019 by Chrysalis Software Corporation.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef UMVM_GENERAL_UTILITIES_H
#define UMVM_GENERAL_UTILITIES_H

typedef int64_t longlong;
using std::pair;

namespace UserModeVirtualMemory
{

string GetVolumeName(const string& FullPath);
size_t GetFreePhysicalMemory();

int64_t GetFreeSpaceOnDevice(const string& Directory);

size_t GetFileSize(const string& FileName);

bool DetectWhetherFileIsOnDAXVolume(const string& FileName);

bool FilesAreOnTheSameVolume(const string& FirstFileName, const string& SecondFileName);

size_t GetSizeWithOptionalModifiers(const string& Param);
string ConvertNumberToString(longlong Number);
string ConvertNumberToStringWithCommas(longlong Number);
string ConvertNumberToStringWithCommas(uint64_t Number);
string ConvertNumberToStringWithCommas(double Number);
string ConvertNumberToStringWithCommas(uint32_t Number);
string ConvertNumberToStringWithCommas(int32_t Number);

string ConvertNumberToStringWithFixedLength(uint64_t Number, int Length, char Pad = ' ');

string GetCurrentDateAndTimeFormatted();

string PadStringToFixedSize(const string & Value, int Length, char Pad = ' ');
string ConvertNumberToStringWithFixedNumberOfDecimalPlaces(double Number, int DecimalPlaces);

string ConvertNumberToStringWithFixedSizeAndNumberOfDecimalPlaces(double Number, int Length, int DecimalPlaces = 3, char Pad = ' ');

string ConvertToLowerCase(const string& Input);
vector<string> ConvertDelimitedStringToVectorOfStrings(const string& Data, const string& Delimiter);
string TrimNullsFromEndOfString(string& Data);

uint64_t CalculateCyclesPerSecond();

string ConvertCyclesToFormattedMicroseconds(uint64_t Cycles, int Length, int DecimalPlaces = 3, char Pad = ' ');

uint64_t CalculateLatencyStatistic(vector<uint64_t> Latencies, double Percentile);

uint64_t FindHighestElement(vector<uint64_t> Latencies);

int64_t GetWallTimeInMilliseconds();
};


#endif
