/**
 * @file
 *
 * ACPI common library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      Common Library
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/

#ifndef _AMDACPILIB_H_
#define _AMDACPILIB_H_

#ifndef   ASSERT
  #define   ASSERT
#endif

#pragma pack(push, 1)

///Header for RSDP table
typedef struct _RSDP {
  UINT64    Signature;                    ///< signature
  UINT8     Checksum;                     ///< 8 bit checksum
  UINT8     OEMID[6];                     ///< OEM identifier
  UINT8     Revision;                     ///< revision
  UINT32    RsdtAddress;                  ///< pointer to rsdt
  UINT32    Length;                       ///< length of RSDP
  UINT64    XsdtAddress;                  ///< pointer to xsdt
  UINT8     ExtendedChecksum;             ///< checksum of xsdt
  UINT8     Reserved[3];                  ///< reserved
} RSDP;

///Header for general ACPI table
typedef struct _DESCRIPTION_HEADER {
  UINT32    Signature;                    ///< signature
  UINT32    Length;                       ///< length
  UINT8     Revision;                     ///< revision
  UINT8     Checksum;                     ///< 8 bit checksum
  UINT8     OEMID[6];                     ///< OEM identifier
  UINT8     OEMTableID[8];                ///< OEM table identifier
  UINT32    OEMRevision;                  ///< OEM revision
  UINT32    CreatorID;                    ///< table creator identifier
  UINT32    CreatorRevision;              ///< table revision
} DESCRIPTION_HEADER;

///IO APIC struct in MADT table
typedef struct  {
  UINT8     Type;                         ///< type
  UINT8     Length;                       ///< length
  UINT8     ApicId;                       ///< Apic Id
  UINT8     Reserved;                     ///< reserved
  UINT32    IoApicBase;                   ///< IO APIC base address
  UINT32    GlobalInterruptBase;          ///< Global Interrupt Base
} MADT_IO_APIC_STRUCT;

///Local APIC struct in MADT table
typedef struct  {
  UINT8     Type;                         ///< type
  UINT8     Length;                       ///< length
  UINT8     AcpiProcessorId;              ///< ACPI Processor ID
  UINT8     ApicId;                       ///< Apic Id
  UINT32    Flags;                        ///< Flags
} MADT_LOCAL_APIC_STRUCT;

///Local SAPIC struct in MADT table
typedef struct  {
  UINT8     Type;                         ///< type
  UINT8     Length;                       ///< length
  UINT8     ApicId;                       ///< Apic Id
  UINT8     Reserved;                     ///< reserved
  UINT32    GlobalInterruptBase;          ///< Global Interrupt Base
  UINT64    IoApicBase;                   ///< IO SAPIC base address
} MADT_IO_SAPIC_STRUCT;

#pragma pack(pop)

AGESA_STATUS
LibAmdSetAcpiTable (
  IN      VOID      *TablePtr,
  IN      BOOLEAN   Checksum,
  IN      UINTN     *TableHandle
  );

AGESA_STATUS
LibAmdGetAcpiTable (
  IN      UINT32  Signature,
  IN      VOID    **TablePtr,
  IN      UINTN   *TableHandle
  );


VOID
LibAmdUpdateAcpiTableChecksum (
  IN      VOID* TablePtr
);

UINT8
LibAmdGetAcpiTableChecksum (
  IN      VOID* TablePtr
);


#endif