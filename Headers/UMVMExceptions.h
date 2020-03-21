//UMVMExceptions.h
//Copyright (c) 2019 by Chrysalis Software Corporation.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)


#ifndef UMVMEXCEPTIONS_H
#define UMVMEXCEPTIONS_H

class UMVMException
#if UMVM_WINDOWS
 : public std::exception
#else
 : public std::runtime_error
#endif
{
public:
	UMVMException(const char* Message);
	UMVMException(const string& Message);
};

class UMVMExceptionOutOfStorage : public UMVMException
{
public:
	UMVMExceptionOutOfStorage(const char* Message);
	UMVMExceptionOutOfStorage(const string& Message);
};

class UMVMExceptionOutOfAddressSpace : public UMVMException
{
public:
	UMVMExceptionOutOfAddressSpace(const char* Message);
	UMVMExceptionOutOfAddressSpace(const string& Message);
};

class UMVMExceptionBlockNumberOverflow : public UMVMException
{
public:
	UMVMExceptionBlockNumberOverflow(const char* Message);
	UMVMExceptionBlockNumberOverflow(const string& Message);
};

class UMVMExceptionLogicError : public UMVMException
{
public:
	UMVMExceptionLogicError(const char* Message);
	UMVMExceptionLogicError(const string& Message);
};

class UMVMExceptionIndexFileAccessError : public UMVMException
{
public:
	UMVMExceptionIndexFileAccessError(const char* Message);
	UMVMExceptionIndexFileAccessError(const string& Message);
};

class UMVMExceptionExternalFileAccessError : public UMVMException
{
public:
	UMVMExceptionExternalFileAccessError(const char* Message);
	UMVMExceptionExternalFileAccessError(const string& Message);
};

#endif // !UMVMEXCEPTIONS_H

