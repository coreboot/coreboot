/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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

#include "AGESA.h"
#include "CommonReturns.h"
#include "AdvancedApi.h"
#include <PlatformMemoryConfiguration.h>
#include "Filecode.h"
#define FILECODE PLATFORM_SPECIFIC_OPTIONS_FILECODE

/* AGESA will check the OEM configuration during preprocessing stage,
 * coreboot enable -Wundef option, so we should make sure we have all contanstand defined
 */
/* MEMORY_BUS_SPEED */
#define  DDR400_FREQUENCY		200	///< DDR 400
#define  DDR533_FREQUENCY		266	///< DDR 533
#define  DDR667_FREQUENCY		333	///< DDR 667
#define  DDR800_FREQUENCY		400	///< DDR 800
#define  DDR1066_FREQUENCY		533	///< DDR 1066
#define  DDR1333_FREQUENCY		667	///< DDR 1333
#define  DDR1600_FREQUENCY		800	///< DDR 1600
#define  DDR1866_FREQUENCY		933	///< DDR 1866
#define  UNSUPPORTED_DDR_FREQUENCY	934	///< Highest limit of DDR frequency

/* QUANDRANK_TYPE*/
#define QUADRANK_REGISTERED		0	///< Quadrank registered DIMM
#define QUADRANK_UNBUFFERED		1	///< Quadrank unbuffered DIMM

/* USER_MEMORY_TIMING_MODE */
#define TIMING_MODE_AUTO		0	///< Use best rate possible
#define TIMING_MODE_LIMITED		1	///< Set user top limit
#define TIMING_MODE_SPECIFIC		2	///< Set user specified speed

/* POWER_DOWN_MODE */
#define POWER_DOWN_BY_CHANNEL		0	///< Channel power down mode
#define POWER_DOWN_BY_CHIP_SELECT	1	///< Chip select power down mode

/* User makes option selections here
 * Comment out the items wanted to be included in the build.
 * Uncomment those items you with to REMOVE from the build.
 */
//#define BLDOPT_REMOVE_UDIMMS_SUPPORT           TRUE
//#define BLDOPT_REMOVE_RDIMMS_SUPPORT           TRUE
//#define BLDOPT_REMOVE_ECC_SUPPORT              TRUE
//#define BLDOPT_REMOVE_BANK_INTERLEAVE          TRUE
//#define BLDOPT_REMOVE_DCT_INTERLEAVE           TRUE
//#define BLDOPT_REMOVE_NODE_INTERLEAVE          TRUE
//#define BLDOPT_REMOVE_PARALLEL_TRAINING        TRUE
//#define BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT     TRUE
#define BLDOPT_REMOVE_MEM_RESTORE_SUPPORT      TRUE
//#define BLDOPT_REMOVE_MULTISOCKET_SUPPORT      TRUE
////#define BLDOPT_REMOVE_ACPI_PSTATES             TRUE
////#define BLDOPT_REMOVE_SRAT                     TRUE
////#define BLDOPT_REMOVE_SLIT                     TRUE
//#define BLDOPT_REMOVE_WHEA                     TRUE
//#define BLDOPT_REMOVE_DMI                      TRUE

/*f15 Rev A1 ucode patch CpuF15OrMicrocodePatch0600011F */
#define BLDOPT_REMOVE_EARLY_SAMPLES		FALSE

//#define BLDOPT_REMOVE_HT_ASSIST                TRUE
//#define BLDOPT_REMOVE_MSG_BASED_C1E            TRUE
//#define BLDCFG_REMOVE_ACPI_PSTATES_PPC               TRUE
//#define BLDCFG_REMOVE_ACPI_PSTATES_PCT               TRUE
//#define BLDCFG_REMOVE_ACPI_PSTATES_PSD               TRUE
//#define BLDCFG_REMOVE_ACPI_PSTATES_PSS               TRUE
//#define BLDCFG_REMOVE_ACPI_PSTATES_XPSS              TRUE

/* Build configuration values here.
 */
#define BLDCFG_VRM_CURRENT_LIMIT                 120000
#define BLDCFG_VRM_LOW_POWER_THRESHOLD           0
#define BLDCFG_VRM_INRUSH_CURRENT_LIMIT          0
#define BLDCFG_PLAT_NUM_IO_APICS                 3
#define BLDCFG_CORE_LEVELING_MODE                CORE_LEVEL_LOWEST
#define BLDCFG_MEM_INIT_PSTATE                   0
#define BLDCFG_AMD_PSTATE_CAP_VALUE              0

