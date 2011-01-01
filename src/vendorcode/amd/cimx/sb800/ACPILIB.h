/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */

/**
 * RSDP - ACPI 2.0 table RSDP
 */
typedef struct _RSDP
{
  UINT64  Signature;        /* RSDP signature "RSD PTR" */
  UINT8  Checksum;          /* checksum of the first 20 bytes */
  UINT8  OEMID[6];          /* OEM ID, "LXBIOS" */
  UINT8  Revision;          /* 0 for APCI 1.0, 2 for ACPI 2.0 */
  UINT32  RsdtAddress;      /* physical address of RSDT */
  UINT32  Length;           /* total length of RSDP (including extended part) */
  UINT64  XsdtAddress;      /* physical address of XSDT */
  UINT8  ExtendedChecksum;  /* chechsum of whole table */
  UINT8  Reserved[3];
} RSDP;


/**
 * DESCRIPTION_HEADER - ACPI common table header
 */
typedef struct _DESCRIPTION_HEADER
{
  UINT32  Signature;        /* ACPI signature (4 ASCII characters) */
  UINT32  Length;           /* Length of table, in bytes, including header */
  UINT8  Revision;          /* ACPI Specification minor version # */
  UINT8  Checksum;          /* To make sum of entire table == 0 */
  UINT8  OEMID[6];          /* OEM identification */
  UINT8  OEMTableID[8];     /* OEM table identification */
  UINT32  OEMRevision;      /* OEM revision number */
  UINT32  CreatorID;        /* ASL compiler vendor ID */
  UINT32  CreatorRevision;  /* ASL compiler revision number */
} DESCRIPTION_HEADER;

VOID* ACPI_LocateTable (IN UINT32 Signature);
VOID  ACPI_SetTableChecksum (IN VOID* TablePtr);
UINT8 ACPI_GetTableChecksum (IN VOID* TablePtr);
UINT8 GetByteSum (IN VOID* pData, IN UINT32 Length);
