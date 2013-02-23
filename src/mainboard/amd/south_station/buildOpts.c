/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

/**
 * @file
 *
 * AMD User options selection for a Brazos platform solution system
 *
 * This file is placed in the user's platform directory and contains the
 * build option selections desired for that platform.
 *
 * For Information about this file, see @ref platforminstall.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 23714 $   @e \$Date: 2009-12-09 17:28:37 -0600 (Wed, 09 Dec 2009) $
 */

#include "Filecode.h"
#define FILECODE PLATFORM_SPECIFIC_OPTIONS_FILECODE


/*  Select the cpu family.  */
#define INSTALL_FAMILY_10_SUPPORT FALSE
#define INSTALL_FAMILY_12_SUPPORT FALSE
#define INSTALL_FAMILY_14_SUPPORT TRUE
#define INSTALL_FAMILY_15_SUPPORT FALSE

/*  Select the cpu socket type.  */
#define INSTALL_G34_SOCKET_SUPPORT  FALSE
#define INSTALL_C32_SOCKET_SUPPORT  FALSE
#define INSTALL_S1G3_SOCKET_SUPPORT FALSE
#define INSTALL_S1G4_SOCKET_SUPPORT FALSE
#define INSTALL_ASB2_SOCKET_SUPPORT FALSE
#define INSTALL_FS1_SOCKET_SUPPORT  FALSE
#define INSTALL_FM1_SOCKET_SUPPORT  FALSE
#define INSTALL_FP1_SOCKET_SUPPORT  FALSE
#define INSTALL_FT1_SOCKET_SUPPORT  TRUE
#define INSTALL_AM3_SOCKET_SUPPORT  FALSE

/*
 * Agesa optional capabilities selection.
 * Uncomment and mark FALSE those features you wish to include in the build.
 * Comment out or mark TRUE those features you want to REMOVE from the build.
 */

#define BLDOPT_REMOVE_FAMILY_10_SUPPORT       TRUE
#define BLDOPT_REMOVE_FAMILY_12_SUPPORT       TRUE
#define BLDOPT_REMOVE_FAMILY_14_SUPPORT       FALSE
#define BLDOPT_REMOVE_FAMILY_15_SUPPORT       TRUE

#define BLDOPT_REMOVE_AM3_SOCKET_SUPPORT      TRUE
#define BLDOPT_REMOVE_ASB2_SOCKET_SUPPORT     TRUE
#define BLDOPT_REMOVE_C32_SOCKET_SUPPORT      TRUE
#define BLDOPT_REMOVE_FM1_SOCKET_SUPPORT      TRUE
#define BLDOPT_REMOVE_FP1_SOCKET_SUPPORT      TRUE
#define BLDOPT_REMOVE_FS1_SOCKET_SUPPORT      TRUE
#define BLDOPT_REMOVE_FT1_SOCKET_SUPPORT      FALSE
#define BLDOPT_REMOVE_G34_SOCKET_SUPPORT      TRUE
#define BLDOPT_REMOVE_S1G3_SOCKET_SUPPORT     TRUE
#define BLDOPT_REMOVE_S1G4_SOCKET_SUPPORT     TRUE

#define BLDOPT_REMOVE_UDIMMS_SUPPORT          FALSE
#define BLDOPT_REMOVE_RDIMMS_SUPPORT        TRUE
#define BLDOPT_REMOVE_LRDIMMS_SUPPORT         FALSE
#define BLDOPT_REMOVE_ECC_SUPPORT             FALSE
//#define BLDOPT_REMOVE_DCT_INTERLEAVE        TRUE
#define BLDOPT_REMOVE_BANK_INTERLEAVE         FALSE
#define BLDOPT_REMOVE_NODE_INTERLEAVE       TRUE
#define BLDOPT_REMOVE_PARALLEL_TRAINING       FALSE
#define BLDOPT_REMOVE_DQS_TRAINING            FALSE
#define BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT    TRUE
#define BLDOPT_REMOVE_MULTISOCKET_SUPPORT     TRUE
#define BLDOPT_REMOVE_ACPI_PSTATES          FALSE
  #define BLDCFG_REMOVE_ACPI_PSTATES_PPC        FALSE
  #define BLDCFG_REMOVE_ACPI_PSTATES_PCT        FALSE
  #define BLDCFG_REMOVE_ACPI_PSTATES_PSD        FALSE
  #define BLDCFG_REMOVE_ACPI_PSTATES_PSS        FALSE
  #define BLDCFG_REMOVE_ACPI_PSTATES_XPSS       FALSE
  #define BLDCFG_FORCE_INDEPENDENT_PSD_OBJECT     FALSE
