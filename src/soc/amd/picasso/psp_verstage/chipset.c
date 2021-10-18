/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <psp_verstage.h>

uint32_t update_psp_bios_dir(uint32_t *psp_dir_offset, uint32_t *bios_dir_offset)
{
	return svc_update_psp_bios_dir(psp_dir_offset, bios_dir_offset,
				       DIR_OFFSET_SET);
}

uint32_t save_uapp_data(void *address, uint32_t size)
{
	return svc_save_uapp_data(UAPP_COPYBUF_CHROME_WORKBUF, address, size);
}

uint32_t get_max_workbuf_size(uint32_t *size)
{
	return svc_get_max_workbuf_size(size);
}

uint32_t get_bios_dir_addr(struct embedded_firmware *ef_table)
{
	return ef_table->bios1_entry;
}

int platform_set_sha_op(enum vb2_hash_algorithm hash_alg,
			struct sha_generic_data *sha_op)
{
	if (hash_alg == VB2_HASH_SHA256) {
		sha_op->SHAType = SHA_TYPE_256;
		sha_op->DigestLen = 32;
	} else if (hash_alg == VB2_HASH_SHA512) {
		sha_op->SHAType = SHA_TYPE_512;
		sha_op->DigestLen = 64;
	} else {
		return -1;
	}
	return 0;
}
