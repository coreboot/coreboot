/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU IO Cstate feature support code.
 *
 * Contains code that declares the AGESA CPU IO Cstate related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 36567 $   @e \$Date: 2010-08-21 02:35:15 +0800 (Sat, 21 Aug 2010) $
 */
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

#ifndef _CPU_IO_CSTATE_H_
#define _CPU_IO_CSTATE_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (IO_CSTATE_FAMILY_SERVICES);

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
// Defines for ACPI C-State Objects
#define   CST_NAME__            '_'
#define   CST_NAME_C            'C'
#define   CST_NAME_S            'S'
#define   CST_NAME_T            'T'
#define   CST_LENGTH            (CST_BODY_SIZE - 1)
#define   CST_NUM_OF_ELEMENTS   0x02
#define   CST_COUNT             0x01
#define   CST_PKG_LENGTH        (CST_BODY_SIZE - 6) // CST_BODY_SIZE - PkgHeader - Count Buffer
#define   CST_PKG_ELEMENTS      0x04
#define   CST_SUBPKG_LENGTH     0x14
#define   CST_SUBPKG_ELEMENTS   0x0A
#define   CST_GDR_LENGTH        0x000C
#define   CST_C1_TYPE           0x01
#define   CST_C2_TYPE           0x02

#define   CSD_NAME_D            'D'
#define   CSD_COORD_TYPE_HW_ALL 0xFE

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/* AML code definition */

/// CST Header
typedef struct _CST_HEADER_STRUCT {
  UINT8   NameOpcode;      ///< Name Opcode
  UINT8   CstName_a__;     ///< String "_"
  UINT8   CstName_a_C;     ///< String "C"
  UINT8   CstName_a_S;     ///< String "S"
  UINT8   CstName_a_T;     ///< String "T"
} CST_HEADER_STRUCT;
#define CST_HEADER_SIZE 5

/// CST Body
typedef struct _CST_BODY_STRUCT {
  UINT8   PkgOpcode;       ///< Package Opcode
  UINT8   PkgLength;       ///< Package Length
  UINT8   PkgElements;     ///< Number of Elements
  UINT8   BytePrefix;      ///< Byte Prefix Opcode
  UINT8   Count;           ///< Number of Cstate info packages
  UINT8   PkgOpcode2;      ///< Package Opcode
  UINT8   PkgLength2;      ///< Package Length
  UINT8   PkgElements2;    ///< Number of Elements
  UINT8   BufferOpcode;    ///< Buffer Opcode
  UINT8   BufferLength;    ///< Buffer Length
  UINT8   BufferElements;  ///< Number of Elements
  UINT8   BufferOpcode2;   ///< Buffer Opcode
  UINT8   GdrOpcode;       ///< Generic Register Descriptor Opcode
  UINT16  GdrLength;       ///< Descriptor Length
  UINT8   AddrSpaceId;     ///< Address Space ID
  UINT8   RegBitWidth;     ///< Register Bit Width
  UINT8   RegBitOffset;    ///< Register Bit Offset
  UINT8   AddressSize;     ///< Address Size
  UINT64  RegisterAddr;    ///< Register Address
  UINT16  EndTag;          ///< End Tag Descriptor
  UINT8   BytePrefix2;     ///< Byte Prefix Opcode
  UINT8   Type;            ///< Type
  UINT8   WordPrefix;      ///< Word Prefix Opcode
  UINT16  Latency;         ///< Latency
  UINT8   DWordPrefix;     ///< Dword Prefix Opcode
  UINT32  Power;           ///< Power
} CST_BODY_STRUCT;
#define CST_BODY_SIZE 39

/// CSD Header
typedef struct _CSD_HEADER_STRUCT {
  UINT8   NameOpcode;     ///< Name Opcode
  UINT8   CsdName_a__;    ///< String "_"
  UINT8   CsdName_a_C;    ///< String "C"
  UINT8   CsdName_a_S;    ///< String "S"
  UINT8   CsdName_a_D;    ///< String "D"
} CSD_HEADER_STRUCT;
#define CSD_HEADER_SIZE 5

/// CSD Body
typedef struct _CSD_BODY_STRUCT {
  UINT8   PkgOpcode;      ///< Package Opcode
  UINT8   PkgLength;      ///< Package Length
  UINT8   PkgElements;    ///< Number of Elements
  UINT8   PkgOpcode2;     ///< Package Opcode
  UINT8   PkgLength2;     ///< Package Length
  UINT8   PkgElements2;   ///< Number of Elements
  UINT8   BytePrefix;     ///< Byte Prefix Opcode
  UINT8   NumEntries;     ///< Number of Entries
  UINT8   BytePrefix2;    ///< Byte Prefix Opcode
  UINT8   Revision;       ///< Revision
  UINT8   DWordPrefix;    ///< DWord Prefix Opcode
  UINT32  Domain;         ///< Dependency Domain Number
  UINT8   DWordPrefix2;   ///< DWord Prefix Opcode
  UINT32  CoordType;      ///< Coordination Type
  UINT8   DWordPrefix3;   ///< Dword Prefix Opcode
  UINT32  NumProcessors;  ///< Number of Processors in the Domain
  UINT8   DWordPrefix4;   ///< Dword Prefix Opcode
  UINT32  Index;          ///< Index of C-State entry for which dependency applies
} CSD_BODY_STRUCT;
#define CSD_BODY_SIZE 30

