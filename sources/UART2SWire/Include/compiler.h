
#pragma once

#ifdef __GNUC__
#define _attribute_packed_		__attribute__((packed))
#define _attribute_aligned_(s)	__attribute__((aligned(s)))
#define _attribute_session_(s)	__attribute__((section(s)))
#define _attribute_ram_code_  	_attribute_session_(".ram_code")
#define _attribute_custom_code_  	_attribute_session_(".custom") volatile
#define _attribute_no_inline_   __attribute__((noinline)) 
// #define _inline_ 				extern __attribute__ ((gnu_inline)) inline
#else
#define _attribute_packed_
#define _attribute_aligned_(s)	__declspec(align(s))
#define _attribute_session_(s)
#define _attribute_ram_code_
#define _attribute_custom_code_
#define _attribute_no_inline_   __declspec(noinline) 
#endif
#define _inline_ 				inline				//   C99 meaning