#define BLDOPT_REMOVE_SRAT            TRUE
#define BLDOPT_REMOVE_SLIT            TRUE
#define BLDOPT_REMOVE_WHEA            TRUE
#define BLDOPT_REMOVE_DMI             TRUE
#define BLDOPT_REMOVE_HT_ASSIST         TRUE
#define BLDOPT_REMOVE_ATM_MODE          TRUE
//#define BLDOPT_REMOVE_MSG_BASED_C1E       TRUE
//#define BLDOPT_REMOVE_LOW_POWER_STATE_FOR_PROCHOT TRUE
#define BLDOPT_REMOVE_MEM_RESTORE_SUPPORT     FALSE
//#define BLDOPT_REMOVE_C6_STATE          TRUE
#define BLDOPT_REMOVE_GFX_RECOVERY        TRUE
#define BLDOPT_REMOVE_EARLY_SAMPLES            TRUE

/*
 * Agesa entry points used in this implementation.
 */
#define AGESA_ENTRY_INIT_RESET                    TRUE
#define AGESA_ENTRY_INIT_RECOVERY                 FALSE
#define AGESA_ENTRY_INIT_EARLY                    TRUE
#define AGESA_ENTRY_INIT_POST                     TRUE
#define AGESA_ENTRY_INIT_ENV                      TRUE
#define AGESA_ENTRY_INIT_MID                      TRUE
#define AGESA_ENTRY_INIT_LATE                     TRUE
#define AGESA_ENTRY_INIT_S3SAVE                   TRUE
#define AGESA_ENTRY_INIT_RESUME                   TRUE
#define AGESA_ENTRY_INIT_LATE_RESTORE             FALSE
#define AGESA_ENTRY_INIT_GENERAL_SERVICES         FALSE

#define BLDCFG_PCI_MMIO_BASE                    CONFIG_MMCONF_BASE_ADDRESS
#define BLDCFG_PCI_MMIO_SIZE                    CONFIG_MMCONF_BUS_NUMBER

#define BLDCFG_VRM_CURRENT_LIMIT                24000
//#define BLDCFG_VRM_NB_CURRENT_LIMIT             0
#define BLDCFG_VRM_LOW_POWER_THRESHOLD          24000
#define BLDCFG_VRM_NB_LOW_POWER_THRESHOLD       1
#define BLDCFG_VRM_SLEW_RATE                    5000
//#define BLDCFG_VRM_NB_SLEW_RATE                 5000
//#define BLDCFG_VRM_ADDITIONAL_DELAY             0
//#define BLDCFG_VRM_NB_ADDITIONAL_DELAY          0
#define BLDCFG_VRM_HIGH_SPEED_ENABLE            TRUE
//#define BLDCFG_VRM_NB_HIGH_SPEED_ENABLE         FALSE
#define BLDCFG_VRM_INRUSH_CURRENT_LIMIT         6000
//#define BLDCFG_VRM_NB_INRUSH_CURRENT_LIMIT      0

