//Copyright (c) 2019 by Chrysalis Software Corporation.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef UMVM_LLFIO_INTERFACE_H
#define UMVM_LLFIO_INTERFACE_H

#define LLFIO_LOGGING_LEVEL 1
#define LLFIO_EXPERIMENTAL_STATUS_CODE 1

#if UMVM_WINDOWS
#include "llfio.hpp"
#else
#include "../../llfio/llfio/include/llfio.hpp"
#endif

namespace llfio = LLFIO_V2_NAMESPACE;

using llfio::mapped_file_handle;
using llfio::result;

namespace UserModeVirtualMemory
{
	struct UMVMLLFIOResult
	{
		result<mapped_file_handle>* m_r;
	};
};

#endif
