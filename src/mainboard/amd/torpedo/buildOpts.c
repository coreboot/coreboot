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
 */

#include <stdlib.h>
#include "AGESA.h"
#include "Filecode.h"
#define FILECODE PLATFORM_SPECIFIC_OPTIONS_FILECODE


/*  Select the CPU family.  */
#define INSTALL_FAMILY_10_SUPPORT FALSE
#define INSTALL_FAMILY_12_SUPPORT TRUE
#define INSTALL_FAMILY_14_SUPPORT FALSE
#define INSTALL_FAMILY_15_SUPPORT FALSE

/*  Select the CPU socket type.  */
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

/* The following definitions specify the default values for various parameters
 * in which there are no clearly defined defaults to be used in the common file.
 * The values below are based on product and BKDG content, please consult the
 * AGESA Memory team for consultation.
 */
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
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "GnbInterface.h"
#include "PlatformInstall.h"
