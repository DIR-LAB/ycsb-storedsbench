//UMVMDate.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVMDATE_H
#define UMVMDATE_H

namespace UserModeVirtualMemory
{
	class Date
	{
	private:
		short m_Year;
		char m_Month;
		char m_Day;
	public:
		Date() : m_Year(0), m_Month(0), m_Day(0) {}
		Date(int Year, int Month, int Day) : m_Year(Year), m_Month(Month), m_Day(Day) {}
		int year() const { return m_Year; }
		int month() const { return m_Month; }
		int day() const { return m_Day; }
		bool operator == (const Date& other) { return ((m_Day == other.m_Day) && (m_Month == other.m_Month) && (m_Year == other.m_Year)); }
		bool operator != (const Date& other) { return !(*this == other); }
	};
}
#endif