//UMVMWindowsUtilities.h
//Copyright (c) 2019 by Chrysalis Software Corporation.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)


#ifndef WindowsUtilities_H
#define WindowsUtilities_H

typedef void* HANDLE;

namespace UMVMWindowsUtilities
{
	static const int WHITE = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
	static const int GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	static const int RED = FOREGROUND_RED | FOREGROUND_INTENSITY;
	static const int AQUA = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	static const int MAGENTA = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
	static const int BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	static const int YELLOW = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

	static const int PROGRAM_COLOR = MAGENTA;
	static const int ERROR_COLOR = RED;
	static const int INFORMATION_COLOR = AQUA;
	static const int SUCCESS_COLOR = GREEN;
	static const int COMMAND_COLOR = YELLOW;
	static const int PLAIN_COLOR = WHITE;

	void DisplayTextInColor(vector<string>& Text, int Color, bool CR);

	void DisplayTextInColor(const string & Text, int Color, bool CR);

	void SetThreadAffinity();
};

#endif
