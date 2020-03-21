//UMVMStringVector.h
//Copyright (c) 2019 by Chrysalis Software Corporation. All rights reserved.

/****keyword-flag*** "%v %f %n" */
/* "13 6-May-98,8:40:02 NEWQUANT.H" */

#ifndef UMVM_STRINGVECTOR_H
#define UMVM_STRINGVECTOR_H

//#include <windows.h>

namespace UserModeVirtualMemory
{

	class UMVMFlexArrayRef;
	class UMVMFlexArrayRefConst;

	class StringVector
	{
		friend class UMVMFlexArrayRef;
		friend class UMVMFlexArrayRefConst;
		friend class UserModeVirtualMemoryFile;
		friend class ValueView;
		friend class UMVMItemReferenceLongString;
		friend class UMVMItemReferenceShortString;
		friend class ThreeMissesPro;

	private:
		UserModeVirtualMemoryFile* m_QF;
		string m_Temp;
		bool m_FlushOnClose;
		string m_ArrayName;

		ArrayIndex m_ElementCount;
		ArrayIndex m_MaximumElementCount;

		BlockNumber m_BigBlockNumber;
		mutable vector<BlockNumber> m_NestedBigBlockNumbers;

		mutable BlockNumber m_CachedNestedBigBlockNumber;
		mutable ArrayIndex 	m_CachedNestedBigBlockIndex;

		BlockNumber GetNestedStringBigBlockNumber();

		string GetNestedStringElement(ArrayIndex ElementNumber) const;

		ValueView GetPinnedElementReferenceToModifiableNestedStringElement(ArrayIndex ElementNumber);
		ValueView GetPinnedElementReferenceToModifiableNestedStringElementPriority(ArrayIndex ElementNumber) const;

		ValueView GetPinnedElementReferenceToReadableNestedStringElementPriority(ArrayIndex ElementNumber) const;

		ValueView GetPinnedElementReferenceToReadableNestedStringElement(ArrayIndex ElementNumber) const;

		pair<BlockNumber, ArrayIndex> GetNestedStringParameters(ArrayIndex ElementNumber) const;
		
		StringVector(const StringVector&) = delete;
		StringVector operator= (const StringVector&) = delete;

		StringVector(UserModeVirtualMemoryFile* UMVMF);

		struct CachingNestedParameterLookupReturnTypeStruct
		{
			BlockNumber BlockNumberResult;
			ArrayIndex NestedBlockNumberIndex;
			ArrayIndex NestedElementIndex;
		};

		void UpdateNestedParameters(const BigPointerBlock& ThisBigPointerBlock, const CachingNestedParameterLookupReturnTypeStruct& CachingResult) const;

		CachingNestedParameterLookupReturnTypeStruct CachingNestedParameterLookup(ArrayIndex ElementNumber) const;

		pair<BlockNumber, ArrayIndex> ComputeParametersForPutNestedInternalOperation(ArrayIndex ElementNumber) const;

	public:
		~StringVector();

		StringVector(UserModeVirtualMemoryFile* UMVMF, const string& p_ArrayName, ArrayIndex p_ElementCount = 0, ArrayIndex p_MaxElementCount = UINT_MAX - 1);
		StringVector& operator = (StringVector&& other) noexcept;

		StringVector();

		void Open(const string& p_ArrayName, BlockNumber p_MainBlockNumber = QuantumNotFound, ArrayIndex p_ElementCount = 0, ArrayIndex p_MaxElementCount = UINT_MAX - 1);

		UMVMFlexArrayRef operator[](ArrayIndex p_ElementIndex);
		UMVMFlexArrayRefConst operator[](ArrayIndex p_ElementIndex) const;

		string GetData(ArrayIndex p_ElementIndex) const;

		ValueView GetValueView(ArrayIndex p_ElementIndex);
		ValueView GetValueView(ArrayIndex p_ElementIndex) const;

		void PutNestedStringElement(AccessVectorChar p_Element, ArrayIndex ElementNumber);

		void PutNestedStringElementPriority(AccessVectorChar p_Element, ArrayIndex ElementNumber);

		void CheckValidSize() const;

		ObjectType GetElementType(ArrayIndex p_ElementIndex) const;

		bool ReplaceStringElementUMVMItemReferenceWithUserSpecifiedUMVMItemReference(ItemReferenceStruct UD,
			ArrayIndex ElementNumber, bool AbandonOldStringElement = false);

		BigArrayIndex GetDirectoryEntryNumber();

		ArrayIndex size() const;

		void resize(ArrayIndex p_NewElementCount);

		void ClearElementCount();
		ArrayIndex GetMaximumElementNumber();
		ArrayIndex GetMaximumElementCount();

		void Flush();
		void Close();

		BlockNumber GetBigBlockNumber();

		static const string StringVectorTypeSuffix;
	};

	class UMVMFlexArrayRef
	{
	private:
		ArrayIndex m_ElementIndex;
		StringVector* m_UMVMSV;

	private: //prevent compiler warnings
		UMVMFlexArrayRef& operator=(const UMVMFlexArrayRef&) = delete;

	public:
		UMVMFlexArrayRef(StringVector* p_UMVMSV, ArrayIndex p_ElementIndex);
		UMVMFlexArrayRef & operator=(AccessVectorChar p_ROE);
		UMVMFlexArrayRef& operator=(const string& p_ROE);
		UMVMFlexArrayRef& operator=(const char* p_ROE);
		operator string();
	};

	class UMVMFlexArrayRefConst
	{
	private:
		ArrayIndex m_ElementIndex;
		const StringVector* m_UMVMSV;

	private: //prevent compiler warnings
		UMVMFlexArrayRefConst& operator=(const UMVMFlexArrayRefConst&);

	public:
		UMVMFlexArrayRefConst(const StringVector* p_UMVMSV, ArrayIndex p_ElementIndex);
		operator string();
	};
}
#endif
