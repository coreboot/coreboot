/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <lib.h>
#include <security/tpm/tspi.h>
#include <security/tpm/tss/tcg-2.0/tss_structures.h>
#include <soc/measured_boot.h>
#include <soc/fit.h>
#include <string.h>

extern const uint64_t fit_ptr;

static void *find_fit_entry_data(uint8_t type, uint8_t sub_type)
{
	struct firmware_interface_table_entry	*fit_entry;
	uint32_t entry_num;
	uint32_t fit_table_offset;
	uint32_t index;

	fit_table_offset = fit_ptr;
	fit_entry = (struct firmware_interface_table_entry *)(uint32_t)fit_table_offset;
	if (fit_entry[0].address != FIT_TABLE_SIGNATURE)
		return NULL;

	if (fit_entry[0].type != FIT_TABLE_TYPE_HEADER)
		return NULL;

	entry_num = *(uint32_t *)(&fit_entry[0].size[0]) & 0xffffff;
	for (index = 0; index < entry_num; index++) {
		if ((fit_entry[index].type == type) && (fit_entry[index].sub_type == sub_type))
			return (void *)(uint32_t)fit_entry[index].address;
	}

	return NULL;
}

bool fetch_pre_rbp_data(struct boot_policy_manifest *bpm_info)
{
	uint8_t *fit_data;
	fit_data = find_fit_entry_data(FIT_TABLE_TYPE_TXE_SECURE_BOOT,
				       FIT_ENTRY_SUB_TYPE_BOOT_POLICY);

	if (fit_data != NULL) {
		memcpy((void *)&(bpm_info->bpm), fit_data, sizeof(bpm_info->bpm));

		printk(BIOS_DEBUG, "Boot Guard 2.0: Verified Boot: %s\n",
			bpm_info->bpm.vb ? "Enforced" : "Not enforced");
		if (bpm_info->bpm.vb && !CONFIG(IFWI_VERIFIED_BOOT))
			printk(BIOS_CRIT, "Boot Guard 2.0: Verified boot is enforced but not enabled.\n");

		printk(BIOS_DEBUG, "Boot Guard 2.0: Measured Boot: %s\n",
			bpm_info->bpm.mb ? "Enforced" : "Not enforced");
		if (bpm_info->bpm.mb && !CONFIG(IFWI_MEASURED_BOOT))
			printk(BIOS_CRIT, "Boot Guard 2.0: Measured boot is enforced but not enabled.\n");
	}

	if (CONFIG(IFWI_MEASURED_BOOT)) {
		/* TXE */
		fit_data = find_fit_entry_data(FIT_TABLE_TYPE_TXE_SECURE_BOOT,
					       FIT_ENTRY_SUB_TYPE_TXE_HASH);
		if (fit_data) {
			memcpy((void *)(bpm_info->txe_hash), fit_data,
				sizeof(bpm_info->txe_hash));
			printk(BIOS_DEBUG, "TXE Hash: %hhn\n", bpm_info->txe_hash);
			hexdump((void *)bpm_info->txe_hash, SHA256_DIGEST_SIZE);

			tpm_extend_pcr(0, TPM_ALG_SHA256, bpm_info->txe_hash,
				SHA256_DIGEST_SIZE, "TXE");
		}

		/* IBBL */
		fit_data = find_fit_entry_data(FIT_TABLE_TYPE_TXE_SECURE_BOOT,
					       FIT_ENTRY_SUB_TYPE_IBBL_HASH);
		if (fit_data) {
			memcpy((void *)(bpm_info->ibbl_hash), fit_data,
				sizeof(bpm_info->ibbl_hash));
			printk(BIOS_DEBUG, "IBBL Hash: %hhn\n", bpm_info->ibbl_hash);
			hexdump((void *)bpm_info->ibbl_hash, SHA256_DIGEST_SIZE);

			tpm_extend_pcr(0, TPM_ALG_SHA256, bpm_info->ibbl_hash,
			SHA256_DIGEST_SIZE, "IBBL");

		}
		/* Return if IBB exists */
		fit_data = find_fit_entry_data(FIT_TABLE_TYPE_TXE_SECURE_BOOT,
					       FIT_ENTRY_SUB_TYPE_IBB_HASH);

		printk(BIOS_DEBUG, "IBB: %s\n", fit_data ? "Present" : "Not present");
		return fit_data;
	}

	return 0;
}

void fetch_post_rbp_data(struct boot_policy_manifest *bpm_info)
{
	if (CONFIG(IFWI_MEASURED_BOOT)) {
		/* IBB */
		uint8_t *fit_data;
		fit_data = find_fit_entry_data(FIT_TABLE_TYPE_TXE_SECURE_BOOT,
					       FIT_ENTRY_SUB_TYPE_IBB_HASH);

		if (fit_data) {
			memcpy((void *)bpm_info->ibb_hash, fit_data,
				sizeof(bpm_info->ibb_hash));
			printk(BIOS_DEBUG, "IBB Hash: %hhn\n", bpm_info->ibb_hash);
			hexdump((void *)bpm_info->ibb_hash, SHA256_DIGEST_SIZE);

			tpm_extend_pcr(0, TPM_ALG_SHA256, bpm_info->ibb_hash,
				SHA256_DIGEST_SIZE, "IBBM");
		}
	}
}
