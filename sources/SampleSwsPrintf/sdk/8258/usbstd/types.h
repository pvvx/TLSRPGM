/*
 * types.h
 *
 *  Created on: 2018-12-15
 *      Author: Administrator
 */
#pragma once

#ifndef TYPES_H_
#define TYPES_H_
#ifndef NULL
#define NULL  0
#endif
typedef unsigned char u8 ;
typedef unsigned short u16 ;
typedef unsigned int u32 ;
typedef signed char s8 ;
typedef signed short s16 ;
typedef signed int s32 ;
typedef enum
{
	false,
	true
}bool;

#ifdef __GNUC__
typedef	u16	wchar_t;
#endif


#endif /* TYPES_H_ */
