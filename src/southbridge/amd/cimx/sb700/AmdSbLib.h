/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#pragma pack (pop)

typedef enum
{
	AccWidthUint8 = 0,
	AccWidthUint16,
	AccWidthUint32,
} ACC_WIDTH;

#define S3_SAVE  0x80

/**
 * AmdSbDispatcher - Dispatch Southbridge function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
AGESA_STATUS  AmdSbDispatcher (IN void *pConfig);

#endif
