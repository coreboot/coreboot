/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Late Init API functions Prototypes.
 *
 * Contains code for doing any late CPU initialization
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 38082 $   @e \$Date: 2010-09-18 01:51:40 +0800 (Sat, 18 Sep 2010) $
 *
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

#ifndef _CPU_LATE_INIT_H_
#define _CPU_LATE_INIT_H_

#include "Filecode.h"
/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
//----------------------------------------------------------------------------
//                         DMI DEFINITIONS AND MACROS
//
//----------------------------------------------------------------------------
#define AP_LATE_TASK_GET_TYPE4_TYPE7 (PROC_CPU_FEATURE_CPUDMI_FILECODE)
// SMBIOS constant definition
#define CENTRAL_PROCESSOR       0x03
#define EXTERNAL_CLOCK_DFLT     200
#define EXTERNAL_CLOCK_100MHZ   100
#define UNKNOWN                 0x02
#define P_CHARACTERISTICS       0x4
#define CACHE_CFG_L1            0x180
#define CACHE_CFG_L2            0x181
#define CACHE_CFG_L3            0x182
#define SRAM_TYPE               0x10
#define ERR_CORRECT_TYPE        0x06
#define CACHE_TYPE              0x05
#define ASSOCIATIVE_2_WAY       0x04
#define ASSOCIATIVE_16_WAY      0x08
#define ASSOCIATIVE_OTHER       0x01
#define SOCKET_POPULATED        0x40
#define CPU_STATUS_UNKNOWN      0x00
#define CPU_STATUS_ENABLED      0x01

// Processor Upgrade Definition
#define P_UPGRADE_UNKNOWN       0x02
#define P_UPGRADE_NONE          0x06
#define P_UPGRADE_S1GX          0x16
#define P_UPGRADE_AM2           0x17
#define P_UPGRADE_F1207         0x18
#define P_UPGRADE_G34           0x1A
#define P_UPGRADE_AM3           0x1B
#define P_UPGRADE_C32           0x1C

//----------------------------------------------------------------------------
//                         SRAT DEFINITIONS AND MACROS
//
//----------------------------------------------------------------------------
#define NorthbridgeCapabilities 0xE8
#define DRAMBase0 0x40
#define MMIOBase0 0x80
#define TOP_MEM 0xC001001A
#define LOW_NODE_DEVICEID 24
#define LOW_APICID 0


// Miscellaneous AMD related values
#define MAX_NUMBER_NODES  8
#define FOURGB            0x01000000    // Use format as in MCT DRAM range registers [39:24]

// Flags
#define ENABLED         1   // Bit 0
#define DISABLED        0   // Bit 0
#define HOTPLUGGABLE    2   // Bit 1

// Affinity Entry Structures
#define AE_APIC     0
#define AE_MEMORY   1


// Memory Types
#define TYPE_MEMORY   1
#define TYPE_RESERVED 2
#define TYPE_ACPI     3
#define TYPE_NVS      4

//----------------------------------------------------------------------------
//                         SLIT DEFINITIONS AND MACROS
//
//----------------------------------------------------------------------------
#define PROBE_FILTER_CTRL_REG             0x1D4
#define AMD_ACPI_SLIT_SOCKET_NUM_LENGTH   8

//----------------------------------------------------------------------------
//                         P-STATE DEFINITIONS AND MACROS
//
//----------------------------------------------------------------------------
//-------------------------------------
//          ERROR Codes
//-------------------------------------
#define     NO_ERROR                    0x0
#define     USER_DISABLE_ERROR          0x01            // User disabled SSDT generation
#define     CORES_MISSMATCH_PSS_ERROR   0x02            // No PSS match
#define     PNOW_SUPPORT_ERROR          0x04            // One of the Cores do not support PNOW!
#define     PWR_FREQ_MATCH_ERROR        0x08            // FREQ and PWR mismatch
#define     NO_PSS_SIZE_ERROR           0x10            // Error in PSS Size
#define     INVALID_PSTATE_ERROR        0x20            // Invalid Max or only 1 P-State available
#define     NO_PSS_ENTRY                0x0FFFF
#define     INVALID_FREQ                0x0FFFFFFFF

