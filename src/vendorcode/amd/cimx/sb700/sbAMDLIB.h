/*;********************************************************************************
;
; Copyright (C) 2012 Advanced Micro Devices, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of
;       its contributors may be used to endorse or promote products derived
;       from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*********************************************************************************/

#ifndef _AMD_AMDLIB_H_
#define _AMD_AMDLIB_H_

typedef CHAR8   *va_list;
#ifndef _INTSIZEOF
	#define _INTSIZEOF(n)( (sizeof(n) + sizeof(UINTN) - 1) & ~(sizeof(UINTN) - 1) )
#endif

// Also support coding convention rules for var arg macros
#ifndef va_start
#define va_start(ap,v)  ( ap = (va_list)&(v) + _INTSIZEOF(v) )
#endif
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )

#ifndef	CIMx_DEBUG
	#define	CIMx_DEBUG	0
#endif


#pragma pack(push,1)

#define	IMAGE_ALIGN		32*1024
#define	NUM_IMAGE_LOCATION	32

//Entry Point Call
typedef void (*CIM_IMAGE_ENTRY)(void* pConfig);

//Hook Call

typedef struct _Reg8Mask
{
	UINT8	bRegIndex;
	UINT8	bANDMask;
	UINT8	bORMask;
}REG8MASK;


typedef struct _CIMFILEHEADER{
	UINT32	AtiLogo;
	UINT32	EntryPoint;
	UINT32	ModuleLogo;
	UINT32	ImageSize;
	UINT16	Version;
	UINT8	CheckSum;
	UINT8	Reserved1;
	UINT32  Reserved2;
}CIMFILEHEADER;

typedef struct _CPUID_DATA{
	UINT32	REG_EAX;
	UINT32	REG_EBX;
	UINT32	REG_ECX;
	UINT32	REG_EDX;
}CPUID_DATA;

#ifndef BIT0
	#define	BIT0					(1 << 0)
#endif
#ifndef BIT1
	#define	BIT1					(1 << 1)
#endif
#ifndef BIT2
	#define	BIT2					(1 << 2)
#endif
#ifndef BIT3
	#define	BIT3					(1 << 3)
#endif
#ifndef BIT4
	#define	BIT4					(1 << 4)
#endif
#ifndef BIT5
	#define	BIT5					(1 << 5)
#endif
#ifndef BIT6
	#define	BIT6					(1 << 6)
#endif
#ifndef BIT7
	#define	BIT7					(1 << 7)
#endif
#ifndef BIT8
	#define	BIT8					(1 << 8)
#endif
#ifndef BIT9
	#define	BIT9					(1 << 9)
#endif
#ifndef BIT10
	#define	BIT10					(1 << 10)
#endif
#ifndef BIT11
	#define	BIT11					(1 << 11)
#endif
#ifndef BIT12
	#define	BIT12					(1 << 12)
#endif
#ifndef BIT13
	#define	BIT13					(1 << 13)
#endif
#ifndef BIT14
	#define	BIT14					(1 << 14)
#endif
#ifndef BIT15
	#define	BIT15					(1 << 15)
#endif
#ifndef BIT16
	#define	BIT16					(1 << 16)
#endif
#ifndef BIT17
	#define	BIT17					(1 << 17)
#endif
#ifndef BIT18
	#define	BIT18					(1 << 18)
#endif
#ifndef BIT19
	#define	BIT19					(1 << 19)
#endif
#ifndef BIT20
	#define	BIT20					(1 << 20)
#endif
#ifndef BIT21
	#define	BIT21					(1 << 21)
#endif
#ifndef BIT22
	#define	BIT22					(1 << 22)
#endif
#ifndef BIT23
	#define	BIT23					(1 << 23)
#endif
#ifndef BIT24
	#define	BIT24					(1 << 24)
#endif
#ifndef BIT25
	#define	BIT25					(1 << 25)
#endif
#ifndef BIT26
	#define	BIT26					(1 << 26)
#endif
#ifndef BIT27
	#define	BIT27					(1 << 27)
#endif
#ifndef BIT28
	#define	BIT28					(1 << 28)
#endif
#ifndef BIT29
	#define	BIT29					(1 << 29)
#endif
#ifndef BIT30
	#define	BIT30					(1 << 30)
#endif
#ifndef BIT31
	#define	BIT31					(1 << 31)
#endif

#define PCI_ADDRESS(bus,dev,func,reg) \
(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) + (((UINT32)func) << 16) + ((UINT32)reg) )

#pragma pack(pop)

typedef enum {
  AccWidthUint8 = 0,
  AccWidthUint16,
  AccWidthUint32,
} ACC_WIDTH;

#define	S3_SAVE 					0x80

#endif	//#ifndef _AMD_AMDLIB_H_
