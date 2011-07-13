/**
 * @file
 *
 * AMD CPU Reset API, and related functions and structures.
 *
 * Contains code that initialized the CPU after early reset.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
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

#ifndef _CPU_EARLY_INIT_H_
#define _CPU_EARLY_INIT_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */
typedef struct _CPU_CORE_LEVELING_FAMILY_SERVICES CPU_CORE_LEVELING_FAMILY_SERVICES;

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
//----------------------------------------------------------------------------
//                         CPU BRAND ID TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
#define CPU_BRAND_ID_LENGTH     48            // Total number of characters supported
#define LOW_NODE_DEVICEID       24
#define NB_CAPABILITIES         0xE8          //Function 3 Registers
//----------------------------------------------------------------------------
//                         CPU MICROCODE PATCH TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/* All lengths are in bytes */
#define MICROCODE_TRIADE_SIZE           28
#define MICROCODE_HEADER_LENGTH         64

/* Offsets in UCODE PATCH Header */
/* Note: Header is 64 bytes      */
#define DATE_CODE_OFFSET                0   // 4 bytes
#define PATCH_ID                        4   // 4 bytes
#define MICROCODE_PATH_DATA_ID          8   // 2 bytes
#define MICROCODE_PATCH_DATA_LENGTH     10  // 1 byte
#define MICROCODE_PATCH_DATA_CHECKSUM   12  // 4 bytes
#define CHIPSET_1_DEVICE_ID             16  // 4 bytes
#define CHIPSET_2_DEVICE_ID             20  // 4 bytes
#define PROCESSOR_REV_ID                24  // 2 bytes
#define CHIPSET_1_REV_ID                26  // 1 byte
#define CHIPSET_2_REV_ID                27  // 1 byte

#define MICROCODE_PATCH_SIZE    2048
/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
//----------------------------------------------------------------------------
//                         CPU BRAND ID TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// A structure representing BrandId[15:0] from
/// CPUID Fn8000_0001_EBX
typedef struct {
  UINT8           String1:4;   ///< An index to a string value used to create the name string
  UINT8           String2:4;   ///< An index to a string value used to create the name string
  UINT8           Page:1;      ///< An index to the appropriate page for the String1, String2, and Model values
  UINT8           Model:7;     ///< A field used to create the model number in the name string
  UINT8           Socket:4;    ///< Specifies the package type
  UINT8           Cores:4;     ///< Identifies how many physical cores are present
} AMD_CPU_BRAND_DATA;

/// A structure containing string1 and string2 values
/// as well as information pertaining to their usage
typedef struct {
  IN       UINT8  Cores;              ///< Appropriate number of physical cores
  IN       UINT8  Page;               ///< This string's page number
  IN       UINT8  Index;              ///< String index
  IN       UINT8  Socket;             ///< Package type information
  IN       CONST CHAR8 *Stringstart;  ///< The literal string
  IN       UINT8  Stringlength;       ///< Number of characters in the string
} AMD_CPU_BRAND;

/// An entire CPU brand table.
typedef struct {
  UINT8 NumberOfEntries;        ///< The number of entries in the table.
  CONST AMD_CPU_BRAND  *Table;  ///< The table entries.
} CPU_BRAND_TABLE;

//----------------------------------------------------------------------------
//                         CPU MICROCODE PATCH TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// Microcode patch field definitions
typedef struct {
  UINT32   DateCode;                   ///< Date of patch creation
  UINT32   PatchID;                    ///< Patch level
  UINT16   MicrocodePatchDataID;       ///< Internal use only
  UINT8    MicrocodePatchDataLength;   ///< Internal use only
  UINT8    InitializationFlag;         ///< Internal use only
  UINT32   MicrocodePatchDataChecksum; ///< Doubleword sum of data block
  UINT32   Chipset1DeviceID;           ///< Device ID of 1st HT device to match
  UINT32   Chipset2DeviceID;           ///< Device ID of 2nd HT device to match
  UINT16   ProcessorRevisionID;        ///< Equivalent ID
  UINT8    Chipset1RevisionID;         ///< Revision level of 1st HT device to match
  UINT8    Chipset2RevisionID;         ///< Revision level of 2nd HT device to match
  UINT8    BiosApiRevision;            ///< BIOS INT 15 API revision required
  UINT8    Reserved1[3];               ///< Reserved
  UINT32   MatchRegister0;             ///< Internal use only
  UINT32   MatchRegister1;             ///< Internal use only
  UINT32   MatchRegister2;             ///< Internal use only
  UINT32   MatchRegister3;             ///< Internal use only
  UINT32   MatchRegister4;             ///< Internal use only
  UINT32   MatchRegister5;             ///< Internal use only
  UINT32   MatchRegister6;             ///< Internal use only
  UINT32   MatchRegister7;             ///< Internal use only
  UINT8    PatchDataBlock[896];        ///< Raw patch data
  UINT8    Reserved2[896];             ///< Reserved
  UINT8    X86CodePresent;             ///< Boolean to determine if executable code exists
  UINT8    X86CodeEntry[191];          ///< Code to execute if X86CodePresent != 0
} MICROCODE_PATCH;

/// Two kilobyte array containing the raw
/// microcode patch binary data
typedef struct {
  IN       UINT8   MicrocodePatches[MICROCODE_PATCH_SIZE];  ///< 2k UINT8 elements
} MICROCODE_PATCHES;

/**
 *  Set down core register
 *
 *  @CpuServiceInstances
 *
 * @param[in]      FamilySpecificServices  The current Family Specific Services.
 * @param[in]      Socket        Socket ID.
 * @param[in]      Module        Module ID in socket.
 * @param[in]      LeveledCores  Number of core.
 * @param[in]      StdHeader     Header for library and services.
 *
 */
typedef VOID (F_CPU_SET_DOWN_CORE_REGISTER) (
  IN       CPU_CORE_LEVELING_FAMILY_SERVICES  *FamilySpecificServices,
  IN       UINT32 *Socket,
  IN       UINT32 *Module,
  IN       UINT32 *LeveledCores,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a method
typedef F_CPU_SET_DOWN_CORE_REGISTER *PF_CPU_SET_DOWN_CORE_REGISTER;

/**
 * Provide the interface to the Core Leveling Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _CPU_CORE_LEVELING_FAMILY_SERVICES {
  UINT16          Revision;                             ///< Interface version
  // Public Methods.
  PF_CPU_SET_DOWN_CORE_REGISTER SetDownCoreRegister;    ///< Method: Set down core register.
};

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

//                   These are   P U B L I C   functions, used by IBVs
AGESA_STATUS
AmdCpuEarly (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig
  );

//                   These are   P U B L I C   functions, used by AGESA
VOID
SetBrandIdRegisters (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
PmInitializationAtEarly (
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

BOOLEAN
LoadMicrocodePatch (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );
#endif  // _CPU_EARLY_INIT_H_