//-------------------------
// Default definitions
// AMD BKDG default values
//-------------------------
#define DEFAULT_ISOCH_RELIEF_TIME       IRT_80uS
#define DEFAULT_RAMP_VOLTAGE_OFFSET     RVO_50mV
#define DEFAULT_MAX_VOLTAGE_STEP        MVS_25mV
#define DEFAULT_PERF_PRESENT_CAP        0           // default for Desktop
#define DEFAULT_VOLTAGE_STABLE_TIME     (100 / 20)  // 100uS
#define DEFAULT_PLL_LOCK_TIME           2           // 2uS
#define DEFAULT_TRANSITION_LATENCY      100         // 100uS
#define DEFAULT_BUS_MASTER_LATENCY      9           // 9uS
#define DEFAULT_CPU_SCOPE_NUMBER        "0UPC"

// Defines for Common ACPI
// -----------------------------
#define     SCOPE_OPCODE            0x10
#define     NAME_OPCODE             0x08
#define     PACKAGE_OPCODE          0x12
#define     BUFFER_OPCODE           0x11
#define     BYTE_PREFIX_OPCODE      0x0A
#define     WORD_PREFIX_OPCODE      0x0B
#define     DWORD_PREFIX_OPCODE     0x0C
#define     ACPI_BUFFER             0x080A0B11

// Generic Register Descriptor (GDR) Fields
#define     GDR_ASI_SYSTEM_IO       0x01    // Address Space ID
#define     GDR_ASZ_BYTE_ACCESS     0x01    // Address Size

// Defines for ACPI Scope Table
// ----------------------------
#define     SCOPE_LENGTH    (SCOPE_STRUCT_SIZE + \
                            PCT_STRUCT_SIZE + \
                            PSS_HEADER_STRUCT_SIZE + \
                            PSS_BODY_STRUCT_SIZE + \
                            PPC_HEADER_BODY_STRUCT_SIZE)
#define     SCOPE_VALUE1    0x5C
#define     SCOPE_VALUE2    0x2E
#define     SCOPE_NAME__    '_'
#define     SCOPE_NAME_P    'P'
#define     SCOPE_NAME_R    'R'
#define     SCOPE_NAME_S    'S'
#define     SCOPE_NAME_B    'B'
#define     SCOPE_NAME_C    'C'
#define     SCOPE_NAME_U    'U'
#define     SCOPE_NAME_0    '0'
#define     SCOPE_NAME_1    '1'
#define     SCOPE_NAME_2    '2'
#define     SCOPE_NAME_3    '3'
#define     SCOPE_NAME_A    'A'

#ifdef OEM_SCOPE_NAME
  #if (OEM_SCOPE_NAME > 'Z') || (OEM_SCOPE_NAME < 'A')
    #error "OEM_SCOPE_NAME: it should be only one char long AND a valid letter (A~Z)"
  #endif
  #define     SCOPE_NAME_VALUE    OEM_SCOPE_NAME
#else
  #define     SCOPE_NAME_VALUE    SCOPE_NAME_C
#endif  // OEM_SCOPE_NAME

#ifdef OEM_SCOPE_NAME1
  #if (!(((OEM_SCOPE_NAME1 >= 'A') && (OEM_SCOPE_NAME1 <= 'Z')) || \
         ((OEM_SCOPE_NAME1 >= '0') && (OEM_SCOPE_NAME1 <= '9')) || \
         (OEM_SCOPE_NAME1 == '_')))
    #error "OEM_SCOPE_NAME1: it should be only one char long AND a valid letter (0~9, A~F)"
  #endif
  #define     SCOPE_NAME_VALUE1   OEM_SCOPE_NAME1
#else
  #define     SCOPE_NAME_VALUE1   SCOPE_NAME_0
#endif  // OEM_SCOPE_NAME

// Defines for PCT Control and Status Table
// ----------------------------------------
#define     PCT_NAME__                  '_'
#define     PCT_NAME_P                  'P'
#define     PCT_NAME_C                  'C'
#define     PCT_NAME_T                  'T'
#define     PCT_VALUE1                  0x11022C12
#define     PCT_VALUE2                  0x0A14
#define     PCT_VALUE3                  0x11
#define     GENERIC_REG_DESCRIPTION     0x82
#define     PCT_LENGTH                  0x0C
#define     PCT_ADDRESS_SPACE_ID        0x7F
#define     PCT_REGISTER_BIT_WIDTH      0x40
#define     PCT_REGISTER_BIT_OFFSET     0x00
#define     PCT_RESERVED                0x00
#define     PCT_CONTROL_REG_LO          0xC0010062
#define     PCT_CONTROL_REG_HI          0x00
#define     PCT_VALUE4                  0x14110079
#define     PCT_VALUE5                  0x110A
#define     PCT_STATUS_REG_LO           0x00
#define     PCT_STATUS_REG_HI           0x00
#define     PCT_VALUE6                  0x0079