//#define BLDCFG_PROCESSOR_SCOPE_NAME0            'C'
//#define BLDCFG_PROCESSOR_SCOPE_NAME1            '0'
//#define BLDCFG_PROCESSOR_SCOPE_IN_SB            FALSE
#define BLDCFG_PLAT_NUM_IO_APICS                3
//#define BLDCFG_PLATFORM_C1E_MODE                C1eModeDisabled
//#define BLDCFG_PLATFORM_C1E_OPDATA              0
//#define BLDCFG_PLATFORM_C1E_MODE_OPDATA1        0
//#define BLDCFG_PLATFORM_C1E_MODE_OPDATA2        0
#define BLDCFG_PLATFORM_CSTATE_MODE             CStateModeC6
#define BLDCFG_PLATFORM_CSTATE_OPDATA           0x840
#define BLDCFG_PLATFORM_CSTATE_IO_BASE_ADDRESS  0x840
//#define BLDCFG_PLATFORM_CPB_MODE                CpbModeAuto
#define BLDCFG_CORE_LEVELING_MODE               CORE_LEVEL_LOWEST
#define BLDCFG_AP_MTRR_SETTINGS_LIST            &OntarioApMtrrSettingsList
#define BLDCFG_AMD_PLATFORM_TYPE                AMD_PLATFORM_MOBILE
//#define BLDCFG_STARTING_BUSNUM                  0
//#define BLDCFG_MAXIMUM_BUSNUM                   0xf8
//#define BLDCFG_ALLOCATED_BUSNUMS                0x20
//#define BLDCFG_PLATFORM_DEEMPHASIS_LIST         0
//#define BLDCFG_BUID_SWAP_LIST                   0
//#define BLDCFG_HTDEVICE_CAPABILITIES_OVERRIDE_LIST  0
//#define BLDCFG_HTFABRIC_LIMITS_LIST             0
//#define BLDCFG_HTCHAIN_LIMITS_LIST              0
//#define BLDCFG_BUS_NUMBERS_LIST                 0
//#define BLDCFG_IGNORE_LINK_LIST                 0
//#define BLDCFG_LINK_SKIP_REGANG_LIST            0
//#define BLDCFG_ADDITIONAL_TOPOLOGIES_LIST       0
//#define BLDCFG_USE_HT_ASSIST                    TRUE
//#define BLDCFG_USE_ATM_MODE                     TRUE
//#define BLDCFG_PLATFORM_CONTROL_FLOW_MODE       Nfcm
#define BLDCFG_S3_LATE_RESTORE                    FALSE
//#define BLDCFG_USE_32_BYTE_REFRESH              FALSE
//#define BLDCFG_USE_VARIABLE_MCT_ISOC_PRIORITY   FALSE
//#define BLDCFG_PLATFORM_POWER_POLICY_MODE       Performance
//#define BLDCFG_SET_HTCRC_SYNC_FLOOD             FALSE
//#define BLDCFG_USE_UNIT_ID_CLUMPING             FALSE
//#define BLDCFG_SYSTEM_PHYSICAL_SOCKET_MAP       0
#define BLDCFG_CFG_GNB_HD_AUDIO                 TRUE
//#define BLDCFG_CFG_ABM_SUPPORT                  FALSE
//#define BLDCFG_CFG_DYNAMIC_REFRESH_RATE         0
//#define BLDCFG_CFG_LCD_BACK_LIGHT_CONTROL       0
//#define BLDCFG_MEM_INIT_PSTATE                  0
//#define BLDCFG_AMD_PSTATE_CAP_VALUE             0
#define BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT       DDR1333_FREQUENCY
#define BLDCFG_MEMORY_MODE_UNGANGED             TRUE
//#define BLDCFG_MEMORY_QUAD_RANK_CAPABLE         TRUE
//#define BLDCFG_MEMORY_QUADRANK_TYPE             QUADRANK_UNBUFFERED
#define BLDCFG_MEMORY_SODIMM_CAPABLE            TRUE
#define BLDCFG_MEMORY_LRDIMM_CAPABLE            FALSE
#define BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING  TRUE
#define BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING  FALSE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING      FALSE
#define BLDCFG_MEMORY_POWER_DOWN                TRUE
#define BLDCFG_POWER_DOWN_MODE                  POWER_DOWN_BY_CHIP_SELECT
//#define BLDCFG_ONLINE_SPARE                     FALSE
//#define BLDCFG_MEMORY_PARITY_ENABLE             FALSE
#define BLDCFG_BANK_SWIZZLE                     TRUE
#define BLDCFG_TIMING_MODE_SELECT               TIMING_MODE_AUTO
#define BLDCFG_MEMORY_CLOCK_SELECT              DDR1333_FREQUENCY
#define BLDCFG_DQS_TRAINING_CONTROL             TRUE
#define BLDCFG_IGNORE_SPD_CHECKSUM              FALSE
#define BLDCFG_USE_BURST_MODE                   FALSE
#define BLDCFG_MEMORY_ALL_CLOCKS_ON             FALSE
//#define BLDCFG_ENABLE_ECC_FEATURE               TRUE
//#define BLDCFG_ECC_REDIRECTION                  FALSE
//#define BLDCFG_SCRUB_DRAM_RATE                  0
//#define BLDCFG_SCRUB_L2_RATE                    0
//#define BLDCFG_SCRUB_L3_RATE                    0
//#define BLDCFG_SCRUB_IC_RATE                    0
//#define BLDCFG_SCRUB_DC_RATE                    0
//#define BLDCFG_ECC_SYNC_FLOOD                   0
//#define BLDCFG_ECC_SYMBOL_SIZE                  0
//#define BLDCFG_1GB_ALIGN                        FALSE
#define BLDCFG_UMA_ALLOCATION_MODE              UMA_AUTO
#define BLDCFG_UMA_ALLOCATION_SIZE              0
#define BLDCFG_UMA_ABOVE4G_SUPPORT              FALSE
#define BLDCFG_UMA_ALIGNMENT                    NO_UMA_ALIGNED
#define BLDCFG_HEAP_DRAM_ADDRESS                0xB0000
#define BLDCFG_CFG_TEMP_PCIE_MMIO_BASE_ADDRESS  0xD0000000

