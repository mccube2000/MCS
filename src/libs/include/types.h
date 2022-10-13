#ifndef _TYPES_H_
#define _TYPES_H_

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long32_t;
typedef unsigned long ulong32_t;
// typedef long long int64_t;
// typedef unsigned long long uint64_t;

typedef int8_t i8_t;
typedef uint8_t ui8_t;
typedef int16_t i16_t;
typedef uint16_t ui16_t;
typedef int32_t i32_t;
typedef uint32_t ui32_t;
typedef long32_t l32_t;
typedef ulong32_t ul32_t;
// typedef int64_t i64_t;
// typedef uint64_t ui64_t;

typedef unsigned char bool;
#define true (bool)1
#define false (bool)0

typedef char BOOL;
#define TRUE (BOOL)1
#define FALSE (BOOL)0
#define ERROR (BOOL)(-1)

#define NULL 0

#endif
