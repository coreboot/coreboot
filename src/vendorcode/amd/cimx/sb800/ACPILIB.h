/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***************************************************************************
 *
 */

/**
 * RSDP - ACPI 2.0 table RSDP
 */
typedef struct _RSDP
{
  unsigned long long  Signature;        /* RSDP signature "RSD PTR" */
  unsigned char  Checksum;          /* checksum of the first 20 bytes */
  unsigned char  OEMID[6];          /* OEM ID, "LXBIOS" */
  unsigned char  Revision;          /* 0 for APCI 1.0, 2 for ACPI 2.0 */
  unsigned int  RsdtAddress;      /* physical address of RSDT */
  unsigned int  Length;           /* total length of RSDP (including extended part) */
  unsigned long long  XsdtAddress;      /* physical address of XSDT */
  unsigned char  ExtendedChecksum;  /* chechsum of whole table */
  unsigned char  Reserved[3];
} RSDP_HEADER;


/**
 * DESCRIPTION_HEADER - ACPI common table header
 */
typedef struct _DESCRIPTION_HEADER
{
  unsigned int  Signature;        /* ACPI signature (4 ASCII characters) */
  unsigned int  Length;           /* Length of table, in bytes, including header */
  unsigned char  Revision;          /* ACPI Specification minor version # */
  unsigned char  Checksum;          /* To make sum of entire table == 0 */
  unsigned char  OEMID[6];          /* OEM identification */
  unsigned char  OEMTableID[8];     /* OEM table identification */
  unsigned int  OEMRevision;      /* OEM revision number */
  unsigned int  CreatorID;        /* ASL compiler vendor ID */
  unsigned int  CreatorRevision;  /* ASL compiler revision number */
} DESCRIPTION_HEADER;

void* ACPI_LocateTable (IN unsigned int Signature);
void  ACPI_SetTableChecksum (IN void* TablePtr);
unsigned char ACPI_GetTableChecksum (IN void* TablePtr);
unsigned char GetByteSum (IN void* pData, IN unsigned int Length);