// Defines for PSS Header Table
// ----------------------------
#define     PSS_NAME__          '_'
#define     PSS_NAME_X          'X'
#define     PSS_NAME_P          'P'
#define     PSS_NAME_S          'S'
#define     PSS_LENGTH          (sizeof pssBodyStruct + 3)
#define     NUM_OF_ITEMS_IN_PSS 0x00


// Defines for PSS Header Table
// ----------------------------
#define     PSS_PKG_LENGTH              0x20  // PSS_BODY_STRUCT_SIZE - 1
#define     PSS_NUM_OF_ELEMENTS         0x06
#define     PSS_FREQUENCY               0x00
#define     PSS_POWER                   0x00
#define     PSS_TRANSITION_LATENCY      DEFAULT_TRANSITION_LATENCY
#define     PSS_BUS_MASTER_LATENCY      DEFAULT_BUS_MASTER_LATENCY
#define     PSS_CONTROL                 ((DEFAULT_ISOCH_RELIEF_TIME << 30) + \
                                         (DEFAULT_RAMP_VOLTAGE_OFFSET << 28) + \
                                         (DEFAULT_EXT_TYPE << 27) + \
                                         (DEFAULT_PLL_LOCK_TIME << 20) + \
                                         (DEFAULT_MAX_VOLTAGE_STEP << 18) + \
                                         (DEFAULT_VOLTAGE_STABLE_TIME << 11) + \
                                         (PSS_VID << 6) + PSS_FID)
#define     PSS_STATUS                  (DEFAULT_EXTENDED_TYPE << 11) + (PSS_VID << 6) + (PSS_FID)

// Defines for XPSS Header Table
// ----------------------------
#define     XPSS_PKG_LENGTH             0x47  // XPSS_BODY_STRUCT_SIZE - 1
#define     XPSS_NUM_OF_ELEMENTS        0x08
#define     XPSS_ACPI_BUFFER            0x080A0B11


// Defines for PPC Header Table
// ----------------------------
#define     PPC_NAME__  '_'
#define     PPC_NAME_P  'P'
#define     PPC_NAME_C  'C'
#define     PPC_VALUE1  0x0A;

// Defines for PSD Header Table
// ----------------------------
#define     PSD_NAME__          '_'
#define     PSD_NAME_P          'P'
#define     PSD_NAME_S          'S'
#define     PSD_NAME_D          'D'
#define     PSD_HEADER_LENGTH   (PSD_BODY_STRUCT_SIZE + 2)
#define     PSD_VALUE1          0x01


// Defines for PSD Header Table
// ----------------------------
#define     PSD_PKG_LENGTH          (PSD_BODY_STRUCT_SIZE - 1)
#define     NUM_OF_ENTRIES          0x05
#define     PSD_NUM_OF_ENTRIES      0x05
#define     PSD_REVISION            0x00
#define     PSD_DEPENDENCY_DOMAIN   0x00
#define     PSD_COORDINATION_TYPE_HW_ALL   0xFE
#define     PSD_COORDINATION_TYPE_SW_ANY   0xFD
#define     PSD_COORDINATION_TYPE_SW_ALL   0xFC
#define     PSD_NUM_OF_PROCESSORS   0x01
#define     PSD_CORE_NUM_PER_COMPUTE_UNIT    0x02
#define     PSD_DOMAIN_COMPUTE_UNIT_MASK   0x7F


#define     CUSTOM_PSTATE_FLAG      0x55
#define     PSTATE_FLAG_1           0x55
#define     TARGET_PSTATE_FLAG      0xAA
#define     PSTATE_FLAG_2           0xAA

/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */
//----------------------------------------------------------------------------
//                         ACPI P-States AML TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------

