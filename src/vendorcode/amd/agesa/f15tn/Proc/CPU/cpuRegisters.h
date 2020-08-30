/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Register Table Related Functions
 *
 * Contains the definition of the CPU CPUID MSRs and PCI registers with BKDG recommended values
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 64348 $   @e \$Date: 2012-01-19 03:43:52 -0600 (Thu, 19 Jan 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
 ******************************************************************************
 */

#ifndef _CPU_REGISTERS_H_
#define _CPU_REGISTERS_H_

#include "Family/cpuFamRegisters.h"
/*
 *--------------------------------------------------------------
 *
 *                      M O D U L E S    U S E D
 *
 *---------------------------------------------------------------
 */

/*
 *--------------------------------------------------------------
 *
 *                      D E F I N I T I O N S  /  M A C R O S
 *
 *---------------------------------------------------------------
 */

#define BIT0        0x0000000000000001ull
#define BIT1        0x0000000000000002ull
#define BIT2        0x0000000000000004ull
#define BIT3        0x0000000000000008ull
#define BIT4        0x0000000000000010ull
#define BIT5        0x0000000000000020ull
#define BIT6        0x0000000000000040ull
#define BIT7        0x0000000000000080ull
#define BIT8        0x0000000000000100ull
#define BIT9        0x0000000000000200ull
#define BIT10       0x0000000000000400ull
#define BIT11       0x0000000000000800ull
#define BIT12       0x0000000000001000ull
#define BIT13       0x0000000000002000ull
#define BIT14       0x0000000000004000ull
#define BIT15       0x0000000000008000ull
#define BIT16       0x0000000000010000ull
#define BIT17       0x0000000000020000ull
#define BIT18       0x0000000000040000ull
#define BIT19       0x0000000000080000ull
#define BIT20       0x0000000000100000ull
#define BIT21       0x0000000000200000ull
#define BIT22       0x0000000000400000ull
#define BIT23       0x0000000000800000ull
#define BIT24       0x0000000001000000ull
#define BIT25       0x0000000002000000ull
#define BIT26       0x0000000004000000ull
#define BIT27       0x0000000008000000ull
#define BIT28       0x0000000010000000ull
#define BIT29       0x0000000020000000ull
#define BIT30       0x0000000040000000ull
#define BIT31       0x0000000080000000ull
#define BIT32       0x0000000100000000ull
#define BIT33       0x0000000200000000ull
#define BIT34       0x0000000400000000ull
#define BIT35       0x0000000800000000ull
#define BIT36       0x0000001000000000ull
#define BIT37       0x0000002000000000ull
#define BIT38       0x0000004000000000ull
#define BIT39       0x0000008000000000ull
#define BIT40       0x0000010000000000ull
#define BIT41       0x0000020000000000ull
#define BIT42       0x0000040000000000ull
#define BIT43       0x0000080000000000ull
#define BIT44       0x0000100000000000ull
#define BIT45       0x0000200000000000ull
#define BIT46       0x0000400000000000ull
#define BIT47       0x0000800000000000ull
#define BIT48       0x0001000000000000ull
#define BIT49       0x0002000000000000ull
#define BIT50       0x0004000000000000ull
#define BIT51       0x0008000000000000ull
#define BIT52       0x0010000000000000ull
#define BIT53       0x0020000000000000ull
#define BIT54       0x0040000000000000ull
#define BIT55       0x0080000000000000ull
#define BIT56       0x0100000000000000ull
#define BIT57       0x0200000000000000ull
#define BIT58       0x0400000000000000ull
#define BIT59       0x0800000000000000ull
#define BIT60       0x1000000000000000ull
#define BIT61       0x2000000000000000ull
#define BIT62       0x4000000000000000ull
#define BIT63       0x8000000000000000ull

/// CPUID related registers
#define AMD_CPUID_FMF               0x80000001ul  // Family Model Features information
#define AMD_CPUID_APICID_LPC_BID    0x00000001ul  // Local APIC ID, Logical Processor Count, Brand ID
#define AMD_CPUID_L2L3Cache_L2TLB   0x80000006ul
#define CPUID_ASSOCIATIVITY_DISABLED 0x00
#define CPUID_ASSOCIATIVITY_1_WAY   0x01
#define CPUID_ASSOCIATIVITY_2_WAY   0x02
#define CPUID_ASSOCIATIVITY_4_WAY   0x04
#define CPUID_ASSOCIATIVITY_8_WAY   0x06
#define CPUID_ASSOCIATIVITY_16_WAY  0x08
#define CPUID_ASSOCIATIVITY_32_WAY  0x0A
#define CPUID_ASSOCIATIVITY_48_WAY  0x0B
#define CPUID_ASSOCIATIVITY_64_WAY  0x0C
#define CPUID_ASSOCIATIVITY_96_WAY  0x0D
#define CPUID_ASSOCIATIVITY_128_WAY 0x0E
#define CPUID_ASSOCIATIVITY_FULLY   0x0F
#define AMD_CPUID_TLB_L1Cache       0x80000005ul
#define AMD_CPUID_APM               0x80000007ul
#define LOCAL_APIC_ID               24
#define LOGICAL_PROCESSOR_COUNT     16
#define AMD_CPUID_ASIZE_PCCOUNT     0x80000008ul  // Address Size, Physical Core Count

/// CPU Logical ID Transfer
typedef struct {
  UINT32         RawId;                         ///< RawID
  UINT64         LogicalId;                     ///< LogicalID
} CPU_LOGICAL_ID_XLAT;

/// Logical CPU ID Table
typedef struct {
  IN       UINT32  Elements;                    ///< Number of Elements
  IN       CPU_LOGICAL_ID_XLAT *LogicalIdTable; ///< CPU Logical ID Transfer table Pointer
} LOGICAL_ID_TABLE;

// MSRs
// ------------------------
#define MCG_CTL_P                   0x00000100ul              // bit 8  for MCG_CTL_P under MSRR
#define MSR_MCG_CAP                 0x00000179ul
#define MSR_MC0_CTL                 0x00000400ul
#define MSR_MC0_STATUS              0x00000401ul
#define MSR_MC5_STATUS              0x00000415ul
#define MSR_MC6_STATUS              0x00000419ul

#define MSR_APIC_BAR                0x0000001Bul

#define CPUID_LONG_MODE_ADDR        0x80000008ul
#define AMD_CPUID_FMF               0x80000001ul

#define MSR_EXTENDED_FEATURE_EN     0xC0000080ul
#define MSR_MC_MISC_LINK_THRESHOLD  0xC0000408ul
#define MSR_MC_MISC_L3_THRESHOLD    0xC0000409ul

/// Patch Loader Register
typedef struct {
  UINT64 PatchBase:32;                ///< Linear address of patch header address block
  UINT64 SBZ:32;                      ///< Should be zero
} PATCH_LOADER_MSR;

#define MSR_SYS_CFG                 0xC0010010ul    // SYSCFG - F15 Shared
#define MSR_TOM2                    0xC001001Dul    // TOP_MEM2 - F15 Shared
#define MSR_MC0_CTL_MASK            0xC0010044ul    // MC0 Control Mask
#define MSR_MC1_CTL_MASK            0xC0010045ul    // MC1 Control Mask
#define MSR_MC2_CTL_MASK            0xC0010046ul    // MC2 Control Mask
#define MSR_MC4_CTL_MASK            0xC0010048ul    // MC4 Control Mask

#define MSR_CPUID_FEATS             0xC0011004ul    // CPUID Features
#define MSR_CPUID_EXT_FEATS         0xC0011005ul    // CPUID Extended Features
#define MSR_HWCR                    0xC0010015ul
#define MSR_NB_CFG                  0xC001001Ful    // NB Config
#define ENABLE_CF8_EXT_CFG          0x00004000ul    // [46]
#define INIT_APIC_CPUID_LO          0x00400000ul    // [54]
#define MSR_LS_CFG                  0xC0011020ul
#define MSR_IC_CFG                  0xC0011021ul    // ICache Config - F15 Shared
#define MSR_DC_CFG                  0xC0011022ul
#define MSR_ME_CFG                  0xC0011029ul
#define MSR_CU_CFG                  0xC0011023ul    // F15 Shared
#define MSR_DE_CFG                  0xC0011029ul    // F15 Shared
#define MSR_CU_CFG2                 0xC001102Aul    // F15 Shared
#define MSR_CU_CFG3                 0xC001102Bul    // F15 Shared
#define MSR_LS_CFG2                 0xC001102Dul
#define MSR_IBS_OP_DATA3            0xC0011037ul


#define MSR_CPUID_NAME_STRING0      0xC0010030ul    // First CPUID namestring register
#define MSR_CPUID_NAME_STRING1      0xC0010031ul
#define MSR_CPUID_NAME_STRING2      0xC0010032ul
#define MSR_CPUID_NAME_STRING3      0xC0010033ul
#define MSR_CPUID_NAME_STRING4      0xC0010034ul
#define MSR_CPUID_NAME_STRING5      0xC0010035ul    // Last CPUID namestring register
#define MSR_MMIO_Cfg_Base           0xC0010058ul    // MMIO Configuration Base Address Register
#define MSR_BIST                    0xC0010060ul    // BIST Results register
#define MSR_OSVW_ID_Length          0xC0010140ul
#define MSR_OSVW_Status             0xC0010141ul
#define MSR_NB_PERF_CTL0            0xC0010240ul
#define MSR_NB_PERF_CTR0            0xC0010241ul
#define MSR_NB_PERF_CTL1            0xC0010242ul
#define MSR_NB_PERF_CTR1            0xC0010243ul
#define MSR_NB_PERF_CTL2            0xC0010244ul
#define MSR_NB_PERF_CTR2            0xC0010245ul
#define MSR_NB_PERF_CTL3            0xC0010246ul
#define MSR_NB_PERF_CTR3            0xC0010247ul
#define MSR_PERF_CONTROL3           0xC0010003ul    // Perfromance control register number 3
#define MSR_PERF_COUNTER3           0xC0010007ul    // Performance counter register number 3
#define PERF_RESERVE_BIT_MASK       0x030FFFDFFFFFull  // Mask of the Performance control Reserve bits
#define PERF_CAR_CORRUPTION_EVENT   0x040040F0E2ul    // Configure the controller to capture the
                                                    // CAR Corruption
// FUNC_0 registers
// ----------------
#define HT_LINK_FREQ_OFFSET         8           // Link HT Frequency from capability base
#define HT_LINK_CONTROL_REG_OFFSET  4
#define HT_LINK_TYPE_REG_OFFSET     0x18
#define HT_LINK_EXTENDED_FREQ       0x1C
#define HT_LINK_HOST_CAP_MAX        0x20        // HT Host Capability offsets are less than its size.
#define HT_CAPABILITIES_POINTER     0x34
#define NODE_ID                     0x60
#define HT_INIT_CTRL                0x6C
#define HT_INIT_CTRL_REQ_DIS        0x02        // [1] = ReqDis
#define HT_INIT_COLD_RST_DET        BIT4
#define HT_INIT_BIOS_RST_DET_0      BIT5
#define HT_INIT_BIOS_RST_DET_1      BIT9
#define HT_INIT_BIOS_RST_DET_2      BIT10
#define HT_INIT_BIOS_RST_DET        BIT9 | BIT10
#define HT_TRANS_CTRL               0x68
#define HT_TRANS_CTRL_CPU1_EN       0x00000020ul  // [5] = CPU1 Enable
#define HT_LINK_CONTROL_0           0x84
#define HT_LINK_FREQ_0              0x88        // Link HT Frequency
#define EXTENDED_NODE_ID            0x160
#define ECS_HT_TRANS_CTRL           0x168
#define ECS_HT_TRANS_CTRL_CPU2_EN   0x00000001ul  // [0] = CPU2 Enable
#define ECS_HT_TRANS_CTRL_CPU3_EN   0x00000002ul  // [1] = CPU3 Enable
#define ECS_HT_TRANS_CTRL_CPU4_EN   0x00000004ul  // [2] = CPU4 Enable
#define ECS_HT_TRANS_CTRL_CPU5_EN   0x00000008ul  // [3] = CPU5 Enable

#define CORE_CTRL                   0x1DC
#define CORE_CTRL_CORE1_EN          0x00000002ul
#define CORE_CTRL_CORE2_EN          0x00000004ul
#define CORE_CTRL_CORE3_EN          0x00000008ul
#define CORE_CTRL_CORE4_EN          0x00000010ul
#define CORE_CTRL_CORE5_EN          0x00000020ul
#define CORE_CTRL_CORE6_EN          0x00000040ul
#define CORE_CTRL_CORE7_EN          0x00000080ul
#define CORE_CTRL_CORE8_EN          0x00000100ul
#define CORE_CTRL_CORE9_EN          0x00000200ul

// FUNC_3 registers
// ----------------
#define HARDWARE_THERMAL_CTRL_REG       0x64
#define SOFTWARE_THERMAL_CTRL_REG       0x68

#define ACPI_PSC_0_REG                  0x80    // ACPI Power State Control Registers
#define ACPI_PSC_4_REG                  0x84

#define NB_CFG_HIGH_REG                 0x8C
#define POWER_CTRL_MISCELLANEOUS_REG    0xA0
#define CLOCK_POWER_TIMING_CTRL2_REG    0xDC
#define NORTH_BRIDGE_CAPABILITIES_REG   0xE8
#define MULTI_NODE_CPU                  29
#define CPUID_FMR                       0xFC    // Family / Model registers
#define DOWNCORE_CTRL                   0x190   // Downcore Control Register

#define LINK_TO_XCS_TOKEN_COUNT_REG_3X148 0x148
#define REG_HT4_PHY_OFFSET_BASE_4X180    0x180
#define REG_HT4_PHY_DATA_PORT_BASE_4X184 0x184

#define HTPHY_OFFSET_MASK         0xE00001FFul
#define HTPHY_WRITE_CMD           0x40000000ul
#define HTPHY_IS_COMPLETE_MASK    0x80000000ul
#define HTPHY_DIRECT_MAP          0x20000000ul
#define HTPHY_DIRECT_OFFSET_MASK  0x6000FFFFul

// FUNC_5 registers
// ----------------
#define COMPUTE_UNIT_STATUS       0x80
#define NORTH_BRIDGE_CAPABILITIES_2_REG  0x84


// Misc. defines.
#define PCI_DEV_BASE    24

#define CPU_STEPPING    0x0000000Ful
#define CPU_MODEL       0x000000F0ul
#define CPU_EMODEL      0x000F0000ul
#define CPU_EFAMILY     0x00F00000ul
#define CPU_FMS_MASK    CPU_EFAMILY | CPU_EMODEL | CPU_MODEL | CPU_STEPPING

#define HTPHY_SELECT        2
#define PCI_SELECT          1
#define MSR_SELECT          0

#define LOGICAL_ID      1
#define F_SCHEME        0
#define DR_SCHEME       1
#define GR_SCHEME       2

#define DR_NO_STRING        0
#define DR_SOCKET_C32       5
#define DR_SOCKET_ASB2      4
#define DR_SOCKET_G34       3
#define DR_SOCKET_S1G3      2
#define DR_SOCKET_S1G4      2
#define DR_SOCKET_AM3       1
#define DR_SOCKET_1207      0
#define LN_SOCKET_FM1       2
#define LN_SOCKET_FS1       1
#define LN_SOCKET_FP1       0
#define ON_SOCKET_FT1       0
#define KR_SOCKET_FT2       0
#define OR_SOCKET_AM3       1
#define OR_SOCKET_G34       3
#define OR_SOCKET_C32       5
#define TN_SOCKET_FP2       0
#define TN_SOCKET_FS1       1
#define TN_SOCKET_FM2       2
#define KV_SOCKET_FS2       1
#define KV_SOCKET_FM3       2
#define KB_SOCKET_FT2       0
#define SOCKET_IGNORE       0xF

#define LAPIC_BASE_ADDR_MASK    0x0000FFFFFFFFF000ull
#define APIC_EXT_BRDCST_MASK    0x000E0000ul
#define APIC_ENABLE_BIT         0x00000800ul

#ifndef LOCAL_APIC_ADDR
#define LOCAL_APIC_ADDR         0xFEE00000ul
#endif

#define INT_CMD_REG_LO          0x300
#define INT_CMD_REG_HI          0x310
#define REMOTE_MSG_REG          0x380
#define REMOTE_READ_REG         0xC0
#define APIC_ID_REG             0x20
#define APIC20_ApicId           24
#define CMD_REG_TO_READ_DATA    0x338

#define MAX_CORE_ID_SIZE 8
#define MAX_CORE_ID_MASK ((1 << MAX_CORE_ID_SIZE) - 1)

/*-------------------------
 * Default definitions
 *-------------------------
 */
#define DOWNCORE_MASK_SINGLE    0xFFFFFFFEul
#define DOWNCORE_MASK_DUAL      0xFFFFFFFCul
#define DOWNCORE_MASK_TRI       0xFFFFFFF8ul
#define DOWNCORE_MASK_FOUR      0xFFFFFFF0ul
#define DOWNCORE_MASK_FIVE      0xFFFFFFE0ul
#define DOWNCORE_MASK_SIX       0xFFFFFFC0ul
#define DOWNCORE_MASK_DUAL_COMPUTE_UNIT  0xFFFFFFFAul
#define DOWNCORE_MASK_TRI_COMPUTE_UNIT   0xFFFFFFEAul
#define DOWNCORE_MASK_FOUR_COMPUTE_UNIT  0xFFFFFFAAul

#define DELIVERY_STATUS             BIT13
#define REMOTE_READ_STAT_MASK       0x00030000ul
#define REMOTE_DELIVERY_PENDING     0x00010000ul
#define REMOTE_DELIVERY_DONE        0x00020000ul

/*
 * --------------------------------------------------------------------------------------
 *
 *             D E F I N E S    /   T Y P E D E F S   /  S T R U C T U R E S
 *
 * --------------------------------------------------------------------------------------
 */

/// CpuEarly param type
typedef struct {
  IN       UINT8                  MemInitPState;        ///< Pstate value during memory initial
  IN       PLATFORM_CONFIGURATION PlatformConfig;       ///< Runtime configurable user options
} AMD_CPU_EARLY_PARAMS;

/// Enum - Will be used to access each structure
///        related to each CPU family
typedef enum {
  REVF,                                       ///< NPT, RevF
  REVG,                                       ///< NPT, RevG
  DEERHOUND,                                  ///< Family 10h, Deerhound
  GRIFFIN                                     ///< Family 11h, Griffin
} CPU_FAMILY;

/// CPUID
typedef enum {
  REG_EAX,                                    ///< EAX
  REG_EBX,                                    ///< EBX
  REG_ECX,                                    ///< ECX
  REG_EDX                                     ///< EDX
} CPUID_REG;

#endif      // _CPU_REGISTERS_H_

