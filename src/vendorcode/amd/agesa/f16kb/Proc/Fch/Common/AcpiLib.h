/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH ACPI lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
****************************************************************************
*/
#ifndef _FCH_ACPILIB_H_
#define _FCH_ACPILIB_H_
///
/// RSDP - ACPI 2.0 table RSDP
///
typedef struct _RSDP_HEADER {
  UINT64               Signature;                      ///< RSDP signature "RSD PTR"
  UINT8                CheckSum;                       ///< checksum of the first 20 bytes
  UINT8                OEMID[6];                       ///< OEM ID
  UINT8                Revision;                       ///< 0 for APCI 1.0, 2 for ACPI 2.0
  UINT32               RsdtAddress;                    ///< physical address of RSDT
  UINT32               Length;                         ///< total length of RSDP (including extended part)
  UINT64               XsdtAddress;                    ///< physical address of XSDT
  UINT8                ExtendedCheckSum;               ///< chechsum of whole table
  UINT8                Reserved[3];                    ///< Reserved
} RSDP_HEADER;

///
/// DESCRIPTION_HEADER - ACPI common table header
///
typedef struct _DESCRIPTION_HEADER {
  UINT32               Signature;                      ///< ACPI signature (4 ASCII characters)
  UINT32               Length;                         ///< Length of table, in bytes, including header
  UINT8                Revision;                       ///< ACPI Specification minor version #
  UINT8                CheckSum;                       ///< To make sum of entire table == 0
  UINT8                OemId[6];                       ///< OEM identification
  UINT8                OemTableId[8];                  ///< OEM table identification
  UINT32               OemRevision;                    ///< OEM revision number
  UINT32               CreatorId;                      ///< ASL compiler vendor ID
  UINT32               CreatorRevision;                ///< ASL compiler revision number
} DESCRIPTION_HEADER;

///
/// _AcpiRegWrite - ACPI MMIO register R/W structure
///
typedef struct _ACPI_REG_WRITE {
  UINT8        MmioBase;               /// MmioBase: Index of Fch block (For instance GPIO_BASE:0x01 SMI_BASE:0x02)
  UINT8        MmioReg;                /// MmioReg      : Register index
  UINT8        DataAndMask;            /// DataANDMask  : AND Register Data
  UINT8        DataOrMask;             /// DataOrMask   : Or Register Data
} ACPI_REG_WRITE;

VOID*     AcpiLocateTable (IN UINT32 Signature);
VOID      AcpiSetTableCheckSum (IN VOID *TablePtr);
UINT8     AcpiGetTableCheckSum (IN VOID *TablePtr);
UINT8     GetByteSum (IN VOID *DataPtr, IN UINT32 Length);

#endif
