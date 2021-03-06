/* 标准头文件 */
#ifndef __STDEFINE_H__
#define __STDEFINE_H__

#ifdef _MSC_VER
#include <windows.h>
#pragma warning(disable:4996)
#else
#include <stdint.h>
#define TRUE     1
#define FALSE    0
typedef int      BOOL ;
typedef uint8_t  BYTE ;
typedef uint16_t WORD ;
typedef uint32_t DWORD;
#endif

//++ for fixed point
#define FIXQ          11
#define FLOAT2FIX(f)  ((int)((f) * (1 << 11)))
//-- for fixed point

#define ALIGN(x, y)   (((x) + (y) - 1) / (y) * (y))

#endif


