/* SPDX-License-Identifier: GPL-2.0-only */

#include "board_verified_boot.h"

/*
 * The items verified by the bootblock, the bootblock will not measure the
 * items to the TPM
 */
const verify_item_t bootblock_verify_list[] = {
	{ VERIFY_FILE, ROMSTAGE, { { NULL, CBFS_TYPE_STAGE } },
		HASH_IDX_ROM_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_FILE, BOOTBLOCK, { { NULL, CBFS_TYPE_BOOTBLOCK } },
		HASH_IDX_BOOTBLOCK, MBOOT_PCR_INDEX_0 },
	{ VERIFY_FILE, FSP, { { NULL, CBFS_TYPE_FSP } }, HASH_IDX_FSP,
		MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, "spd.bin", { { NULL, CBFS_TYPE_SPD } },
		HASH_IDX_SPD0, MBOOT_PCR_INDEX_1 },
#if CONFIG(VENDORCODE_ELTAN_VBOOT_SIGNED_MANIFEST)
	{ VERIFY_BLOCK, "PublicKey",
		{ { (void *)CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_LOCATION,
		CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_SIZE, } }, HASH_IDX_PUBLICKEY,
		MBOOT_PCR_INDEX_0 },
#endif
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

/*
 * The items used by the romstage. Items verified by bootblock are added here to make sure they
 * are measured
 */
const verify_item_t romstage_verify_list[] = {
	{ VERIFY_FILE, ROMSTAGE, { { NULL, CBFS_TYPE_STAGE } },
		HASH_IDX_ROM_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_FILE, MICROCODE, { { NULL, CBFS_TYPE_MICROCODE } },
		HASH_IDX_MICROCODE, MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, FSP, { { NULL, CBFS_TYPE_FSP } }, HASH_IDX_FSP,
		MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, "spd.bin", { { NULL, CBFS_TYPE_SPD } },
		HASH_IDX_SPD0, MBOOT_PCR_INDEX_1 },
	{ VERIFY_FILE, BOOTBLOCK, { { NULL, CBFS_TYPE_BOOTBLOCK } },
		HASH_IDX_BOOTBLOCK, MBOOT_PCR_INDEX_0 },
#if CONFIG(VENDORCODE_ELTAN_VBOOT_SIGNED_MANIFEST)
	{ VERIFY_BLOCK, "PublicKey",
		{ { (void *)CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_LOCATION,
		CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_SIZE, } }, HASH_IDX_PUBLICKEY,
		MBOOT_PCR_INDEX_6 },
#endif
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

/* The items used by the postcar stage */
const verify_item_t postcar_verify_list[] = {
	{ VERIFY_FILE, POSTCAR, { { NULL, CBFS_TYPE_STAGE } },
		HASH_IDX_POSTCAR_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

/*
 * The items  used by the ramstage. FSP and microcode are already checked in the
 * romstage verify list
 */
static const verify_item_t ram_stage_additional_list[] = {
	{ VERIFY_FILE, OP_ROM_VBT, { { NULL, CBFS_TYPE_RAW } },
		HASH_IDX_OPROM, MBOOT_PCR_INDEX_2 },
#if CONFIG(BMP_LOGO)
	{ VERIFY_FILE, "logo.bmp", { { NULL, CBFS_TYPE_RAW } },
		HASH_IDX_LOGO, MBOOT_PCR_INDEX_2 },
#endif
	{ VERIFY_FILE, "fallback/dsdt.aml", { { NULL, CBFS_TYPE_RAW } },
		HASH_IDX_DSDT, MBOOT_PCR_INDEX_2 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

const verify_item_t ramstage_verify_list[] = {
	{ VERIFY_FILE, RAMSTAGE, { { ram_stage_additional_list,
		CBFS_TYPE_STAGE } }, HASH_IDX_RAM_STAGE, MBOOT_PCR_INDEX_0 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

/* items used by the payload */
const verify_item_t payload_verify_list[] = {
	{ VERIFY_FILE, PAYLOAD, { { NULL, CBFS_TYPE_SELF |
		VERIFIED_BOOT_COPY_BLOCK } }, HASH_IDX_PAYLOAD,
		MBOOT_PCR_INDEX_3 },
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};

/* list of allowed options roms */
const verify_item_t oprom_verify_list[] = {
	{ VERIFY_TERMINATOR, NULL, { { NULL, 0 } }, 0, 0 }
};
