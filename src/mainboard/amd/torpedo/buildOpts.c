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
 * AMD User options selection for a Sabine/Lynx platform solution system
 *
 * This file is placed in the user's platform directory and contains the
 * build option selections desired for that platform.
 *
 * For Information about this file, see @ref platforminstall.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 6049 $   @e \$Date: 2008-05-14 01:58:02 -0500 (Wed, 14 May 2008) $
 */
#include  "AGESA.h"
#include  "CommonReturns.h"
#include "Filecode.h"
#define FILECODE PLATFORM_SPECIFIC_OPTIONS_FILECODE


/*  Select the cpu family.  */
#define INSTALL_FAMILY_10_SUPPORT FALSE
#define INSTALL_FAMILY_12_SUPPORT TRUE
#define INSTALL_FAMILY_14_SUPPORT FALSE
#define INSTALL_FAMILY_15_SUPPORT FALSE

/*  Select the cpu socket type.  */
#define INSTALL_G34_SOCKET_SUPPORT  FALSE
#define INSTALL_C32_SOCKET_SUPPORT  FALSE
#define INSTALL_S1G3_SOCKET_SUPPORT FALSE
#define INSTALL_S1G4_SOCKET_SUPPORT FALSE
#define INSTALL_ASB2_SOCKET_SUPPORT FALSE
#define INSTALL_FS1_SOCKET_SUPPORT  TRUE
#define INSTALL_FM1_SOCKET_SUPPORT  FALSE
#define INSTALL_FP1_SOCKET_SUPPORT  TRUE
#define INSTALL_FT1_SOCKET_SUPPORT  FALSE
#define INSTALL_AM3_SOCKET_SUPPORT  FALSE

/*
 * Agesa optional capabilities selection.
 * Uncomment and mark FALSE those features you wish to include in the build.
 * Comment out or mark TRUE those features you want to REMOVE from the build.
 */

#define BLDOPT_REMOVE_UDIMMS_SUPPORT           FALSE
#define BLDOPT_REMOVE_RDIMMS_SUPPORT           FALSE
#define BLDOPT_REMOVE_ECC_SUPPORT              FALSE
#define BLDOPT_REMOVE_BANK_INTERLEAVE          FALSE
#define BLDOPT_REMOVE_DCT_INTERLEAVE           FALSE
#define BLDOPT_REMOVE_NODE_INTERLEAVE          TRUE
#define BLDOPT_REMOVE_PARALLEL_TRAINING        TRUE
#define BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT     TRUE
#define BLDOPT_REMOVE_MEM_RESTORE_SUPPORT      FALSE
#define BLDOPT_REMOVE_DDR2_SUPPORT             TRUE
#define BLDOPT_REMOVE_DDR3_SUPPORT             FALSE
#define BLDOPT_REMOVE_MULTISOCKET_SUPPORT      TRUE
#define BLDOPT_REMOVE_ACPI_PSTATES             FALSE
#define BLDOPT_REMOVE_SRAT                     TRUE
#define BLDOPT_REMOVE_SLIT                     TRUE
#define BLDOPT_REMOVE_WHEA                     TRUE
#define BLDOPT_REMOVE_DMI                      FALSE
#define BLDOPT_REMOVE_EARLY_SAMPLES            TRUE
#define BLDCFG_REMOVE_ACPI_PSTATES_PPC         FALSE
#define BLDCFG_REMOVE_ACPI_PSTATES_PCT         FALSE
#define BLDCFG_REMOVE_ACPI_PSTATES_PSD         FALSE
#define BLDCFG_REMOVE_ACPI_PSTATES_PSS         FALSE
#define BLDCFG_REMOVE_ACPI_PSTATES_XPSS        FALSE

//For revision C single-link processors
#define BLDCFG_SUPPORT_ACPI_PSTATES_PSD_INDPX  TRUE

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
#define AGESA_ENTRY_INIT_GENERAL_SERVICES         TRUE

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
                  // This is the delivery package title, "LlanoPI "
                  // This string MUST be exactly 8 characters long