/*
 * Agesa configuration values selection.
 * Uncomment and specify the value for the configuration options
 * needed by the system.
 */
#include "AGESA.h"
#include "CommonReturns.h"

/* The fixed MTRR values to be set after memory initialization. */
CONST AP_MTRR_SETTINGS ROMDATA OntarioApMtrrSettingsList[] =
{
  { AMD_AP_MTRR_FIX64k_00000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX16k_80000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX16k_A0000, 0x0000000000000000ull },
  { AMD_AP_MTRR_FIX4k_C0000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX4k_C8000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX4k_D0000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX4k_D8000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX4k_E0000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX4k_E8000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX4k_F0000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX4k_F8000, 0x1E1E1E1E1E1E1E1Eull },
  { CPU_LIST_TERMINAL }
};

/*  Include the files that instantiate the configuration definitions.  */

#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "AdvancedApi.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "cpuFeatures.h"
#include "Table.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "GnbInterface.h"

/*****************************************************************************
 *   Define the RELEASE VERSION string
 *
 * The Release Version string should identify the next planned release.
 * When a branch is made in preparation for a release, the release manager
 * should change/confirm that the branch version of this file contains the
 * string matching the desired version for the release. The trunk version of
 * the file should always contain a trailing 'X'. This will make sure that a
 * development build from trunk will not be confused for a released version.
 * The release manager will need to remove the trailing 'X' and update the
 * version string as appropriate for the release. The trunk copy of this file
 * should also be updated/incremented for the next expected version, + trailing 'X'
 ****************************************************************************/
                  // This is the delivery package title, "BrazosPI"
                  // This string MUST be exactly 8 characters long
#define AGESA_PACKAGE_STRING  {'c', 'b', '_', 'A', 'g', 'e', 's', 'a'}

                  // This is the release version number of the AGESA component
                  // This string MUST be exactly 12 characters long
