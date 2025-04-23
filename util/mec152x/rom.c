/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/bsd/compiler.h>
#include <endian.h>
#include <fmap.h>
#include <stdio.h>
#include "rom.h"
#include "utils.h"

/* Generate EC FW PTR. EC FW must be 256 byte aligned and has a CRC-8 checksum */
static uint32_t gen_signature(uint32_t rom_offset)
{
	uint32_t signature = rom_offset >> 8;
	uint8_t crc = crc8_itu((uint8_t *)&signature, 3);

	signature |= ((uint32_t)crc) << 24;

	return signature;
}

/**
 *  @brief  Write the EC FW pointer to the ROM
 *  @param  rom_file          The filename of the ROM to update
 *                            The ROM must contain a valid FMAP
 *  @param  fmap_region_name  FMAP region name containing the EC firmware
 *  @return  True on success
 *
 *  On MEC15x1 the firmware resides in the shared x86 SPI flash. As the EC
 *  boots first and does the power sequencing there's no concurrent access.
 *  The EC expects to find a pointer to the FW at offset 0.
 */
bool rom_set_ec_fw_ptr(const char rom_file[], const char *fmap_region_name)
{
	struct mem_range file = {0};
	bool ret = false;

	file = map_file(rom_file, true);
	if (file.start == NULL) {
		fprintf(stderr, "ERROR: Failed to load \"%s\"\n", rom_file);
		return false;
	}

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

		/* EC FW must be aligned */
		if (area->offset % 256) {
			fprintf(stderr,
				"ERROR: Region '%s' not aligned to 256 in \"%s\"\n",
				fmap_region_name, rom_file);
			goto error;
		}

		/* EC FW pointer resides at offset 0 */
		uint32_t *ec_fw_ptr = (uint32_t *)file.start;
		*ec_fw_ptr = htole32(gen_signature(area->offset));
	} else {
		fprintf(stderr, "ERROR: FMAP not found \"%s\"\n", rom_file);
		goto error;
	}

	ret = true;
error:
	unmap_file(file);
	return ret;
}