#define BLDCFG_AMD_PLATFORM_TYPE                  AMD_PLATFORM_SERVER

#define BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT         DDR1333_FREQUENCY//1600
#define BLDCFG_MEMORY_MODE_UNGANGED               TRUE
#define BLDCFG_MEMORY_QUAD_RANK_CAPABLE           TRUE
#define BLDCFG_MEMORY_QUADRANK_TYPE               QUADRANK_REGISTERED
#define BLDCFG_MEMORY_RDIMM_CAPABLE               TRUE
#define BLDCFG_MEMORY_UDIMM_CAPABLE               TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE              FALSE
#define BLDCFG_LIMIT_MEMORY_TO_BELOW_1TB          TRUE
#define BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING    FALSE//TRUE
#define BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING    FALSE//TRUE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING        FALSE//TRUE
#define BLDCFG_MEMORY_POWER_DOWN                  FALSE
#define BLDCFG_POWER_DOWN_MODE                    POWER_DOWN_BY_CHANNEL
#define BLDCFG_ONLINE_SPARE                       FALSE
#define BLDCFG_BANK_SWIZZLE                       TRUE
#define BLDCFG_TIMING_MODE_SELECT                 TIMING_MODE_AUTO
#define BLDCFG_MEMORY_CLOCK_SELECT                DDR1333_FREQUENCY //DDR800_FREQUENCY
#define BLDCFG_DQS_TRAINING_CONTROL               TRUE
#define BLDCFG_IGNORE_SPD_CHECKSUM                FALSE
#define BLDCFG_USE_BURST_MODE                     FALSE
#define BLDCFG_MEMORY_ALL_CLOCKS_ON               FALSE
#define BLDCFG_ENABLE_ECC_FEATURE                 TRUE
#define BLDCFG_ECC_REDIRECTION                    FALSE
#define BLDCFG_SCRUB_IC_RATE                      0
#define BLDCFG_ECC_SYNC_FLOOD                     TRUE
#define BLDCFG_ECC_SYMBOL_SIZE                    4

#define BLDCFG_PCI_MMIO_BASE                    CONFIG_MMCONF_BASE_ADDRESS
#define BLDCFG_PCI_MMIO_SIZE                    CONFIG_MMCONF_BUS_NUMBER

/**
 * Enable Message Based C1e CPU feature in multi-socket systems.
 * BLDCFG_PLATFORM_C1E_OPDATA element be defined with a valid IO port value,
 * else the feature cannot be enabled.
 */
#define BLDCFG_PLATFORM_C1E_MODE                  C1eModeMsgBased
#define BLDCFG_PLATFORM_C1E_OPDATA                0x80//TODO
//#define BLDCFG_PLATFORM_C1E_MODE_OPDATA1        0
//#define BLDCFG_PLATFORM_C1E_MODE_OPDATA2        0

#define BLDCFG_HEAP_DRAM_ADDRESS                  0xB0000
#define BLDCFG_CFG_TEMP_PCIE_MMIO_BASE_ADDRESS    0xD0000000
#define BLDCFG_1GB_ALIGN                          FALSE
//#define BLDCFG_PROCESSOR_SCOPE_NAME0              'C'
//#define BLDCFG_PROCESSOR_SCOPE_NAME1              '0'
//

// Select the platform control flow mode for performance tuning.
#define BLDCFG_PLATFORM_CONTROL_FLOW_MODE Nfcm

/**
 * Enable the probe filtering performance tuning feature.
 * The probe filter provides filtering of broadcast probes to
 * improve link bandwidth and performance for multi- node systems.
 *
 * This feature may interact with other performance features.
 *  TRUE  -Enable the feature (default) if supported by all processors,
 *         based on revision and presence of L3 cache.
 *         The feature is not enabled if there are no coherent HT links.
 *  FALSE -Do not enable the feature regardless of the configuration.
 */
//TODO enable it,
//but AGESA set PFMode = 0; //PF Disable, HW never set PFInitDone
//hang in F10HtAssistInit() do{...} while(PFInitDone != 1)
#define BLDCFG_USE_HT_ASSIST	FALSE

/**
 * The socket and link match values are platform specific
 */