#define AGESA_VERSION_STRING  {'V', '1', '.', '1', '.', '0', '.', '3', ' ', ' ', ' ', ' '}

/* MEMORY_BUS_SPEED */
#define DDR400_FREQUENCY              200 ///< DDR 400
#define DDR533_FREQUENCY              266 ///< DDR 533
#define DDR667_FREQUENCY              333 ///< DDR 667
#define DDR800_FREQUENCY              400 ///< DDR 800
#define DDR1066_FREQUENCY             533 ///< DDR 1066
#define DDR1333_FREQUENCY             667 ///< DDR 1333
#define DDR1600_FREQUENCY             800 ///< DDR 1600
#define DDR1866_FREQUENCY             933 ///< DDR 1866
#define UNSUPPORTED_DDR_FREQUENCY     934 ///< Highest limit of DDR frequency

/* QUANDRANK_TYPE*/
#define QUADRANK_REGISTERED             0 ///< Quadrank registered DIMM
#define QUADRANK_UNBUFFERED             1 ///< Quadrank unbuffered DIMM

/* USER_MEMORY_TIMING_MODE */
#define TIMING_MODE_AUTO                0 ///< Use best rate possible
#define TIMING_MODE_LIMITED             1 ///< Set user top limit
#define TIMING_MODE_SPECIFIC            2 ///< Set user specified speed

/* POWER_DOWN_MODE */
#define POWER_DOWN_BY_CHANNEL           0 ///< Channel power down mode
#define POWER_DOWN_BY_CHIP_SELECT       1 ///< Chip select power down mode

// The following definitions specify the default values for various parameters in which there are
// no clearly defined defaults to be used in the common file.  The values below are based on product
// and BKDG content, please consult the AGESA Memory team for consultation.
#define DFLT_SCRUB_DRAM_RATE            (0)
#define DFLT_SCRUB_L2_RATE              (0)
#define DFLT_SCRUB_L3_RATE              (0)
#define DFLT_SCRUB_IC_RATE              (0)
#define DFLT_SCRUB_DC_RATE              (0)
#define DFLT_MEMORY_QUADRANK_TYPE       QUADRANK_UNBUFFERED
#define DFLT_VRM_SLEW_RATE              (5000)

// Instantiate all solution relevant data.
#include "PlatformInstall.h"

/*----------------------------------------------------------------------------------------
 *                        CUSTOMER OVERIDES MEMORY TABLE
 *----------------------------------------------------------------------------------------
 */

/*
 *  Platform Specific Overriding Table allows IBV/OEM to pass in platform information to AGESA
 *  (e.g. MemClk routing, the number of DIMM slots per channel,...). If PlatformSpecificTable
 *  is populated, AGESA will base its settings on the data from the table. Otherwise, it will
 *  use its default conservative settings.
 */