//--------------------------------------------
//              AML code definition
//                   (Scope)
//---------------------------------------------
/// SCOPE
typedef struct _SCOPE {
  UINT8   ScopeOpcode;                  ///< Opcode
  UINT16  ScopeLength;                  ///< Scope Length
  UINT8   ScopeValue1;                  ///< Value1
  UINT8   ScopeValue2;                  ///< Value2
  UINT8   ScopeNamePt1a__;              ///< Name Pointer
  UINT8   ScopeNamePt1a_P;              ///< Name Pointer
  UINT8   ScopeNamePt1a_R;              ///< Name Pointer
  UINT8   ScopeNamePt1b__;              ///< Name Pointer
  UINT8   ScopeNamePt2a_C;              ///< Name Pointer
  UINT8   ScopeNamePt2a_P;              ///< Name Pointer
  UINT8   ScopeNamePt2a_U;              ///< Name Pointer
  UINT8   ScopeNamePt2a_0;              ///< Name Pointer
} SCOPE;
#define     SCOPE_STRUCT_SIZE   13      // 13 Bytes

//--------------------------------------------
//              AML code definition
//             (PCT Header and Body)
//---------------------------------------------

///Performance Control Header
typedef struct _PCT_HEADER_BODY {
  UINT8   NameOpcode;                   ///< Opcode
  UINT8   PctName_a__;                  ///< String "_"
  UINT8   PctName_a_P;                  ///< String "P"
  UINT8   PctName_a_C;                  ///< String "C"
  UINT8   PctName_a_T;                  ///< String "T"
  UINT32  Value1;                       ///< Value1
  UINT16  Value2;                       ///< Value2
  UINT8   Value3;                       ///< Value3
  UINT8   GenericRegDescription1;       ///< Generic Reg Description
  UINT16  Length1;                      ///< Length1
  UINT8   AddressSpaceId1;              ///< PCT Address Space ID
  UINT8   RegisterBitWidth1;            ///< PCT Register Bit Width
  UINT8   RegisterBitOffset1;           ///< PCT Register Bit Offset
  UINT8   Reserved1;                    ///< Reserved
  UINT32  ControlRegAddressLo;          ///< Control Register Address Low
  UINT32  ControlRegAddressHi;          ///< Control Register Address High
  UINT32  Value4;                       ///< Value4
  UINT16  Value5;                       ///< Value 5
  UINT8   GenericRegDescription2;       ///< Generic Reg Description
  UINT16  Length2;                      ///< Length2
  UINT8   AddressSpaceId2;              ///< PCT Address Space ID
  UINT8   RegisterBitWidth2;            ///< PCT Register Bit Width
  UINT8   RegisterBitOffset2;           ///< PCT Register Bit Offset
  UINT8   Reserved2;                    ///< Reserved
  UINT32  StatusRegAddressLo;           ///< Control Register Address Low
  UINT32  StatusRegAddressHi;           ///< Control Register Address High
  UINT16  Value6;                       ///< Values
} PCT_HEADER_BODY;
#define     PCT_STRUCT_SIZE     50      // 50 Bytes


//--------------------------------------------
//              AML code definition
//                 (PSS Header)
//--------------------------------------------
///Performance Supported States Header
typedef struct  _PSS_HEADER {
  UINT8   NameOpcode;                   ///< Opcode
  UINT8   PssName_a__;                  ///< String "_"
  UINT8   PssName_a_P;                  ///< String "P"
  UINT8   PssName_a_S;                  ///< String "S"
  UINT8   PssName_b_S;                  ///< String "S"
  UINT8   PkgOpcode;                    ///< Package Opcode
  UINT16  PssLength;                    ///< PSS Length
  UINT8   NumOfItemsInPss;              ///< Number of Items in PSS
} PSS_HEADER;
#define     PSS_HEADER_STRUCT_SIZE  9   // 9 Bytes


//--------------------------------------------
//              AML code definition
//                 (PSS Body)
//--------------------------------------------
///Performance Supported States Body
typedef struct _PSS_BODY {
  UINT8   PkgOpcode;                    ///< Package Opcode
  UINT8   PkgLength;                    ///< Package Length
  UINT8   NumOfElements;                ///< Number of Elements
  UINT8   DwordPrefixOpcode1;           ///< Prefix Opcode1
  UINT32  Frequency;                    ///< Frequency
  UINT8   DwordPrefixOpcode2;           ///< Prefix Opcode2
  UINT32  Power;                        ///< Power
  UINT8   DwordPrefixOpcode3;           ///< Prefix Opcode3
  UINT32  TransitionLatency;            ///< Transition Latency
  UINT8   DwordPrefixOpcode4;           ///< Prefix Opcode4
  UINT32  BusMasterLatency;             ///< Bus Master Latency
  UINT8   DwordPrefixOpcode5;           ///< Prefix Opcode5
  UINT32  Control;                      ///< Control
  UINT8   DwordPrefixOpcode6;           ///< Prefix Opcode6
  UINT32  Status;                       ///< Status
} PSS_BODY;
#define     PSS_BODY_STRUCT_SIZE    33  // 33 Bytes