/// input for create _CST
typedef struct _ACPI_CST_CREATE_INPUT {
  IO_CSTATE_FAMILY_SERVICES *IoCstateServices;    ///< Family service of IoCstate
  UINT8                     LocalApicId;    ///< Local Apic for create _CST
  VOID                      **PstateAcpiBufferPtr;  ///< buffer for fill _CST
} ACPI_CST_CREATE_INPUT ;

/// input for get _CST
typedef struct _ACPI_CST_GET_INPUT {
  IO_CSTATE_FAMILY_SERVICES *IoCstateServices;    ///< Family service of IoCstate
  PLATFORM_CONFIGURATION    *PlatformConfig;  ///< platform config
  UINT32                    *CStateAcpiObjSizePtr;  ///< Point to size of _CST
} ACPI_CST_GET_INPUT ;


/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if IO Cstate is supported.
 *
 * @param[in]    IoCstateServices   IO Cstate services.
 * @param[in]    Socket             Zero-based socket number.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               IO Cstate is supported.
 * @retval       FALSE              IO Cstate is not supported.
 *
 */
typedef BOOLEAN F_IO_CSTATE_IS_SUPPORTED (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       UINT32                    Socket,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to enable IO Cstate.
 *
 * @param[in]    IoCstateServices   IO Cstate services.
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       Family specific error value.
 *
 */
typedef AGESA_STATUS F_IO_CSTATE_INIT (
  IN       IO_CSTATE_FAMILY_SERVICES      *IoCstateServices,
  IN       UINT64                  EntryPoint,
  IN       PLATFORM_CONFIGURATION  *PlatformConfig,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to return the size of ACPI C-State Objects
 *
 * @param[in]    IoCstateServices   IO Cstate services.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       Size of ACPI C-State Objects
 *
 */
typedef UINT32 F_IO_CSTATE_GET_CST_SIZE (
  IN       IO_CSTATE_FAMILY_SERVICES   *IoCstateServices,
  IN       PLATFORM_CONFIGURATION      *PlatformConfig,
  IN       AMD_CONFIG_PARAMS           *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to create ACPI C-State Objects
 *
 * @param[in]      IoCstateServices      IO Cstate services.
 * @param[in]      LocalApicId           Local Apic Id
 * @param[in, out] PstateAcpiBufferPtr   Pointer to Pstate data buffer
 * @param[in]      StdHeader             Config Handle for library, services.
 *
 */
typedef VOID F_IO_CSTATE_CREATE_CST (
  IN       IO_CSTATE_FAMILY_SERVICES   *IoCstateServices,
  IN       UINT8                       LocalApicId,
  IN OUT   VOID                        **PstateAcpiBufferPtr,
  IN       AMD_CONFIG_PARAMS           *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check whether CSD object should be created.
 *
 * @param[in]      IoCstateServices      IO Cstate services.
 * @param[in]      StdHeader             Config Handle for library, services.
 *
 * @retval         TRUE                  CSD Object should be created.
 * @retval         FALSE                 CSD Object should not be created.
 *
 */
typedef BOOLEAN F_IO_CSTATE_IS_CSD_GENERATED (
  IN       IO_CSTATE_FAMILY_SERVICES   *IoCstateServices,
  IN       AMD_CONFIG_PARAMS           *StdHeader
  );

/// Reference to a Method
typedef F_IO_CSTATE_IS_SUPPORTED     *PF_IO_CSTATE_IS_SUPPORTED;
typedef F_IO_CSTATE_INIT             *PF_IO_CSTATE_INIT;
typedef F_IO_CSTATE_GET_CST_SIZE     *PF_IO_CSTATE_GET_CST_SIZE;
typedef F_IO_CSTATE_CREATE_CST       *PF_IO_CSTATE_CREATE_CST;
typedef F_IO_CSTATE_IS_CSD_GENERATED *PF_IO_CSTATE_IS_CSD_GENERATED;

/**
 * Provide the interface to the IO Cstate Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _IO_CSTATE_FAMILY_SERVICES {
  UINT16          Revision;                                  ///< Interface version
  // Public Methods.
  PF_IO_CSTATE_IS_SUPPORTED     IsIoCstateSupported;         ///< Method: Family specific call to check if IO Cstate is supported.
  PF_IO_CSTATE_INIT             InitializeIoCstate;          ///< Method: Family specific call to enable IO Cstate
  PF_IO_CSTATE_GET_CST_SIZE     GetAcpiCstObj;               ///< Method: Family specific call to return the size of ACPI CST objects.
  PF_IO_CSTATE_CREATE_CST       CreateAcpiCstObj;            ///< Method: Family specific call to create ACPI CST object
  PF_IO_CSTATE_IS_CSD_GENERATED IsCsdObjGenerated;           ///< Method: Family specific call to check whether CSD Object should be created.
};

#endif  // _CPU_IO_CSTATE_H_
