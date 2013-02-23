/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * ***************************************************************************
 *
 */

#ifndef _AMD_SB_LIB_H_
#define _AMD_SB_LIB_H_

typedef signed char  *va_list;
#ifndef _INTSIZEOF
  #define _INTSIZEOF (n) ( (sizeof (n) + sizeof (UINTN) - 1) & ~(sizeof (UINTN) - 1) )
#endif

// Also support coding convention rules for var arg macros
#ifndef va_start
  #define va_start(ap, v)  ( ap = (va_list)&(v) + _INTSIZEOF (v) )
#endif
#define va_arg(ap, t)    ( *(t *) ((ap += _INTSIZEOF (t)) - _INTSIZEOF (t)) )
#define va_end(ap)      ( ap = (va_list)0 )


#pragma pack (push, 1)

#define IMAGE_ALIGN          32*1024
#define NUM_IMAGE_LOCATION   32

//Entry Point Call
typedef void (*CIM_IMAGE_ENTRY) (void* pConfig);

//Hook Call

typedef struct _CIMFILEHEADER
{
  unsigned int  AMDLogo;
  unsigned long long  CreatorID;
  unsigned int  Version1;
  unsigned int  Version2;
  unsigned int  Version3;
  unsigned int  ModuleInfoOffset;
  unsigned int  EntryPoint;
  unsigned int  ImageBase;
  unsigned int  RelocTableOffset;
  unsigned int  ImageSize;
  unsigned short  CheckSum;
  unsigned char ImageType;
  unsigned char Reserved2;
} CIMFILEHEADER;

#ifndef BIT0
  #define BIT0 (1 << 0)
#endif
#ifndef BIT1
  #define BIT1 (1 << 1)
#endif
#ifndef BIT2
  #define BIT2 (1 << 2)
#endif
#ifndef BIT3
  #define BIT3 (1 << 3)
#endif
#ifndef BIT4
  #define BIT4 (1 << 4)
#endif
#ifndef BIT5
  #define BIT5 (1 << 5)
#endif
#ifndef BIT6
  #define BIT6 (1 << 6)
#endif
#ifndef BIT7
  #define BIT7 (1 << 7)
#endif
#ifndef BIT8
  #define BIT8 (1 << 8)
#endif
#ifndef BIT9
  #define BIT9 (1 << 9)
#endif
#ifndef BIT10
  #define BIT10 (1 << 10)
#endif
#ifndef BIT11
  #define BIT11 (1 << 11)
#endif
#ifndef BIT12
  #define BIT12 (1 << 12)
#endif
#ifndef BIT13
  #define BIT13 (1 << 13)
#endif
#ifndef BIT14
  #define BIT14 (1 << 14)
#endif
#ifndef BIT15
  #define BIT15 (1 << 15)
#endif
#ifndef BIT16
  #define BIT16 (1 << 16)
#endif
#ifndef BIT17
  #define BIT17 (1 << 17)
#endif
#ifndef BIT18
  #define BIT18 (1 << 18)
#endif
#ifndef BIT19
  #define BIT19 (1 << 19)
#endif
#ifndef BIT20
  #define BIT20 (1 << 20)
#endif
#ifndef BIT21
  #define BIT21 (1 << 21)
#endif
#ifndef BIT22
  #define BIT22 (1 << 22)
#endif
#ifndef BIT23
  #define BIT23 (1 << 23)
#endif
#ifndef  BIT24
  #define BIT24 (1 << 24)
#endif
#ifndef BIT25
  #define BIT25 (1 << 25)
#endif
#ifndef BIT26
  #define BIT26 (1 << 26)
#endif
#ifndef BIT27
  #define BIT27 (1 << 27)
#endif
#ifndef BIT28
  #define BIT28 (1 << 28)
#endif
#ifndef BIT29
  #define BIT29 (1 << 29)
#endif
#ifndef BIT30
  #define BIT30 (1 << 30)
#endif
#ifndef BIT31
  #define BIT31 (1 << 31)
#endif

#pragma pack (pop)

typedef enum
{
  AccWidthUint8 = 0,
  AccWidthUint16,
  AccWidthUint32,
} ACC_WIDTH;

#define S3_SAVE  0x80

#endif
