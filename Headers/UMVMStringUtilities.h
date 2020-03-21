//StringUtilities.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H

#include <string>

#include "UMVMBlocki.h"

namespace UserModeVirtualMemory
{
	namespace StringUtilities
	{
		std::pair<std::string, BlockNumber> GetNameAndBlockNumberFromSerializedData(const std::string& SerializedData, char Separator = '\x1');
		std::string CombineDataWithSeparator(const std::string& Name, BlockNumber Number, char Separator = '\x1');

		pair<string, string> SplitSerializedDataAtSeparator(const string& SerializedData, char Separator, bool Last);
	}
}

#endif