#define AGESA_PACKAGE_STRING  {'L', 'l', 'a', 'n', 'o', 'P', 'I', ' '}

                  // This is the release version number of the AGESA component
                  // This string MUST be exactly 12 characters long
#define AGESA_VERSION_STRING  {'V', '1', '.', '1', '.', '0', '.', '0', ' ', ' ', ' ', ' '}

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

/* Build configuration values here.
 */
#define BLDCFG_VRM_CURRENT_LIMIT                 65000  //240000 //120000
#define BLDCFG_VRM_LOW_POWER_THRESHOLD           15000  // 0
#define BLDCFG_VRM_INRUSH_CURRENT_LIMIT          0
#define BLDCFG_PLAT_NUM_IO_APICS                 3
#define BLDCFG_CORE_LEVELING_MODE                CORE_LEVEL_LOWEST
#define BLDCFG_MEM_INIT_PSTATE                   0

#define BLDCFG_AMD_PLATFORM_TYPE                  AMD_PLATFORM_MOBILE

#define BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT         DDR1866_FREQUENCY //DDR1066_FREQUENCY
#define BLDCFG_MEMORY_MODE_UNGANGED               TRUE
#define BLDCFG_MEMORY_QUAD_RANK_CAPABLE           TRUE
#define BLDCFG_MEMORY_QUADRANK_TYPE               QUADRANK_REGISTERED
#define BLDCFG_MEMORY_RDIMM_CAPABLE               TRUE
#define BLDCFG_MEMORY_UDIMM_CAPABLE               TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE              TRUE
#define BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING    TRUE
#define BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING    FALSE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING        TRUE
#define BLDCFG_MEMORY_POWER_DOWN                  TRUE
#define BLDCFG_POWER_DOWN_MODE                    POWER_DOWN_BY_CHIP_SELECT
#define BLDCFG_ONLINE_SPARE                       FALSE
#define BLDCFG_MEMORY_PARITY_ENABLE               FALSE
#define BLDCFG_BANK_SWIZZLE                       TRUE
#define BLDCFG_TIMING_MODE_SELECT                 TIMING_MODE_AUTO
#define BLDCFG_MEMORY_CLOCK_SELECT                DDR800_FREQUENCY
#define BLDCFG_DQS_TRAINING_CONTROL               TRUE
#define BLDCFG_IGNORE_SPD_CHECKSUM                FALSE
#define BLDCFG_USE_BURST_MODE                     FALSE
#define BLDCFG_MEMORY_ALL_CLOCKS_ON               FALSE
#define BLDCFG_ENABLE_ECC_FEATURE                 TRUE
#define BLDCFG_ECC_REDIRECTION                    FALSE
#define BLDCFG_SCRUB_DRAM_RATE                    0
#define BLDCFG_SCRUB_L2_RATE                      0
#define BLDCFG_SCRUB_L3_RATE                      0
#define BLDCFG_SCRUB_IC_RATE                      0
#define BLDCFG_SCRUB_DC_RATE                      0
#define BLDCFG_ECC_SYNC_FLOOD                     FALSE
#define BLDCFG_ECC_SYMBOL_SIZE                    4
#define BLDCFG_HEAP_DRAM_ADDRESS                  0xB0000
#define BLDCFG_1GB_ALIGN                          FALSE
#define BLDCFG_VRM_HIGH_SPEED_ENABLE              TRUE
//#define BLDCFG_PROCESSOR_SCOPE_NAME0              'C'
//#define BLDCFG_PROCESSOR_SCOPE_NAME1              '0'

//enable HW C1E
#define BLDCFG_PLATFORM_C1E_MODE     0 //C1eModeHardware
//#define BLDCFG_PLATFORM_C1E_OPDATA              0x415
#define BLDCFG_PLATFORM_CSTATE_MODE     CStateModeC6 //0 //CStateModeC6
//#define BLDCFG_PLATFORM_CSTATE_OPDATA   0x840         //Specifies a free block of 8 consecutive I/O ports to be used to place the CPU into C6
#define BLDCFG_PLATFORM_CSTATE_IO_BASE_ADDRESS   0x840         //Specifies a free block of 8 consecutive I/O ports to be used to place the CPU into C6


