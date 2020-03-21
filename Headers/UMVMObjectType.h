//UMVMObjectType.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef UMVM_OBJECTTYPE_H
#define UMVM_OBJECTTYPE_H

namespace UserModeVirtualMemory
{
	//Note: this MUST MATCH the definition of TypeID in UMVMItemReferenceGenerated.h
	//  Otherwise, terrible things will happen!

	enum class ObjectType : uint16_t
	{
		UnusedItem = 0,
		RawBlock,
		ShortString,
		LongString,
		MultipleRawBlockSlot
	};
};

#endif