CONST MANUAL_BUID_SWAP_LIST ROMDATA s8226_manual_swaplist[2] =
{
	{
		/* On the reference platform, there is only one nc chain, so socket & link are 'don't care' */
		HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY,

		{ //BUID Swap List
			{ //BUID Swaps
				/* Each Non-coherent chain may have a list of device swaps,
				 * Each item specify a device will be swap from its current id to a new one
				 */
				/* FromID 0x00 is the chain with the southbridge */
				/* 'Move' device zero to device zero, All others are non applicable */
				{0x00, 0x00}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
				{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
				{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
				{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
				{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
				{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
				{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
				{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF},
			},

			{ //The ordered final BUIDs
				/* Specify the final BUID to be zero, All others are non applicable */
				0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			}
		}
	},

	/* The 2nd element in the array merely terminates the list */
	{
		HT_LIST_TERMINAL,
	}
};

#if CONFIG_HT3_SUPPORT
/**
 * The socket and link match values are platform specific
 *
 */
CONST CPU_TO_CPU_PCB_LIMITS ROMDATA s8226_cpu2cpu_limit_list[2] =
{
	{
		/* On the reference platform, these settings apply to all coherent links */
		HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY,

		/* Set incoming and outgoing links to 16 bit widths, and 3.2GHz frequencies */
		HT_WIDTH_16_BITS, HT_WIDTH_16_BITS, HT_FREQUENCY_LIMIT_3200M,
	},

	/* The 2nd element in the array merely terminates the list */
	{
		HT_LIST_TERMINAL,
	}
};

CONST IO_PCB_LIMITS ROMDATA s8226_io_limit_list[2] =
{
	{
		/* On the reference platform, there is only one nc chain, so socket & link are 'don't care' */
		HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY,

		/* Set upstream and downstream links to 16 bit widths, and limit frequencies to 3.2GHz */
		HT_WIDTH_16_BITS, HT_WIDTH_16_BITS, HT_FREQUENCY_LIMIT_3200M, //Actually IO hub only support 2600M MAX
	},

	/* The 2nd element in the array merely terminates the list */
	{
		HT_LIST_TERMINAL,
	}
};
#else //CONFIG_HT3_SUPPORT == 0
CONST CPU_TO_CPU_PCB_LIMITS ROMDATA s8226_cpu2cpu_limit_list[2] =
{
	{
		/* On the reference platform, these settings apply to all coherent links */
		HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY,

		/* Set incoming and outgoing links to 16 bit widths, and 1GHz frequencies */
		HT_WIDTH_16_BITS, HT_WIDTH_16_BITS, HT_FREQUENCY_LIMIT_HT1_ONLY,
	},

	/* The 2nd element in the array merely terminates the list */
	{
		HT_LIST_TERMINAL,
	}
};

CONST IO_PCB_LIMITS ROMDATA s8226_io_limit_list[2] =
{
	{
		/* On the reference platform, there is only one nc chain, so socket & link are 'don't care' */
		HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY,

		/* Set incoming and outgoing links to 16 bit widths, and 1GHz frequencies */
		HT_WIDTH_16_BITS, HT_WIDTH_16_BITS, HT_FREQUENCY_LIMIT_HT1_ONLY,
	},

	/* The 2nd element in the array merely terminates the list */
	{
		HT_LIST_TERMINAL
	}
};
#endif //CONFIG_HT3_SUPPORT == 0

/**
 * HyperTransport links will typically require an equalization at high frequencies.
 * This is called deemphasis.
 *
 * Deemphasis is specified as levels, for example, -3 db.
 * There are two levels for each link, its receiver deemphasis level and its DCV level,
 * which is based on the far side transmitter's deemphasis.
 * For each link, different levels may be required at each link frequency.
 *
 * Coherent connections between processors should have an entry for the port on each processor.
 * There should be one entry for the host root port of each non-coherent chain.
 *
 * AGESA initialization code does not set deemphasis on IO Devices.
 * A default is provided for internal links of MCM processors, and
 * those links will generally not need deemphasis structures.
 */
CONST CPU_HT_DEEMPHASIS_LEVEL ROMDATA s8226_deemphasis_list[] =
{
	/* Socket, Link, LowFreq, HighFreq, Receiver Deemphasis, Dcv Deemphasis */

	/* Non-coherent link deemphasis. */
	{0, 2, HT3_FREQUENCY_MIN, HT_FREQUENCY_1600M, DeemphasisLevelNone, DcvLevelNone},
	{0, 2, HT_FREQUENCY_1800M, HT_FREQUENCY_1800M, DeemphasisLevelMinus3, DcvLevelMinus5},
	{0, 2, HT_FREQUENCY_2000M, HT_FREQUENCY_2000M, DeemphasisLevelMinus6, DcvLevelMinus5},
	{0, 2, HT_FREQUENCY_2200M, HT_FREQUENCY_2200M, DeemphasisLevelMinus6, DcvLevelMinus7},
	{0, 2, HT_FREQUENCY_2400M, HT_FREQUENCY_2400M, DeemphasisLevelMinus8, DcvLevelMinus7},
	{0, 2, HT_FREQUENCY_2600M, HT_FREQUENCY_2600M, DeemphasisLevelMinus11pre8, DcvLevelMinus9},

	{1, 2, HT3_FREQUENCY_MIN, HT_FREQUENCY_1600M, DeemphasisLevelNone, DcvLevelNone},
	{1, 2, HT_FREQUENCY_1800M, HT_FREQUENCY_1800M, DeemphasisLevelMinus3, DcvLevelMinus5},
	{1, 2, HT_FREQUENCY_2000M, HT_FREQUENCY_2000M, DeemphasisLevelMinus6, DcvLevelMinus5},
	{1, 2, HT_FREQUENCY_2200M, HT_FREQUENCY_2200M, DeemphasisLevelMinus6, DcvLevelMinus7},
	{1, 2, HT_FREQUENCY_2400M, HT_FREQUENCY_2400M, DeemphasisLevelMinus8, DcvLevelMinus7},
	{1, 2, HT_FREQUENCY_2600M, HT_FREQUENCY_2600M, DeemphasisLevelMinus11pre8, DcvLevelMinus9},

	{2, 0, HT3_FREQUENCY_MIN, HT_FREQUENCY_1600M, DeemphasisLevelNone, DcvLevelNone},
	{2, 0, HT_FREQUENCY_1800M, HT_FREQUENCY_1800M, DeemphasisLevelMinus3, DcvLevelMinus5},
	{2, 0, HT_FREQUENCY_2000M, HT_FREQUENCY_2000M, DeemphasisLevelMinus6, DcvLevelMinus5},
	{2, 0, HT_FREQUENCY_2200M, HT_FREQUENCY_2200M, DeemphasisLevelMinus6, DcvLevelMinus7},
	{2, 0, HT_FREQUENCY_2400M, HT_FREQUENCY_2400M, DeemphasisLevelMinus8, DcvLevelMinus7},
	{2, 0, HT_FREQUENCY_2600M, HT_FREQUENCY_2600M, DeemphasisLevelMinus11pre8, DcvLevelMinus9},

	{3, 0, HT3_FREQUENCY_MIN, HT_FREQUENCY_1600M, DeemphasisLevelNone, DcvLevelNone},
	{3, 0, HT_FREQUENCY_1800M, HT_FREQUENCY_1800M, DeemphasisLevelMinus3, DcvLevelMinus5},
	{3, 0, HT_FREQUENCY_2000M, HT_FREQUENCY_2000M, DeemphasisLevelMinus6, DcvLevelMinus5},
	{3, 0, HT_FREQUENCY_2200M, HT_FREQUENCY_2200M, DeemphasisLevelMinus6, DcvLevelMinus7},
	{3, 0, HT_FREQUENCY_2400M, HT_FREQUENCY_2400M, DeemphasisLevelMinus8, DcvLevelMinus7},
	{3, 0, HT_FREQUENCY_2600M, HT_FREQUENCY_2600M, DeemphasisLevelMinus11pre8, DcvLevelMinus9},

	/* Coherent link deemphasis. */
	{HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT3_FREQUENCY_MIN, HT_FREQUENCY_1600M, DeemphasisLevelNone, DcvLevelNone},
	{HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_FREQUENCY_1800M, HT_FREQUENCY_1800M, DeemphasisLevelMinus3, DcvLevelMinus3},
	{HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_FREQUENCY_2000M, HT_FREQUENCY_2000M, DeemphasisLevelMinus6, DcvLevelMinus6},
	{HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_FREQUENCY_2200M, HT_FREQUENCY_2200M, DeemphasisLevelMinus6, DcvLevelMinus6},
	{HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_FREQUENCY_2400M, HT_FREQUENCY_2400M, DeemphasisLevelMinus8, DcvLevelMinus8},
	{HT_LIST_MATCH_ANY, HT_LIST_MATCH_ANY, HT_FREQUENCY_2600M, HT_FREQUENCY_MAX, DeemphasisLevelMinus11pre8, DcvLevelMinus11},

	/* End of the list */
	{
		HT_LIST_TERMINAL
	}
};

/**
 * For systems using socket infrastructure that permits strapping the SBI
 * address for each socket, this should be used to provide a socket ID value.
 * This is referred to as the hardware method for socket naming, and is the
 * preferred solution.
 */
/*
 * I do NOT know howto config socket id in simnow,
 * so use this software way to make HT works in simnow,
 * real hardware do not need this Socket Map.
 *
 * A physical socket map for a 4 G34 Sockets MCM processors topology,
 * reference the mainboard schemantic in detail.
 *
 */
CONST SYSTEM_PHYSICAL_SOCKET_MAP ROMDATA s8226_socket_map[] =
{
#define HT_SOCKET0	0
#define HT_SOCKET1	1
#define HT_SOCKET2	2
#define HT_SOCKET3	3

/**
 * 0-3 are sublink 0, 4-7 are sublink 1
 */
#define HT_LINK0A	0
#define HT_LINK1A	1
#define HT_LINK2A	2
#define HT_LINK3A	3
#define HT_LINK0B	4
#define HT_LINK1B	5
#define HT_LINK2B	6
#define HT_LINK3B	7

	/* Source Socket, Link, Target Socket */
/*	{HT_SOCKET0, HT_LINK0A, HT_SOCKET1},
	{HT_SOCKET0, HT_LINK0B, HT_SOCKET3},
	{HT_SOCKET0, HT_LINK1A, HT_SOCKET1},
	{HT_SOCKET0, HT_LINK1B, HT_SOCKET3},
	{HT_SOCKET0, HT_LINK3A, HT_SOCKET2},
	{HT_SOCKET0, HT_LINK3B, HT_SOCKET2},

	{HT_SOCKET1, HT_LINK0A, HT_SOCKET2},
	{HT_SOCKET1, HT_LINK0B, HT_SOCKET3},
	{HT_SOCKET1, HT_LINK1A, HT_SOCKET0},
	{HT_SOCKET1, HT_LINK1B, HT_SOCKET2},
	{HT_SOCKET1, HT_LINK3A, HT_SOCKET0},
	{HT_SOCKET1, HT_LINK3B, HT_SOCKET3},

	{HT_SOCKET2, HT_LINK0A, HT_SOCKET3},
	{HT_SOCKET2, HT_LINK0B, HT_SOCKET0},
	{HT_SOCKET2, HT_LINK1A, HT_SOCKET3},
	{HT_SOCKET2, HT_LINK1B, HT_SOCKET1},
	{HT_SOCKET2, HT_LINK3A, HT_SOCKET1},
	{HT_SOCKET2, HT_LINK3B, HT_SOCKET0},

	{HT_SOCKET3, HT_LINK0A, HT_SOCKET2},
	{HT_SOCKET3, HT_LINK0B, HT_SOCKET1},
	{HT_SOCKET3, HT_LINK1A, HT_SOCKET1},
	{HT_SOCKET3, HT_LINK1B, HT_SOCKET0},
	{HT_SOCKET3, HT_LINK3A, HT_SOCKET0},
	{HT_SOCKET3, HT_LINK3B, HT_SOCKET2}, */
};

CONST AP_MTRR_SETTINGS ROMDATA s8226_ap_mtrr_list[] =
{
	{AMD_AP_MTRR_FIX64k_00000, 0x1E1E1E1E1E1E1E1E},
	{AMD_AP_MTRR_FIX16k_80000, 0x1E1E1E1E1E1E1E1E},
	{AMD_AP_MTRR_FIX16k_A0000, 0x0000000000000000},
	{AMD_AP_MTRR_FIX4k_C0000,  0x0000000000000000},
	{AMD_AP_MTRR_FIX4k_C8000,  0x0000000000000000},
	{AMD_AP_MTRR_FIX4k_D0000,  0x0000000000000000},
	{AMD_AP_MTRR_FIX4k_D8000,  0x0000000000000000},
	{AMD_AP_MTRR_FIX4k_E0000,  0x1818181818181818},
	{AMD_AP_MTRR_FIX4k_E8000,  0x1818181818181818},
	{AMD_AP_MTRR_FIX4k_F0000,  0x1818181818181818},
	{AMD_AP_MTRR_FIX4k_F8000,  0x1818181818181818},
	{CPU_LIST_TERMINAL}
};

#define BLDCFG_BUID_SWAP_LIST			&s8226_manual_swaplist
#define BLDCFG_HTFABRIC_LIMITS_LIST		&s8226_cpu2cpu_limit_list
#define BLDCFG_HTCHAIN_LIMITS_LIST		&s8226_io_limit_list
#define BLDCFG_PLATFORM_DEEMPHASIS_LIST		&s8226_deemphasis_list
#define BLDCFG_AP_MTRR_SETTINGS_LIST		&s8226_ap_mtrr_list
//#define BLDCFG_SYSTEM_PHYSICAL_SOCKET_MAP	&s8226_socket_map


/*  Process the options...
 * This file include MUST occur AFTER the user option selection settings
 */
#define AGESA_ENTRY_INIT_RESET                    TRUE//FALSE
#define AGESA_ENTRY_INIT_RECOVERY                 FALSE
#define AGESA_ENTRY_INIT_EARLY                    TRUE
#define AGESA_ENTRY_INIT_POST                     TRUE
#define AGESA_ENTRY_INIT_ENV                      TRUE
#define AGESA_ENTRY_INIT_MID                      TRUE
#define AGESA_ENTRY_INIT_LATE                     TRUE
#define AGESA_ENTRY_INIT_S3SAVE                   TRUE
#define AGESA_ENTRY_INIT_RESUME                   TRUE
#define AGESA_ENTRY_INIT_LATE_RESTORE             TRUE
#define AGESA_ENTRY_INIT_GENERAL_SERVICES         TRUE

/*
#if CONFIG_CPU_AMD_AGESA_FAMILY15
  #define BLDOPT_REMOVE_FAMILY_10_SUPPORT         TRUE
#endif
#if CONFIG_CPU_AMD_AGESA_FAMILY10
  #define BLDOPT_REMOVE_FAMILY_15_SUPPORT         TRUE
#endif
*/

//#include "GnbInterface.h"     /*prototype for GnbInterfaceStub*/
#include "SanMarinoInstall.h"

/*----------------------------------------------------------------------------------------
 *                        CUSTOMER OVERIDES MEMORY TABLE
 *----------------------------------------------------------------------------------------
 */

//reference BKDG Table87: works
#define F15_WL_SEED 0x3B //family15 BKDG recommand 3B RDIMM, 1A UDIMM.
#define SEED_A 0x54
#define SEED_B 0x4D
#define SEED_C 0x45
#define SEED_D 0x40

#define F10_WL_SEED 0x3B //family10 BKDG recommand 3B RDIMM, 1A UDIMM.
//4B 41 51

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

	/* Specifies the write leveling seed for a channel of a socket.
	 * WRITE_LEVELING_SEED(SocketID, ChannelID, DimmID,
	 *                     Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed,
	 *                     Byte4Seed, Byte5Seed, Byte6Seed, Byte7Seed,
	 *                     ByteEccSeed)
	 */
	WRITE_LEVELING_SEED(
			ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS,
			F15_WL_SEED, F15_WL_SEED, F15_WL_SEED, F15_WL_SEED,
			F15_WL_SEED, F15_WL_SEED, F15_WL_SEED, F15_WL_SEED,
			F15_WL_SEED),

	/* HW_RXEN_SEED(SocketID, ChannelID, DimmID,
	 *              Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed,
	 *              Byte4Seed, Byte5Seed, Byte6Seed, Byte7Seed, ByteEccSeed)
	 */
	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_A, ALL_DIMMS,
		SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A,
		SEED_A),
	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_B, ALL_DIMMS,
		SEED_B, SEED_B, SEED_B, SEED_B, SEED_B, SEED_B, SEED_B, SEED_B,
		SEED_B),
	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_C, ALL_DIMMS,
		SEED_C, SEED_C, SEED_C, SEED_C, SEED_C, SEED_C, SEED_C, SEED_C,
		SEED_C),
	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_D, ALL_DIMMS,
		SEED_D, SEED_D, SEED_D, SEED_D, SEED_D, SEED_D, SEED_D, SEED_D,
		SEED_D),

	NUMBER_OF_DIMMS_SUPPORTED (ANY_SOCKET, ANY_CHANNEL, 3), //max 3
	PSO_END
};

/*
 * These tables are optional and may be used to adjust memory timing settings
 */
//HY Customer table
UINT8 AGESA_MEM_TABLE_HY[][sizeof (MEM_TABLE_ALIAS)] =
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
UINT8 SizeOfTableHy = sizeof (AGESA_MEM_TABLE_HY) / sizeof (AGESA_MEM_TABLE_HY[0]);