CONST PSO_ENTRY ROMDATA DefaultPlatformMemoryConfiguration[] = {
  //
  // The following macros are supported (use comma to separate macros):
  //
  // MEMCLK_DIS_MAP(SocketID, ChannelID, MemClkDisBit0CSMap,..., MemClkDisBit7CSMap)
  //      The MemClk pins are identified based on BKDG definition of Fn2x88[MemClkDis] bitmap.
  //      AGESA will base on this value to disable unused MemClk to save power.
  //      Example:
  //      BKDG definition of Fn2x88[MemClkDis] bitmap for AM3 package is like below:
  //           Bit AM3/S1g3 pin name
  //           0   M[B,A]_CLK_H/L[0]
  //           1   M[B,A]_CLK_H/L[1]
  //           2   M[B,A]_CLK_H/L[2]
  //           3   M[B,A]_CLK_H/L[3]
  //           4   M[B,A]_CLK_H/L[4]
  //           5   M[B,A]_CLK_H/L[5]
  //           6   M[B,A]_CLK_H/L[6]
  //           7   M[B,A]_CLK_H/L[7]
  //      And platform has the following routing:
  //           CS0   M[B,A]_CLK_H/L[4]
  //           CS1   M[B,A]_CLK_H/L[2]
  //           CS2   M[B,A]_CLK_H/L[3]
  //           CS3   M[B,A]_CLK_H/L[5]
  //      Then platform can specify the following macro:
  //      MEMCLK_DIS_MAP(ANY_SOCKET, ANY_CHANNEL, 0x00, 0x00, 0x02, 0x04, 0x01, 0x08, 0x00, 0x00)
  //
  // CKE_TRI_MAP(SocketID, ChannelID, CKETriBit0CSMap, CKETriBit1CSMap)
  //      The CKE pins are identified based on BKDG definition of Fn2x9C_0C[CKETri] bitmap.
  //      AGESA will base on this value to tristate unused CKE to save power.
  //
  // ODT_TRI_MAP(SocketID, ChannelID, ODTTriBit0CSMap,..., ODTTriBit3CSMap)
  //      The ODT pins are identified based on BKDG definition of Fn2x9C_0C[ODTTri] bitmap.
  //      AGESA will base on this value to tristate unused ODT pins to save power.
  //
  // CS_TRI_MAP(SocketID, ChannelID, CSTriBit0CSMap,..., CSTriBit7CSMap)
  //      The Chip select pins are identified based on BKDG definition of Fn2x9C_0C[ChipSelTri] bitmap.
  //      AGESA will base on this value to tristate unused Chip select to save power.
  //
  // NUMBER_OF_DIMMS_SUPPORTED(SocketID, ChannelID, NumberOfDimmSlotsPerChannel)
  //      Specifies the number of DIMM slots per channel.
  //
  // NUMBER_OF_CHIP_SELECTS_SUPPORTED(SocketID, ChannelID, NumberOfChipSelectsPerChannel)
  //      Specifies the number of Chip selects per channel.
  //
  // NUMBER_OF_CHANNELS_SUPPORTED(SocketID, NumberOfChannelsPerSocket)
  //      Specifies the number of channels per socket.
  //
  // OVERRIDE_DDR_BUS_SPEED(SocketID, ChannelID, USER_MEMORY_TIMING_MODE, MEMORY_BUS_SPEED)
  //      Specifies DDR bus speed of channel ChannelID on socket SocketID.
  //
  // DRAM_TECHNOLOGY(SocketID, TECHNOLOGY_TYPE)
  //      Specifies the DRAM technology type of socket SocketID (DDR2, DDR3,...)
  //
  // WRITE_LEVELING_SEED(SocketID, ChannelID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed,
  //      Byte6Seed, Byte7Seed, ByteEccSeed)
  //      Specifies the write leveling seed for a channel of a socket.
  //
  NUMBER_OF_DIMMS_SUPPORTED (ANY_SOCKET, ANY_CHANNEL, 2),
  NUMBER_OF_CHANNELS_SUPPORTED (ANY_SOCKET, 1),
  PSO_END
};

/*
 * These tables are optional and may be used to adjust memory timing settings
 */
#include "mm.h"
#include "mn.h"

