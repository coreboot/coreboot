/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "amdfwtool.h"

/* This file contains the SoC quirks. */

#define PLATFORM_MAX_PSP_IDS 4

struct platform_info {
	const char *name;
	bool needs_ish;
	bool is_multi_level;
	bool has_apob_nv_quirk;
	bool is_initial_alignment_required;
	bool is_second_gen;
	bool has_dir_header_v1;
	uint32_t num_psp_ids;
	uint32_t psp_ids[PLATFORM_MAX_PSP_IDS];
	uint32_t psb_reserved_size;
};

static const struct platform_info platform_table[] = {
	[PLATFORM_MULLINS] = {
		.name = "Mullins",
		.needs_ish = false,
		.is_multi_level = false,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = false,
		.has_dir_header_v1 = false,
		.num_psp_ids = 0,
		.psp_ids = {0},
		.psb_reserved_size = 0,
	},
	[PLATFORM_CARRIZO] = {
		.name = "Carrizo",
		.needs_ish = false,
		.is_multi_level = false,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = false,
		.has_dir_header_v1 = false,
		.num_psp_ids = 0,
		.psp_ids = {0},
		.psb_reserved_size = 0,
	},
	[PLATFORM_STONEYRIDGE] = {
		.name = "Stoneyridge",
		.needs_ish = false,
		.is_multi_level = false,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = false,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0x10220B00},
		.psb_reserved_size = 0,
	},
	[PLATFORM_RAVEN] = {
		.name = "Raven",
		.needs_ish = false,
		.is_multi_level = false,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = false,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0A0000},
		.psb_reserved_size = 0x100,
	},
	[PLATFORM_PICASSO] = {
		.name = "Picasso",
		.needs_ish = false,
		.is_multi_level = false,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = false,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0A0000},
		.psb_reserved_size = 0x100,
	},
	[PLATFORM_RENOIR] = {
		.name = "Renoir",
		.needs_ish = false,
		.is_multi_level = true,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = true,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0C0000},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_LUCIENNE] = {
		.name = "Lucienne",
		.needs_ish = false,
		.is_multi_level = true,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = true,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0C0000},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_CEZANNE] = {
		.name = "Cezanne",
		.needs_ish = false,
		.is_multi_level = true,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = true,
		.is_second_gen = true,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0C0140},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_MENDOCINO] = {
		.name = "Mendocino",
		.needs_ish = true,
		.is_multi_level = true,
		.has_apob_nv_quirk = true,
		.is_initial_alignment_required = false,
		.is_second_gen = true,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0D0900},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_PHOENIX] = {
		.name = "Phoenix",
		.needs_ish = true,
		.is_multi_level = true,
		.has_apob_nv_quirk = false,
		.is_initial_alignment_required = false,
		.is_second_gen = true,
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0D0400},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_STRIX] = {
		.name = "Strix",
		.needs_ish = true,
		.is_multi_level = true,
		.has_apob_nv_quirk = false,
		.is_initial_alignment_required = false,
		.is_second_gen = true,
		.has_dir_header_v1 = true,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0E0200},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_GENOA] = {
		.name = "Genoa",
		.needs_ish = false,
		.is_multi_level = true,
		.has_apob_nv_quirk = false,
		.is_initial_alignment_required = true,
		.is_second_gen = false, /* Client SoC only flag */
		.has_dir_header_v1 = false,
		.num_psp_ids = 1,
		.psp_ids = {0xBC0C0111},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_KRACKAN2E] = {
		.name = "Krackan2e",
		.needs_ish = true,
		.is_multi_level = true,
		.has_apob_nv_quirk = false,
		.is_initial_alignment_required = false,
		.is_second_gen = true,
		.has_dir_header_v1 = true,
		.num_psp_ids = 1,
		.psp_ids = {0xbc0e1000},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_STRIXHALO] = {
		.name = "Strixhalo",
		.needs_ish = true,
		.is_multi_level = true,
		.has_apob_nv_quirk = false,
		.is_initial_alignment_required = false,
		.is_second_gen = true,
		.has_dir_header_v1 = true,
		.num_psp_ids = 1,
		.psp_ids = {0xbc0e0900},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_TURIN] = {
		.name = "Turin",
		.needs_ish = false,
		.is_multi_level = true,
		.has_apob_nv_quirk = false,
		.is_initial_alignment_required = false,
		.is_second_gen = false, /* Client SoC only flag */
		.has_dir_header_v1 = true,
		.num_psp_ids = 1,
		.psp_ids = {0xbc0e1100},
		.psb_reserved_size = 0x200,
	},
	[PLATFORM_STRIXKRACKAN] = {
		.name = "StrixKrackan",
		.needs_ish = true,
		.is_multi_level = true,
		.has_apob_nv_quirk = false,
		.is_initial_alignment_required = false,
		.is_second_gen = true,
		.has_dir_header_v1 = true,
		/* Supports multiple PSP at once */
		.num_psp_ids = 3,
		.psp_ids = {0xbc0e0200, 0xbc0e0b00, 0xbc0e1000},
		.psb_reserved_size = 0x200,
	},
};