//#define BLDCFG_VRM_NB_CURRENT_LIMIT               0  // Not currently used on Llano/Ontario
#define BLDCFG_VRM_NB_LOW_POWER_THRESHOLD         1  // Zero - disable NBPSI_L, Non-zero - enable NBPSI_L.  Default is Zero.
//#define BLDCFG_VRM_NB_SLEW_RATE                   5000 // Used in calculating the VSRampSlamTime per BKDG.  Defaults to 5000, same as core VRM.  Cannot be zero.
//#define BLDCFG_VRM_NB_ADDITIONAL_DELAY            0  // Not currently used on Llano/Ontario
//#define BLDCFG_VRM_NB_HIGH_SPEED_ENABLE           0  // Not currently used on Llano/Ontario
//#define BLDCFG_VRM_NB_INRUSH_CURRENT_LIMIT        0  // Not currently used on Llano/Ontario

#define BLDCFG_UMA_ABOVE4G_SUPPORT              TRUE
#define BLDCFG_STEREO_3D_PINOUT                 TRUE

/*  Process the options...
 * This file include MUST occur AFTER the user option selection settings
 */
CONST AP_MTRR_SETTINGS ROMDATA LlanoApMtrrSettingsList[] =
{
  { AMD_AP_MTRR_FIX64k_00000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX16k_80000, 0x1E1E1E1E1E1E1E1Eull },
  { AMD_AP_MTRR_FIX16k_A0000, 0x0000000000000000ull },
  { AMD_AP_MTRR_FIX4k_C0000, 0x0000000000000000ull },
  { AMD_AP_MTRR_FIX4k_C8000, 0x0000000000000000ull },
  { AMD_AP_MTRR_FIX4k_D0000, 0x0000000000000000ull },
  { AMD_AP_MTRR_FIX4k_D8000, 0x0000000000000000ull },
  { AMD_AP_MTRR_FIX4k_E0000, 0x1818181818181818ull },
  { AMD_AP_MTRR_FIX4k_E8000, 0x1818181818181818ull },
  { AMD_AP_MTRR_FIX4k_F0000, 0x1818181818181818ull },
  { AMD_AP_MTRR_FIX4k_F8000, 0x1818181818181818ull },
  { CPU_LIST_TERMINAL }
};

#define BLDCFG_AP_MTRR_SETTINGS_LIST &LlanoApMtrrSettingsList
//#define OPTION_NB_LCLK_DPM_INIT FALSE
//#define OPTION_POWER_GATE       FALSE
//#define OPTION_PCIE_POWER_GATE  FALSE
//#define OPTION_ALIB             FALSE
//#define OPTION_PCIe_MID_INIT  FALSE
//#define OPTION_NB_MID_INIT    FALSE

#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "AdvancedApi.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "cpuFeatures.h"
#include "Table.h"
#include "CommonReturns.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "GnbInterface.h"
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
  NUMBER_OF_DIMMS_SUPPORTED (ANY_SOCKET, ANY_CHANNEL, 1),
  NUMBER_OF_CHANNELS_SUPPORTED (ANY_SOCKET, 2),
  PSO_END
};

/*
 * These tables are optional and may be used to adjust memory timing settings
 */
#include "mm.h"
#include "mn.h"

