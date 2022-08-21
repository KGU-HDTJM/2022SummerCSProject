#pragma once
#ifndef HDTJMTHREAD_H
#define HDTJMTHREAD_H

#include "HDTJMType.h"

#if defined(THREAD_API_EXPORTS)
#define THREAD_API __declspec(dllexport)
#else
#define THREAD_API __declspec(dllimport)
#endif

THREAD_API void __fastcall SpinlockAcquire(volatile boolean_t* bLock);
THREAD_API void __fastcall SpinlockRelease(volatile boolean_t* bLock);
// volatile�� �����Ϸ� ����ȭ �ɼ��̶� ������
// test and set true boolean
THREAD_API extern boolean_t __fastcall TASBoolean(/*volatile*/boolean_t* bValue);
#endif