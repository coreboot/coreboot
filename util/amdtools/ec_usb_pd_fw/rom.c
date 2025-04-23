/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/bsd/compiler.h>
#include <endian.h>
#include <fmap.h>
#include <stdio.h>
#include "rom.h"
#include "utils.h"

/**
 *  @brief Generate pointer to PD firmware.
 *  @param  rom_offset  Offset of the PD firmware
 *  @param  size        Size of the PD firmware
 *
 *  Each pointer is 8 byte wide consists of:
 *   - 4 byte offset
 *   - 3 byte size
 *   - 1 byte checksum (all 8 bytes sum up to 0xff)
 */
static uint64_t gen_signature(uint32_t rom_offset, uint32_t size)
{
	uint64_t signature = rom_offset;
	signature |= ((uint64_t)size & 0xffffffULL) << 32;
	uint8_t crc = 0;

	for (int i = 0; i < 7; i++)
		crc += (signature >> (i * 8)) & 0xff;

	signature |= (0xffULL - (uint64_t)crc) << 56;

	return signature;
}

static bool check_signature(uint64_t signature)
{
	uint8_t crc = 0;

	for (int i = 0; i < 8; i++)
		crc += (signature >> (i * 8)) & 0xff;

	return crc == 0xff;
}

/**
 *  @brief  Write the EC PD pointer to the ROM
 *  @param  rom_file          The filename of the ROM to update
 *                            The ROM must contain a valid FMAP
 *  @param  fmap_region_name  FMAP region name containing the PD firmware
 *  @return  True on success
 *
 *  The EC firmware copies the USB Type-C PD firmware from the x86 SPI flash
 *  to the corresponding controller when necessary. It expects to find a list
 *  of pointers at offset 10h. The ROM can contain multiple PD firmwares depending
 *  on the used IC vendor/familiy.
 *
 */
bool rom_set_pd_fw_ptr(const char rom_file[], const char *fmap_region_name, int slot, bool automatic)
{
	uint64_t *ec_pd_fw_ptr;
	struct mem_range file = {0};
	bool ret = false;

	file = map_file(rom_file, true);
	if (file.start == NULL) {
		fprintf(stderr, "ERROR: Failed to load \"%s\"\n", rom_file);
		return false;
	}

	if (automatic)
		slot = 0;

	long fmap_offset = fmap_find(file.start, file.length);
	if (fmap_offset >= 0) {
		struct fmap *fmap = (void *)(file.start + fmap_offset);
		const struct fmap_area *area = fmap_find_area(fmap, fmap_region_name);
		if (area == NULL) {
			fprintf(stderr,
				"ERROR: Found FMAP without '%s' in \"%s\"\n\n",
				fmap_region_name, rom_file);
			printf("FMAP is:\n");
			fmap_print(fmap);
			goto error;
		}

		/* EC PD pointer resides at offset 0x10 */
		ec_pd_fw_ptr = (uint64_t *)(file.start + 0x10 + slot * 8);

		/* Find free slot if in auto mode */
		for (; automatic && check_signature(le64toh(*ec_pd_fw_ptr)) && slot < 12; slot++)
			ec_pd_fw_ptr++;

		if (slot == 12) {
			fprintf(stderr, "ERROR: No free slots left\n\n");
			goto error;
		}

		*ec_pd_fw_ptr = htole64(gen_signature(area->offset, area->size));
	} else {
		fprintf(stderr, "ERROR: FMAP not found \"%s\"\n", rom_file);
		goto error;
	}

	ret = true;
error:
	unmap_file(file);
	return ret;
}
