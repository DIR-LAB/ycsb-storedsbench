//UMVMBlockNumber.h
//Copyright (c) 2019 by Chrysalis Software Corporation.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)


#ifndef UMVM_BLOCK_NUMBER_H
#define UMVM_BLOCK_NUMBER_H

namespace UserModeVirtualMemory
{
	class BlockNumber
	{
	public:
		BlockNumber();
		BlockNumber(ArrayIndex value);
		operator ArrayIndex() const;

		bool operator==(BlockNumber& other) const;
		bool operator==(ArrayIndex other) const;

		bool operator!=(BlockNumber& other) const;
		bool operator!=(ArrayIndex other) const;

		bool operator>=(BlockNumber& other) const;
		bool operator>=(ArrayIndex other) const;

		bool operator<=(BlockNumber& other) const;
		bool operator<=(ArrayIndex other) const;

		bool operator<(BlockNumber& other) const;
		bool operator<(ArrayIndex other) const;
		bool operator<(int other) const;

		bool operator>(BlockNumber& other) const;
		bool operator>(ArrayIndex other) const;

		BlockNumber operator++(int);
	private:
		uint32_t BN;
	};
};

#endif
