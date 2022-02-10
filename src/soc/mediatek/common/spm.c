/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <soc/mcu_common.h>
#include <soc/spm.h>
#include <soc/spm_common.h>
#include <string.h>

#define SPMFW_HEADER_SIZE 16

void spm_parse_firmware(struct mtk_mcu *mcu)
{
	size_t file_size, copy_size;
	int offset;
	u16 firmware_size;

	struct dyna_load_pcm *pcm = (struct dyna_load_pcm *)mcu->priv;
	file_size = mcu->run_size;

	/*
	 * spmfw layout:
	 *   u16 firmware_size
	 *   u32 binary[firmware_size]
	 *   struct pcm_desc descriptor
	 *   char *version
	 */

	/* Firmware size */
	offset = 0;
	copy_size = sizeof(firmware_size);
	memcpy(&firmware_size, mcu->load_buffer + offset, copy_size);
	printk(BIOS_DEBUG, "SPM: binary array size = %#x\n", firmware_size);

	/* Binary */
	offset = SPMFW_HEADER_SIZE; /* binary start offset */
	copy_size = firmware_size * sizeof(u32);
	assert(offset < file_size);
	pcm->buf = (u8 *)(mcu->load_buffer + offset);

	/* Descriptor */
	offset += copy_size;
	assert(offset < file_size);
	copy_size = sizeof(pcm->desc);
	memcpy(&pcm->desc, mcu->load_buffer + offset, copy_size);

	/* Firmware size and total words need to be the same */
	assert(firmware_size == pcm->desc.total_words);

	/* Version */
	offset += copy_size;
	assert(offset < file_size);
	printk(BIOS_INFO, "SPM: spmfw (version %.*s)\n",
	       (int)(file_size - offset),
	       (u8 *)mcu->load_buffer + offset);
}