/*--------------------------------------------
 *              AML code definition
 *                 (XPSS Header)
 *--------------------------------------------
 */
/// Extended PSS Header
typedef struct  _XPSS_HEADER {
  UINT8   NameOpcode;                   ///< 08h
  UINT8   XpssName_a_X;                 ///< String "X"
  UINT8   XpssName_a_P;                 ///< String "P"
  UINT8   XpssName_a_S;                 ///< String "S"
  UINT8   XpssName_b_S;                 ///< String "S"
  UINT8   PkgOpcode;                    ///< 12h
  UINT16  XpssLength;                   ///< XPSS Length
  UINT8   NumOfItemsInXpss;             ///< Number of Items in XPSS
} XPSS_HEADER;
#define     XPSS_HEADER_STRUCT_SIZE  9  // 9 Bytes

/*--------------------------------------------
 *              AML code definition
 *                 (XPSS Body)
 *--------------------------------------------
 */
/// Extended PSS Body
typedef struct  _XPSS_BODY {
  UINT8   PkgOpcode;                    ///< 12h
  UINT8   PkgLength;                    ///< Package Length
  UINT8   XpssValueTbd;                 ///< XPSS Value
  UINT8   NumOfElements;                ///< Number of Elements
  UINT8   DwordPrefixOpcode1;           ///< Prefix Opcode1
  UINT32  Frequency;                    ///< Frequency
  UINT8   DwordPrefixOpcode2;           ///< Prefix Opcode2
  UINT32  Power;                        ///< Power
  UINT8   DwordPrefixOpcode3;           ///< Prefix Opcode3
  UINT32  TransitionLatency;            ///< Transition Latency
  UINT8   DwordPrefixOpcode4;           ///< Prefix Opcode4
  UINT32  BusMasterLatency;             ///< Bus Master Latency
  UINT32  ControlBuffer;                ///< Control Buffer
  UINT32  ControlLo;                    ///< Control Low
  UINT32  ControlHi;                    ///< Control High
  UINT32  StatusBuffer;                 ///< Status Buffer
  UINT32  StatusLo;                     ///< Status Low
  UINT32  StatusHi;                     ///< Status High
  UINT32  ControlMaskBuffer;            ///< Control Mask Buffer
  UINT32  ControlMaskLo;                ///< Control Mask Low
  UINT32  ControlMaskHi;                ///< Control Mask High
  UINT32  StatusMaskBuffer;             ///< Status Mask Buffer
  UINT32  StatusMaskLo;                 ///< Status Mask Low
  UINT32  StatusMaskHi;                 ///< Status Mask High
} XPSS_BODY;
#define     XPSS_BODY_STRUCT_SIZE    72 // 72 Bytes

/*--------------------------------------------
 *              AML code definition
 *             (PPC Header and Body)
 *--------------------------------------------
 */
/// Performance Present Capabilities Header
typedef struct _PPC_HEADER_BODY {
  UINT8   NameOpcode;                   ///< Name Opcode
  UINT8   PpcName_a__;                  ///< String "_"
  UINT8   PpcName_a_P;                  ///< String "P"
  UINT8   PpcName_b_P;                  ///< String "P"
  UINT8   PpcName_a_C;                  ///< String "C"
  UINT8   Value1;                       ///< Value
  UINT8   DefaultPerfPresentCap;        ///< Default Perf Present Cap
} PPC_HEADER_BODY;
#define     PPC_HEADER_BODY_STRUCT_SIZE     7 // 7 Bytes


/*--------------------------------------------
 *              AML code definition
 *                (PSD Header)
 *--------------------------------------------
 */
/// P-State Dependency Header
typedef struct  _PSD_HEADER {
  UINT8   NameOpcode;                   ///< Name Opcode
  UINT8   PsdName_a__;                  ///< String "_"
  UINT8   PsdName_a_P;                  ///< String "P"
  UINT8   PsdName_a_S;                  ///< String "S"
  UINT8   PsdName_a_D;                  ///< String "D"
  UINT8   PkgOpcode;                    ///< Package Opcode
  UINT8   PsdLength;                    ///< PSD Length
  UINT8   Value1;                       ///< Value
} PSD_HEADER;
#define     PSD_HEADER_STRUCT_SIZE  8   // 8 Bytes