//DA Customer table
CONST UINT8 AGESA_MEM_TABLE_ON[][sizeof (MEM_TABLE_ALIAS)] =
{
 // Hardcoded Memory Training Values

 // The following macro should be used to override training values for your platform
 //
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNodes, MTDcts, MTDIMMs, BFRdDqsDly, MTOverride, 0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20),
 //
 //   NOTE:
 //   The following training hardcode values are example values that were taken from a tilapia motherboard
 //   with a particular DIMM configuration.  To hardcode your own values, uncomment the appropriate line in
 //   the table and replace the byte lane values with your own.
 //
 //                                                                               ------------------ BYTE LANES ----------------------
 //                                                                                BL0   BL1   BL2   BL3   BL4   BL5   BL6   Bl7   ECC
 // Write Data Timing
 // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct0, MTDIMM0, BFWrDatDly, MTOverride, 0x1D, 0x20, 0x26, 0x2B, 0x37, 0x3A, 0x3e, 0x3F, 0x30),// DCT0, DIMM0
 // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct0, MTDIMM1, BFWrDatDly, MTOverride, 0x1D, 0x00, 0x06, 0x0B, 0x17, 0x1A, 0x1E, 0x1F, 0x10),// DCT0, DIMM1
 // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct1, MTDIMM0, BFWrDatDly, MTOverride, 0x18, 0x1D, 0x27, 0x2B, 0x3B, 0x3B, 0x3E, 0x3E, 0x30),// DCT1, DIMM0
 // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct1, MTDIMM1, BFWrDatDly, MTOverride, 0x18, 0x1D, 0x1C, 0x0B, 0x17, 0x1A, 0x1D, 0x1C, 0x10),// DCT1, DIMM1

 // DQS Receiver Enable
 // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct0, MTDIMM0, BFRcvEnDly, MTOverride, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),// DCT0, DIMM0
 // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct0, MTDIMM1, BFRcvEnDly, MTOverride, 0x7C, 0x7D, 0x7E, 0x81, 0x88, 0x8F, 0x96, 0x9F, 0x84),// DCT0, DIMM1
 // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct1, MTDIMM0, BFRcvEnDly, MTOverride, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),// DCT1, DIMM0
 // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct1, MTDIMM1, BFRcvEnDly, MTOverride, 0x1C, 0x1D, 0x1E, 0x01, 0x08, 0x0F, 0x16, 0x1F, 0x04),// DCT1, DIMM1

 // Write DQS Delays
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM0, BFWrDqsDly, MTOverride, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),// DCT0, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM1, BFWrDqsDly, MTOverride, 0x06, 0x0D, 0x12, 0x1A, 0x25, 0x28, 0x2C, 0x2C, 0x44),// DCT0, DIMM1
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM0, BFWrDqsDly, MTOverride, 0x07, 0x0E, 0x14, 0x1B, 0x24, 0x29, 0x2B, 0x2C, 0x1F),// DCT1, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM1, BFWrDqsDly, MTOverride, 0x07, 0x0C, 0x14, 0x19, 0x25, 0x28, 0x2B, 0x2B, 0x1A),// DCT1, DIMM1

 // Read DQS Delays
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM0, BFRdDqsDly, MTOverride, 0x10, 0x10, 0x0E, 0x10, 0x10, 0x10, 0x10, 0x0E, 0x10),// DCT0, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM1, BFRdDqsDly, MTOverride, 0x10, 0x10, 0x0E, 0x10, 0x10, 0x10, 0x10, 0x1E, 0x10),// DCT0, DIMM1
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM0, BFRdDqsDly, MTOverride, 0x10, 0x10, 0x0E, 0x10, 0x10, 0x10, 0x10, 0x1E, 0x10),// DCT1, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM1, BFRdDqsDly, MTOverride, 0x10, 0x10, 0x0E, 0x10, 0x10, 0x10, 0x10, 0x1E, 0x10),// DCT1, DIMM1
 //--------------------------------------------------------------------------------------------------------------------------------------------------
 // TABLE END
  NBACCESS (MTEnd, 0,  0, 0, 0, 0),      // End of Table
};
CONST UINT8 SizeOfTableON = sizeof (AGESA_MEM_TABLE_ON) / sizeof (AGESA_MEM_TABLE_ON[0]);

/* ***************************************************************************
 *   Optional User code to be included into the AGESA build
 *    These may be 32-bit call-out routines...
 */
//AGESA_STATUS
//AgesaReadSpd (
//  IN        UINTN                 FcnData,
//  IN OUT    AGESA_READ_SPD_PARAMS *ReadSpd
//  )
//{
//  /* platform code to read an SPD...  */
//  return Status;
//}


