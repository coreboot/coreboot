/**
 * @file
 *
 * AMD AGESA CPU C6 Functions declarations.
 *
 * Contains code that declares the AGESA CPU C6 related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_C6_STATE_H_
#define _CPU_C6_STATE_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
// Forward declaration.
typedef struct _C6_FAMILY_SERVICES C6_FAMILY_SERVICES;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */


// Defines for CST ACPI Objects
#define   CST_NAME__            '_'
#define   CST_NAME_C            'C'
#define   CST_NAME_S            'S'
#define   CST_NAME_T            'T'
#define   CST_LENGTH            (CST_BODY_SIZE - 1)
#define   CST_NUM_OF_ELEMENTS   0x02
#define   CST_COUNT             0x01
#define   CST_PKG_LENGTH        (CST_BODY_SIZE - 5) // CST_BODY_SIZE - PkgHeader - Count Buffer
#define   CST_PKG_ELEMENTS      0x04
#define   CST_SUBPKG_LENGTH     0x14
#define   CST_SUBPKG_ELEMENTS   0x0A
#define   CST_GDR_LENGTH        0x000C
#define   CST_C2_TYPE           0x02

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
  UINT8   BytePrefix;      ///< Byte Prefix Opcode
  UINT8   Type;            ///< Type
  UINT8   BytePrefix2;     ///< Byte Prefix Opcode
  UINT8   Latency;         ///< Latency
  UINT8   Power;           ///< Power
} CST_BODY_STRUCT;
#define CST_BODY_SIZE 33

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if C6 is supported.
 *
 * @param[in]    C6Services         C6 C-state services.
 * @param[in]    Socket             Zero-based socket number.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               C6 is supported.
 * @retval       FALSE              C6 is not supported.
 *
 */
typedef BOOLEAN F_C6_IS_SUPPORTED (
  IN       C6_FAMILY_SERVICES *C6Services,
  IN       UINT32             Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/// Reference to a Method.
typedef F_C6_IS_SUPPORTED *PF_C6_IS_SUPPORTED;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to enable C6.
 *
 * @param[in]    C6Services         C6 services.
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       Family specific error value.
 *
 */
typedef AGESA_STATUS F_C6_INIT (
  IN       C6_FAMILY_SERVICES *C6Services,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

typedef UINT32 F_C6_GET_CST_SIZE (
  IN      VOID
  );

typedef VOID F_C6_CREATE_CST (
  IN OUT   VOID              **PstateAcpiBufferPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_C6_INIT *PF_C6_INIT;
typedef F_C6_GET_CST_SIZE *PF_C6_GET_CST_SIZE;
typedef F_C6_CREATE_CST   *PF_C6_CREATE_CST;

/**
 * Provide the interface to the C6 Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _C6_FAMILY_SERVICES {
  UINT16          Revision;                                             ///< Interface version
  // Public Methods.
  PF_C6_IS_SUPPORTED IsC6Supported;                                     ///< Method: Family specific call to check if C6 is supported.
  PF_C6_INIT InitializeC6;                                              ///< Method: Family specific call to enable C6.
  PF_C6_GET_CST_SIZE  GetAcpiCstObj;                                    ///< Method: Family specific call to return the size of ACPI CST objects.
  PF_C6_CREATE_CST    CreateAcpiCstObj;                                 ///< Method: Family specific call to create ACPI CST object
};


/*----------------------------------------------------------------------------------------
 *                          F U N C T I O N S     P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

#endif  // _CPU_C6_STATE_H_