//DA Customer table
UINT8 AGESA_MEM_TABLE_LN[][sizeof (MEM_TABLE_ALIAS)] =
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
  // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM0, BFWrDatDly, MTOverride, 0x15, 0x14, 0x21, 0x11, 0x40, 0x2A, 0x34, 0x2D, 0x15),// DCT0, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM1, BFWrDatDly, MTOverride, 0x1D, 0x00, 0x06, 0x0B, 0x17, 0x1A, 0x1E, 0x1F, 0x10),// DCT0, DIMM1
  // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM0, BFWrDatDly, MTOverride, 0x17, 0x16, 0x21, 0x11, 0x3F, 0x2A, 0x35, 0x2E, 0x17),// DCT1, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM1, BFWrDatDly, MTOverride, 0x18, 0x1D, 0x1C, 0x0B, 0x17, 0x1A, 0x1D, 0x1C, 0x10),// DCT1, DIMM1

 // DQS Receiver Enable
  // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct0, MTDIMM0, BFRcvEnDly, MTOverride, 0x77, 0x70, 0x77, 0x60, 0x95, 0x83, 0x8F, 0x90, 0x77),// DCT0, DIMM0
 // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct0, MTDIMM1, BFRcvEnDly, MTOverride, 0x7C, 0x7D, 0x7E, 0x81, 0x88, 0x8F, 0x96, 0x9F, 0x84),// DCT0, DIMM1
  // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct1, MTDIMM0, BFRcvEnDly, MTOverride, 0x7D, 0x75, 0x7F, 0x6C, 0x9A, 0x8D, 0x94, 0x98, 0x7D),// DCT1, DIMM0
 // DQSACCESS(MTAfterSwRxEnTrn, MTNode0, MTDct1, MTDIMM1, BFRcvEnDly, MTOverride, 0x1C, 0x1D, 0x1E, 0x01, 0x08, 0x0F, 0x16, 0x1F, 0x04),// DCT1, DIMM1

 // Write DQS Delays
  // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct0, MTDIMM0, BFWrDqsDly, MTOverride, 0x03, 0x04, 0x0F, 0x00, 0x2D, 0x1B, 0x23, 0x1C, 0x00),// DCT0, DIMM0
 // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct0, MTDIMM1, BFWrDqsDly, MTOverride, 0x06, 0x0D, 0x12, 0x1A, 0x25, 0x28, 0x2C, 0x2C, 0x44),// DCT0, DIMM1
  // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct1, MTDIMM0, BFWrDqsDly, MTOverride, 0x05, 0x05, 0x0F, 0x00, 0x2E, 0x19, 0x24, 0x1C, 0x00),// DCT1, DIMM0
 // DQSACCESS(MTAfterHwWLTrnP2, MTNode0, MTDct1, MTDIMM1, BFWrDqsDly, MTOverride, 0x07, 0x0C, 0x14, 0x19, 0x25, 0x28, 0x2B, 0x2B, 0x1A),// DCT1, DIMM1

 // Read DQS Delays
  // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM0, BFRdDqsDly, MTOverride, 0x0C, 0x0A, 0x0A, 0x0E, 0x0A, 0x0C, 0x0C, 0x0A, 0x0C),// DCT0, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct0, MTDIMM1, BFRdDqsDly, MTOverride, 0x10, 0x10, 0x0E, 0x10, 0x10, 0x10, 0x10, 0x1E, 0x10),// DCT0, DIMM1
  // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM0, BFRdDqsDly, MTOverride, 0x0A, 0x0C, 0x0E, 0x0A, 0x0C, 0x0A, 0x0C, 0x0E, 0x0A),// DCT1, DIMM0
 // DQSACCESS(MTAfterDqsRwPosTrn, MTNode0, MTDct1, MTDIMM1, BFRdDqsDly, MTOverride, 0x10, 0x10, 0x0E, 0x10, 0x10, 0x10, 0x10, 0x1E, 0x10),// DCT1, DIMM1
 // --------------------------------------------------------------------------------------------------------------------------------------------------

 // MaxRdLatency
  // NBACCESS (MTAfterMaxRdLatTrn, MTNode0, MTDct0, BFMaxLatency, MTOverride, 0x0C),
  // NBACCESS (MTAfterMaxRdLatTrn, MTNode0, MTDct1, BFMaxLatency, MTOverride, 0x0C),
 // TABLE END
  NBACCESS (MTEnd, 0,  0, 0, 0, 0),      // End of Table
};
UINT8 SizeOfTableLN = sizeof (AGESA_MEM_TABLE_LN) / sizeof (AGESA_MEM_TABLE_LN[0]);

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


