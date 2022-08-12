#pragma once

#if !defined(HDTJMTYPE_H)
#define HDTJMTYPE_H
// general types
typedef unsigned char byte_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long long ull_t;
// boolean_t �迭�� ����ϰų� 1 byte boolean ���� �迭�� �ʿ��Ѱ�� byte_t ����� ������
typedef enum { False, True } boolean_t;

#if defined (_WIN64)
// x86_64
typedef long long Word_t;
#else
// x86
typedef int Word_t;
#endif

#endif
