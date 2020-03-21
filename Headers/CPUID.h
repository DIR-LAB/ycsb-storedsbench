#ifndef CPUID_H
#define CPUID_H

#ifdef _WIN32
#include <limits.h>
#include <intrin.h>
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;

#else
#include <stdint.h>
#endif

namespace UserModeVirtualMemory
{
	class CPUID {
		uint32_t regs[4];

	public:
		explicit CPUID(int i) {
#ifdef _WIN32
			__cpuid((int *)regs, i);

#else
			asm volatile
				("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
					: "a" (i), "c" (0));
#endif
		}

		static bool CLWB();
	};
}
#endif