/*--------------------------------------------
 *              AML code definition
 *                 (PSD Body)
 *--------------------------------------------
 */
/// P-State Dependency Body
typedef struct _PSD_BODY {
  UINT8   PkgOpcode;                    ///< Package Opcode
  UINT8   PkgLength;                    ///< Package Length
  UINT8   NumOfEntries;                 ///< Number of Entries
  UINT8   BytePrefixOpcode1;            ///< Prefix Opcode1 in Byte
  UINT8   PsdNumOfEntries;              ///< PSD Number of Entries
  UINT8   BytePrefixOpcode2;            ///< Prefix Opcode2 in Byte
  UINT8   PsdRevision;                  ///< PSD Revision
  UINT8   DwordPrefixOpcode1;           ///< Prefix Opcode1 in DWord
  UINT32  DependencyDomain;             ///< Dependency Domain
  UINT8   DwordPrefixOpcode2;           ///< Prefix Opcode2 in DWord
  UINT32  CoordinationType;             ///< (0xFC = SW_ALL, 0xFD = SW_ANY, 0xFE = HW_ALL)
  UINT8   DwordPrefixOpcode3;           ///< Prefix Opcode3 in DWord
  UINT32  NumOfProcessors;              ///< Number of Processors
} PSD_BODY;
#define     PSD_BODY_STRUCT_SIZE    22  // 22 Bytes

//----------------------------------------------------------------------------
//                         WHEA TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------

/// HEST MCE TABLE
typedef struct _AMD_HEST_MCE_TABLE {
  UINT16 TblLength;               ///< Length, in bytes, of entire AMD_HEST_MCE structure.
  UINT32 GlobCapInitDataLSD;      ///< Holds the value that the OS will program into
  UINT32 GlobCapInitDataMSD;      ///< the machine check global capability register(MCG_CAP).
  UINT32 GlobCtrlInitDataLSD;     ///< Holds the value that the OS will program into
  UINT32 GlobCtrlInitDataMSD;     ///< the machine check global control register(MCG_CTL).
  UINT8 NumHWBanks;               ///< The number of hardware error reporting banks.
  UINT8 Rsvd[7];                  ///< reserve 7 bytes as spec's required
} AMD_HEST_MCE_TABLE;

/// HEST CMC TABLE
typedef struct _AMD_HEST_CMC_TABLE {
  UINT16 TblLength;               ///< Length, in bytes, of entire AMD_HEST_CMC structure.
  UINT8 NumHWBanks;               ///< The number of hardware error reporting banks.
  UINT8 Rsvd[3];                  ///< reserve 3 bytes as spec's required
} AMD_HEST_CMC_TABLE;

/// HEST BANK
typedef struct _AMD_HEST_BANK {
  UINT8 BankNum;                  ///< Zero-based index identifies the machine check error bank.
  UINT8 ClrStatusOnInit;          ///< Indicates if the status information in this machine check bank
                                  ///< is to be cleared during system initialization.
  UINT8 StatusDataFormat;         ///< Indicates the format of the data in the status register
  UINT8 ConfWriteEn;              ///< This field indicates whether configuration parameters may be
                                  ///< modified by the OS. If the bit for the associated parameter is
                                  ///< set, the parameter is writable by the OS.
  UINT32 CtrlRegMSRAddr;          ///< Address of the hardware bank's control MSR. Ignored if zero.

  UINT32 CtrlInitDataLSD;         ///< This is the value the OS will program into the machine check
  UINT32 CtrlInitDataMSD;         ///< bank's control register
  UINT32 StatRegMSRAddr;          ///< Address of the hardware bank's MCi_STAT MSR. Ignored if zero.
  UINT32 AddrRegMSRAddr;          ///< Address of the hardware bank's MCi_ADDR MSR. Ignored if zero.
  UINT32 MiscRegMSRAddr;          ///< Address of the hardware bank's MCi_MISC MSR. Ignored if zero.
} AMD_HEST_BANK;

/// Initial data of AMD_HEST_BANK
typedef struct _AMD_HEST_BANK_INIT_DATA {
  UINT32 CtrlInitDataLSD;         ///< Initial data of CtrlInitDataLSD
  UINT32 CtrlInitDataMSD;         ///< Initial data of CtrlInitDataMSD
  UINT32 CtrlRegMSRAddr;          ///< Initial data of CtrlRegMSRAddr
  UINT32 StatRegMSRAddr;          ///< Initial data of StatRegMSRAddr
  UINT32 AddrRegMSRAddr;          ///< Initial data of AddrRegMSRAddr
  UINT32 MiscRegMSRAddr;          ///< Initial data of MiscRegMSRAddr
} AMD_HEST_BANK_INIT_DATA;