/**
 * Identify the platform based on SoC name string.
 *
 * @param soc_name: Case-insensitive SoC name string to match
 * @return: Matching platform enum value, or PLATFORM_UNKNOWN if no match found
 */
enum platform platform_identify(char *soc_name)
{
	for (size_t i = 0; i < ARRAY_SIZE(platform_table); i++) {
		if (!platform_table[i].name) /* PLATFORM_UNKNOWN has no entry */
			continue;
		if (!strcasecmp(soc_name, platform_table[i].name))
			return i;
	}
	return PLATFORM_UNKNOWN;
}

/**
 * Check if the platform requires ISH (Image slot header) support.
 *
 * @param platform_type: Platform enum to check
 * @return: true if ISH is needed, false otherwise
 */
bool platform_needs_ish(enum platform platform_type)
{
	return platform_table[platform_type].needs_ish;
}

/**
 * Check if the platform uses multi-level directory structure.
 * Multi-level means there are separate PSP L1 and PSP L2 directory tables,
 * as opposed to a flat structure with only one PSP directory.
 *
 * Combo directory tables do not count as multi-level since they are a different
 * format and not supported.
 *
 * @param platform_type: Platform enum to check
 * @return: true if multi-level directory is used, false for flat structure
 */
bool platform_is_multi_level(enum platform platform_type)
{
	return platform_table[platform_type].is_multi_level;
}

/**
 * Check if the platform has APOB NV (AMD PSP Output Block Non-Volatile) quirk.
 * On older platforms that only had 16 MiB of ROM space or less, the address-mode was
 * always physical and the PSP would do address translation internally by cutting off the upper bits.
 *
 * On newer platforms, the address-mode is always AMD_ADDR_REL_BIOS.
 *
 * @param platform_type: Platform enum to check
 * @return: true if APOB NV quirk is present, false otherwise
 */
bool platform_has_apob_nv_quirk(enum platform platform_type)
{
	return platform_table[platform_type].has_apob_nv_quirk;
}

/**
 * Check if the platform requires initial alignment for firmware layout.
 *
 * @param platform_type: Platform enum to check
 * @return: true if initial alignment is required, false otherwise
 */
bool platform_is_initial_alignment_required(enum platform platform_type)
{
	return platform_table[platform_type].is_initial_alignment_required;
}

/**
 * Check if the platform is second generation EFS structure.
 *
 * First generation platforms support at most 16MiB of flash. They typically
 * use x86 MMIO addresses as the whole flash is mapped into the MMIO space.
 *
 * Second generation platforms support more than 16MiB of flash and thus the
 * PSP searches every 16MiB page for an EFS that offset 0x24 BIT0 cleared.
 * On those platforms, the address in the directory is a relative offset to
 * the flash start.
 *
 * @param platform_type: Platform enum to check
 * @return: true if second generation, false for first generation
 */
bool platform_is_second_gen(enum platform platform_type)
{
	return platform_table[platform_type].is_second_gen;
}

/**
 * Check if the platform uses PSP directory header version 1 format.
 * If false it uses the PSP directory header version 0 and supports a maximum
 * of 4 MiB directory size, while version 1 supports up to 64 MiB.
 *
 * @param platform_type: Platform enum to check
 * @return: true if dir header v1 is used, false for other versions
 */
bool platform_has_dir_header_v1(enum platform platform_type)
{
	return platform_table[platform_type].has_dir_header_v1;
}

/**
 * Get the number of PSP (Platform Security Processor) IDs for the platform.
 * Older platforms might not insert a PSP ID in the directory, in which case
 * this function returns 0.
 *
 * @param platform_type: Platform enum to query
 * @return: number of PSP IDs, or 0 if not applicable
 */
uint32_t platform_get_num_psp_ids(enum platform platform_type)
{
	return platform_table[platform_type].num_psp_ids;
}

/**
 * Get the PSP (Platform Security Processor) ID for the platform.
 * Older platforms might not insert a PSP ID in the directory, in which case
 * this function returns 0.
 *
 * @param platform_type: Platform enum to query
 * @return: 32-bit PSP ID value, or 0 if not applicable
 */
uint32_t platform_get_psp_id(enum platform platform_type, const unsigned int index)
{
	if (index >= platform_get_num_psp_ids(platform_type))
		return 0;
	return platform_table[platform_type].psp_ids[index];
}

/**
 * Returns true when legacy A/B recovery is enabled, which means that the PSP will look for
 * PSP type 0x6e (PSP_AB_NVRAM) in the directory and treat it as A/B recovery data.
 *
 * @param cb_config: Configuration struct
 * @return: true if legacy A/B recovery is enabled, false otherwise
 */
bool platform_has_legacy_ab_recovery(amd_cb_config *cb_config)
{
	return cb_config->soc_id == PLATFORM_RENOIR && cb_config->recovery_ab;
}

/**
 * Returns the maximum size of BIOS type 0x07 used for PSB.
 *
 * @param cb_config: Configuration struct
 * @return: number of bytes to reserve for PSB, or 0 if PSB is unsupported
 */
uint32_t platform_psb_reserved_size(enum platform platform_type)
{
	return platform_table[platform_type].psb_reserved_size;
}
