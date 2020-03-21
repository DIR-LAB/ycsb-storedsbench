//ThreeMissesCIterGenerated.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

#ifndef ThreeMissesCIterGenerated_H
#define ThreeMissesCIterGenerated_H

namespace UserModeVirtualMemory
{
	class ThreeMissesCIter;
	
	class ThreeMissesCIterBase;

	struct ThreeMissesCIterDummyType {};

	typedef std::map<string, ThreeMissesCIterBase*> ThreeMissesCIterTypeMap;

	class ThreeMissesCIterBase
	{
	friend class ThreeMissesCIter;
		friend class ThreeMissesCIter;
		friend class ThreeMissesCIterFree;
		friend class ThreeMissesFree;
		friend class ThreeMissesCIterPro;
		friend class ThreeMissesPro;
		friend class ThreeMissesBase;

	
	public:
		virtual string GetType() const;
	public:
		typedef int64_t difference_type;
		typedef size_t size_type;
		typedef pair<string, string> value_type;

		virtual ThreeMissesCIterBase operator++();
		virtual value_type operator*() const;
		
		virtual pair<AccessVectorChar,AccessVectorChar> GetKeyAndValueFromIterator() const;

		bool operator !=(ThreeMissesCIterBase other) const;
		bool operator ==(ThreeMissesCIterBase other) const;

	protected:
		ThreeMissesCIterBase(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesBasic& StringMap);

	protected:
		ArrayIndex m_SlotIndex;
		size_t m_PositionInSlot;
		AccessVectorChar m_SlotAV;
		UserModeVirtualMemoryFile* m_DataUMVMFPtr;
		size_t m_ElementLength;
		const ThreeMissesBasic& m_StringMap;

	public:
		static vector<string> GetList();

		virtual ~ThreeMissesCIterBase();

		static ThreeMissesCIterBase* FindType(string Type);
		
		static string ThreeMissesPrefix;

	protected:
		static ThreeMissesCIterTypeMap& GetTypeMap();
		static void AddTypeToMap(ThreeMissesCIterBase* Type);

		virtual ThreeMissesCIterBase* GetNewWorker(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIter& StringMap);
		virtual ThreeMissesCIterBase* GetNewWorker();
		
		ThreeMissesCIterBase();

		int m_Count;
	};


	class ThreeMissesCIterBasic : public ThreeMissesCIterBase
	{
		friend class ThreeMissesCIter;

	protected:
		ThreeMissesCIterBasic();
		ThreeMissesCIterBasic(string Type);
		ThreeMissesCIterBasic(const ThreeMissesCIterDummyType&, string Type);
		ThreeMissesCIterBasic(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesBasic& StringMap);
		string GetType() const;
		~ThreeMissesCIterBasic();
		ThreeMissesCIterBase* GetNewWorker(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIter& StringMap);
		ThreeMissesCIterBase* GetNewWorker();

		string m_ThreeMissesCIterType;

	protected:
#include "ThreeMissesCIter.inc"		

	};

	class ThreeMissesCIter : public ThreeMissesCIterBase
	{
	public:

		ThreeMissesCIter();
		ThreeMissesCIter(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIter& StringMap);
		ThreeMissesCIter(const ThreeMissesCIter& Item);
		ThreeMissesCIter& operator = (const ThreeMissesCIter& Item);
		~ThreeMissesCIter();

		string GetType() const;

	protected:
		ThreeMissesCIterBase * m_Worker;

	private:
		ThreeMissesCIterBase * GetNewWorker(ArrayIndex SlotIndex, size_t PositionInSlot, AccessVectorChar SlotAV, UserModeVirtualMemoryFile* DataUMVMFPtr, size_t ElementLength, const ThreeMissesCIter& StringMap);
		ThreeMissesCIterBase * GetNewWorker();
		
		void Release();
	
	};

	extern ThreeMissesCIterDummyType ThreeMissesCIterDummy;
};


#endif
