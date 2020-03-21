//UMVMFunctionTimer.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVM_FUNCTION_TIMER_H
#define UMVM_FUNCTION_TIMER_H

#include <string>
#include <vector>
using std::pair;
using std::vector;

namespace UserModeVirtualMemory
{
	class FunctionTimer
	{
	public:
		FunctionTimer(size_t Count, uint64_t Threshold);
		void StartTimingFunction();
		void EndTimingFunction();
		struct TimingResults
		{
			uint64_t m_FastestTime;
			uint64_t m_SlowestTime;
			int64_t m_FastestElementNumber;
			int64_t m_SlowestElementNumber;
			uint64_t m_Average;
			vector<pair<int64_t, uint64_t>> m_Slownesses;
			TimingResults() : m_FastestTime(), m_SlowestTime(), m_FastestElementNumber(), m_SlowestElementNumber(), m_Average(), m_Slownesses() {}
		};

		TimingResults GetTimingResults();

	private:
		size_t m_Count;
		uint64_t m_Threshold;
		uint64_t m_Start;
		size_t m_CurrentCounter;
		vector<uint64_t> m_Timestamps;
	};
};

#endif