/// MSR179 Global Machine Check Capabilities data struct
typedef struct _MSR_MCG_CAP_STRUCT {
  UINT64 Count:8;                 ///< Indicates the number of
                                  ///< error-reporting banks visible to each core
  UINT64 McgCtlP:1;               ///< 1=The machine check control registers
  UINT64 Rsvd:55;                 ///< reserved
} MSR_MCG_CAP_STRUCT;

/// Initial data of WHEA
typedef struct _AMD_WHEA_INIT_DATA {
  UINT32 GlobCapInitDataLSD;      ///< Holds the value that the OS will program into the machine
  UINT32 GlobCapInitDataMSD;      ///< Check global capability register
  UINT32 GlobCtrlInitDataLSD;     ///< Holds the value that the OS will grogram into the machine
  UINT32 GlobCtrlInitDataMSD;     ///< Check global control register
  UINT8  ClrStatusOnInit;         ///< Indicates if the status information in this machine check
                                  ///< bank is to be cleared during system initialization
  UINT8  StatusDataFormat;        ///< Indicates the format of the data in the status register
  UINT8  ConfWriteEn;             ///< This field indicates whether configuration parameters may be
                                  ///< modified by the OS. If the bit for the associated parameter is
                                  ///< set, the parameter is writable by the OS.
  UINT8  HestBankNum;             ///< Number of HEST Bank
  AMD_HEST_BANK_INIT_DATA *HestBankInitData;  ///< Pointer to Initial data of HEST Bank
} AMD_WHEA_INIT_DATA;

//----------------------------------------------------------------------------
//                         DMI TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// DMI brand information
typedef struct {
  UINT16 String1:4;                 ///< String1
  UINT16 String2:4;                 ///< String2
  UINT16 Model:7;                   ///< Model
  UINT16 Pg:1;                      ///< Page
} BRAND_ID;

/// DMI processor information
typedef struct {
  UINT8  ExtendedFamily;            ///< Extended Family
  UINT8  ExtendedModel;             ///< Extended Model
  UINT8  BaseFamily;                ///< Base Family
  UINT8  BaseModel;                 ///< Base Model
  UINT8  Stepping;                  ///< Stepping
  UINT8  PackageType;               ///< PackageType
  BRAND_ID BrandId;                 ///< BrandId which contains information about String1, String2, Model and Page
  UINT8  TotalCoreNumber;           ///< Number of total cores
  UINT8  EnabledCoreNumber;         ///< Number of enabled cores
  UINT8  ProcUpgrade;               ///< ProcUpdrade
} CPU_TYPE_INFO;

/// DMI ECC information
typedef struct {
  BOOLEAN EccCapable;               ///< ECC Capable
  UINT8  PartitionRowPosition;      ///< DMI Type 20 offset 10h: Partition Row Position
                                    ///<                         2 - single channel memory
                                    ///<                         0 - dual channel memory
} CPU_GET_MEM_INFO;

