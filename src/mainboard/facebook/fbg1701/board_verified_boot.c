/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2019 Eltan B.V.
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

#include "board_verified_boot.h"

#ifdef __BOOTBLOCK__
/* The items verified by the bootblock, the bootblock will not measure the
 * items to the TPM
 */
const verify_item_t bootblock_verify_list[] = {
	{ VERIFY_FILE, ROMSTAGE, { { NULL, CBFS_TYPE_STAGE } },
		HASH_IDX_ROM_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_BLOCK, "BootBlock",
		{ { (void *)0xffffffff - CONFIG_C_ENV_BOOTBLOCK_SIZE + 1,
		CONFIG_C_ENV_BOOTBLOCK_SIZE, } }, HASH_IDX_BOOTBLOCK,
		MBOOT_PCR_INDEX_0 },
	{ VERIFY_BLOCK, "PublicKey",
		{ { (void *)CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_LOCATION,
		CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_SIZE, } }, HASH_IDX_PUBLICKEY,
		MBOOT_PCR_INDEX_0 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};
#endif

#if defined(__ROMSTAGE__) || defined(__POSTCAR__)
/* The FSP is already checked in romstage */
static const verify_item_t ram_stage_additional_list[] = {
	{ VERIFY_FILE, OP_ROM_VBT, { { NULL, CBFS_TYPE_RAW } },
		HASH_IDX_OPROM, MBOOT_PCR_INDEX_2 },
	{ VERIFY_FILE, "logo.bmp", { { NULL, CBFS_TYPE_RAW } },
		HASH_IDX_LOGO, MBOOT_PCR_INDEX_2 },
	{ VERIFY_FILE, "fallback/dsdt.aml", { { NULL, CBFS_TYPE_RAW } },
		HASH_IDX_DSDT, MBOOT_PCR_INDEX_2 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
	};
#endif

#ifdef __ROMSTAGE__
/* The items used by the romstage */
const verify_item_t romstage_verify_list[] = {
	{ VERIFY_FILE, ROMSTAGE, { { NULL, CBFS_TYPE_STAGE } },
		HASH_IDX_ROM_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_FILE, MICROCODE, { { NULL, CBFS_TYPE_MICROCODE } },
		HASH_IDX_MICROCODE, MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, FSP, { { NULL, CBFS_TYPE_FSP } }, HASH_IDX_FSP,
		MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, "spd.bin", { { NULL, CBFS_TYPE_SPD } },
		HASH_IDX_SPD0, MBOOT_PCR_INDEX_1 },
#if CONFIG(POSTCAR_STAGE)
	{ VERIFY_FILE, POSTCAR, { { NULL, CBFS_TYPE_STAGE } },
		HASH_IDX_POSTCAR_STAGE, MBOOT_PCR_INDEX_0 },
#endif
	{ VERIFY_BLOCK, "BootBlock",
		{ { (void *)0xffffffff - CONFIG_C_ENV_BOOTBLOCK_SIZE + 1,
		CONFIG_C_ENV_BOOTBLOCK_SIZE, } }, HASH_IDX_BOOTBLOCK,
		MBOOT_PCR_INDEX_0 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

/* The items  used by the ramstage */
const verify_item_t ramstage_verify_list[] = {
	{ VERIFY_FILE, RAMSTAGE, { { ram_stage_additional_list,
		CBFS_TYPE_STAGE } }, HASH_IDX_RAM_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};
#endif

#ifdef __POSTCAR__
/* POSTSTAGE */
/* The items used by the postcar stage */
const verify_item_t postcar_verify_list[] = {
	{ VERIFY_FILE, RAMSTAGE, { { ram_stage_additional_list,
		CBFS_TYPE_STAGE } }, HASH_IDX_RAM_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_FILE, MICROCODE, { { NULL, CBFS_TYPE_MICROCODE } },
		HASH_IDX_MICROCODE, MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, FSP, { { NULL, CBFS_TYPE_FSP } }, HASH_IDX_FSP,
		MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, "spd.bin", { { NULL, CBFS_TYPE_SPD } }, HASH_IDX_SPD0,
		MBOOT_PCR_INDEX_1 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};
#endif

#ifdef __RAMSTAGE__
/* RAMSTAGE */
const verify_item_t payload_verify_list[] = {
	{ VERIFY_FILE, PAYLOAD, { { NULL, CBFS_TYPE_SELF |
		VERIFIED_BOOT_COPY_BLOCK } }, HASH_IDX_PAYLOAD,
		MBOOT_PCR_INDEX_3 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

const verify_item_t oprom_verify_list[] = {
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};
#endif
