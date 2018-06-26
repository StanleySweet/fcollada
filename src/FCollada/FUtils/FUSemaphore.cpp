/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America

	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FUSemaphore.h"

FUSemaphore::FUSemaphore(uint32 initialValue, uint32 maximumValue)
#ifdef WIN32
:	semaphoreHandle(nullptr)
#endif // WIN32
{
#ifdef WIN32
	FUAssert(initialValue <= maximumValue, ;);
	semaphoreHandle = CreateSemaphore(nullptr, initialValue, maximumValue, nullptr);
#endif
}

FUSemaphore::~FUSemaphore()
{
#ifdef WIN32
	CloseHandle(semaphoreHandle);
#endif
}

void FUSemaphore::Up()
{
#ifdef WIN32
	ReleaseSemaphore(semaphoreHandle, 1, nullptr);
#endif
}

void FUSemaphore::Down()
{
#ifdef WIN32
	WaitForSingleObject(semaphoreHandle, INFINITE);
#endif
}