/*  Transfer vectors for DMI family specific routines   */
typedef VOID  OPTION_DMI_GET_CPU_INFO (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

typedef UINT8 OPTION_DMI_GET_VOLTAGE (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

typedef UINT16  OPTION_DMI_GET_MAX_SPEED (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

typedef UINT16  OPTION_DMI_GET_EXT_CLOCK (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

typedef VOID  OPTION_DMI_GET_MEM_INFO (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/// Brand table entry format
typedef struct {
  UINT8 PackageType;                ///< Package type
  UINT8 PgOfBrandId;                ///< Page
  UINT8 NumberOfCores;              ///< Number of cores
  UINT8 String1ofBrandId;           ///< String1
  UINT8 ValueSetToDmiTable;         ///< The value which will should be set to DMI table
} DMI_BRAND_ENTRY;

/// Family specific data table structure
typedef struct {
  UINT64                    ProcessorFamily;  ///< processor
  OPTION_DMI_GET_CPU_INFO   *DmiGetCpuInfo;   ///< transfer vectors
  OPTION_DMI_GET_VOLTAGE    *DmiGetVoltage;   ///< vector for reading voltage
  OPTION_DMI_GET_MAX_SPEED  *DmiGetMaxSpeed;  ///< vector for reading speed
  OPTION_DMI_GET_EXT_CLOCK  *DmiGetExtClock;  ///< vector for reading external clock speed
  OPTION_DMI_GET_MEM_INFO   *DmiGetMemInfo;   ///< Get memory information
  UINT8                     LenBrandList;     ///< size of brand table
  CONST DMI_BRAND_ENTRY     *DmiBrandList;    ///< translate brand info to DMI identifier
} PROC_FAMILY_TABLE;

//----------------------------------------------------------------------------
//                         SLIT TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// Format for SRAT Header
typedef struct {
  UINT8   Sign[4];                      ///< Signature
  UINT32  TableLength;                  ///< Table Length
  UINT8   Revision;                     ///< Revision
  UINT8   Checksum;                     ///< Checksum
  UINT8   OemId[6];                     ///< OEM ID
  UINT8   OemTableId[8];                ///< OEM Tabled ID
  UINT32  OemRev;                       ///< OEM Revision
  UINT8   CreatorId[4];                 ///< Creator ID
  UINT32  CreatorRev;                   ///< Creator Revision
} ACPI_TABLE_HEADER;

//----------------------------------------------------------------------------
//                         SRAT TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// Format for SRAT Header
typedef struct _CPU_SRAT_HEADER {
  UINT8   Sign[4];                      ///< Signature
  UINT32  TableLength;                  ///< Table Length
  UINT8   Revision;                     ///< Revision
  UINT8   Checksum;                     ///< Checksum
  UINT8   OemId[6];                     ///< OEM ID
  UINT8   OemTableId[8];                ///< OEM Tabled ID
  UINT32  OemRev;                       ///< OEM Revision
  UINT8   CreatorId[4];                 ///< Creator ID
  UINT32  CreatorRev;                   ///< Creator Revision
  UINT32  TableRev;                     ///< Table Revision
  UINT8   Reserved[8];                  ///< Reserved
} CPU_SRAT_HEADER;


/// Format for SRAT APIC Affinity Entry
typedef struct _CPU_SRAT_APIC_ENTRY {
  UINT8   Type;                         ///< Type
  UINT8   Length;                       ///< Length
  UINT8   Domain;                       ///< Domain
  UINT8   ApicId;                       ///< Apic ID
  UINT32  Flags;                        ///< Flags
  UINT8   LSApicEid;                    ///< Local SAPIC EID
  UINT8   Reserved[7];                  ///< Reserved
} CPU_SRAT_APIC_ENTRY;


/// Format for SRAT Memory Affinity Entry
typedef struct _CPU_SRAT_MEMORY_ENTRY {
  UINT8   Type;                         ///< 0: Memory affinity = 1
  UINT8   Length;                       ///< 1: Length = 40 bytes
  UINT32  Domain;                       ///< 2: Proximity domain
  UINT8   Reserved1[2];                 ///< 6: Reserved
  UINT32  BaseAddrLow;                  ///< 8:  Low 32bits address base
  UINT32  BaseAddrHigh;                 ///< 12: High 32bits address base
  UINT32  LengthAddrLow;                ///< 16: Low 32bits address limit
  UINT32  LengthAddrHigh;               ///< 20: High 32bits address limit
  UINT8   Reserved2[4];                 ///< 24: Memory Type
  UINT32  Flags;                        ///< 28: Flags
  UINT8   Reserved3[8];                 ///< 32: Reserved
} CPU_SRAT_MEMORY_ENTRY;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
AmdCpuLate (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
CreateAcpiWhea (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   VOID                 **WheaMcePtr,
  IN OUT   VOID                 **WheaCmcPtr
  );

AGESA_STATUS
CreateDmiRecords (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   DMI_INFO             **DmiTable
  );

AGESA_STATUS
GetType4Type7Info (
  IN       AP_EXE_PARAMS *ApExeParams
  );

AGESA_STATUS
CreateAcpiSrat (
  IN OUT   AMD_CONFIG_PARAMS    *StdHeader,
  IN OUT   VOID                 **SratPtr
  );

AGESA_STATUS
CreateAcpiSlit (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SlitPtr
  );

VOID
ChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
RunLateApTaskOnAllAPs (
  IN       AP_EXE_PARAMS     *ApParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
RunLateApTaskOnAllCore0s (
  IN       AP_EXE_PARAMS     *ApParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
GenerateSsdt (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  );

#endif // _CPU_LATE_INIT_H_
