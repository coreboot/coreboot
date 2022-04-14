/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <crc_byte.h>
#include <fmap.h>
#include <spd_cache.h>
#include <spd_bin.h>
#include <string.h>

/*
 * SPD_CACHE layout
 *    +==========+ offset 0x00
 *    |DIMM 1 SPD|   SPD data length is CONFIG_DIMM_SPD_SIZE.
 *    +----------+ offset CONFIG_DIMM_SPD_SIZE * 1
 *    |DIMM 2 SPD|
 *    +----------+ offset CONFIG_DIMM_SPD_SIZE * 2
 *         ...
 *    +----------+ offset CONFIG_DIMM_SPD_SIZE * (N -1)
 *    |DIMM N SPD|   N = CONFIG_DIMM_MAX
 *    +----------+ offset CONFIG_DIMM_SPD_SIZE * CONFIG_DIMM_MAX
 *    |  CRC 16  |   Use to verify the data correctness.
 *    +==========+
 *
 *  The size of the RW_SPD_CACHE needs to be aligned with 4KiB.
 */

/*
 * Use to update SPD cache.
 *  *blk : the new SPD data will be stash into the cache.
 *
 *  return CB_SUCCESS , update SPD cache successfully.
 *  return CB_ERR , update SPD cache unsuccessfully and the cache is invalid
 */
enum cb_err update_spd_cache(struct spd_block *blk)
{
	struct region_device rdev;
	uint16_t data_crc = 0;
	int i, j;

	assert(blk->len <= SC_SPD_LEN);

	if (fmap_locate_area_as_rdev_rw(SPD_CACHE_FMAP_NAME, &rdev)) {
		printk(BIOS_ERR, "SPD_CACHE: Cannot access %s region\n", SPD_CACHE_FMAP_NAME);
		return CB_ERR;
	}

	/* Erase whole area, it's for align with 4KiB which is the size of SPI rom sector. */
	if (rdev_eraseat(&rdev, 0, region_device_sz(&rdev)) < 0) {
		printk(BIOS_ERR, "SPD_CACHE: Cannot erase %s region\n", SPD_CACHE_FMAP_NAME);
		return CB_ERR;
	}

	/* Write SPD data */
	for (i = 0; i < SC_SPD_NUMS; i++) {
		if (blk->spd_array[i] == NULL) {
			/* If DIMM is not present, we calculate the CRC with 0xff. */
			for (j = 0; j < SC_SPD_LEN; j++)
				data_crc = crc16_byte(data_crc, 0xff);
		} else {
			if (rdev_writeat(&rdev, blk->spd_array[i], SC_SPD_OFFSET(i), blk->len)
										< 0) {
				printk(BIOS_ERR, "SPD_CACHE: Cannot write SPD data at %d\n",
					SC_SPD_OFFSET(i));
				return CB_ERR;
			}

			for (j = 0; j < blk->len; j++)
				data_crc = crc16_byte(data_crc, blk->spd_array[i][j]);

			/* If the blk->len < SC_SPD_LEN, we calculate the CRC with 0xff. */
			if (blk->len < SC_SPD_LEN)
				for (j = 0; j < (SC_SPD_LEN - (blk->len)); j++)
					data_crc = crc16_byte(data_crc, 0xff);
		}
	}

	/* Write the crc16 */
	/* It must be the last step to ensure that the data is written correctly */
	if (rdev_writeat(&rdev, &data_crc, SC_CRC_OFFSET, SC_CRC_LEN) < 0) {
		printk(BIOS_ERR, "SPD_CACHE: Cannot write crc at 0x%04x\n", SC_CRC_OFFSET);
		return CB_ERR;
	}
	return CB_SUCCESS;
}

/*
 * Locate the RW_SPD_CACHE area in the fmap and read SPD_CACHE data.
 *  return CB_SUCCESS ,if the SPD_CACHE data is ready and the pointer return at *spd_cache.
 *  return CB_ERR ,if it cannot locate RW_SPD_CACHE area in the fmap or data cannot be read.
 */
enum cb_err load_spd_cache(uint8_t **spd_cache, size_t *spd_cache_sz)
{
	struct region_device rdev;

	if (fmap_locate_area_as_rdev(SPD_CACHE_FMAP_NAME, &rdev) < 0) {
		printk(BIOS_ERR, "SPD_CACHE: Cannot find %s region\n", SPD_CACHE_FMAP_NAME);
		return CB_ERR;
	}

	/* Assume boot device is memory mapped. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));
	*spd_cache = rdev_mmap_full(&rdev);

	if (*spd_cache == NULL)
		return CB_ERR;

	*spd_cache_sz = region_device_sz(&rdev);

	/* SPD cache found */
	printk(BIOS_INFO, "SPD_CACHE: cache found, size 0x%zx\n", *spd_cache_sz);

	return CB_SUCCESS;
}

/* Use to verify the cache data is valid. */
bool spd_cache_is_valid(uint8_t *spd_cache, size_t spd_cache_sz)
{
	uint16_t data_crc = 0;
	int i;

	if (spd_cache_sz < SC_SPD_TOTAL_LEN + SC_CRC_LEN)
		return false;

	/* Check the spd_cache crc */
	for (i = 0; i < SC_SPD_TOTAL_LEN; i++)
		data_crc = crc16_byte(data_crc, *(spd_cache + i));

	return *(uint16_t *)(spd_cache + SC_CRC_OFFSET) == data_crc;
}

/*
 * Check if the DIMM is preset in cache.
 *  return true , DIMM is present.
 *  return false, DIMM is not present.
 */
static bool get_cached_dimm_present(uint8_t *spd_cache, uint8_t idx)
{
	if (*(uint16_t *)(spd_cache + SC_SPD_OFFSET(idx)) == 0xffff)
		return false;
	else
		return true;
}

/*
 * Use to check if the SODIMM is changed.
 *  spd_cache : it's a valid SPD cache.
 *  blk       : it must include the smbus addresses of SODIMM.
 */
bool check_if_dimm_changed(u8 *spd_cache, struct spd_block *blk)
{
	int i;
	u32 sn;
	bool dimm_present_in_cache;
	bool dimm_changed = false;
	/* Check if the dimm is the same with last system boot. */
	for (i = 0; i < SC_SPD_NUMS && !dimm_changed; i++) {
		if (blk->addr_map[i] == 0) {
			printk(BIOS_NOTICE, "SPD_CACHE: DIMM%d does not exist\n", i);
			continue;
		}
		/* Return true if any error happened here. */
		if (get_spd_sn(blk->addr_map[i], &sn) == CB_ERR)
			return true;
		dimm_present_in_cache = get_cached_dimm_present(spd_cache, i);
		/* Dimm is not present now. */
		if (sn == 0xffffffff) {
			if (!dimm_present_in_cache)
				printk(BIOS_NOTICE, "SPD_CACHE: DIMM%d is not present\n", i);
			else {
				printk(BIOS_NOTICE, "SPD_CACHE: DIMM%d lost\n", i);
				dimm_changed = true;
			}
		} else { /* Dimm is present now. */
			if (dimm_present_in_cache) {
				if (memcmp(&sn, spd_cache + SC_SPD_OFFSET(i) + DDR4_SPD_SN_OFF,
						SPD_SN_LEN) == 0)
					printk(BIOS_NOTICE, "SPD_CACHE: DIMM%d is the same\n",
											i);
				else {
					printk(BIOS_NOTICE, "SPD_CACHE: DIMM%d is new one\n",
											i);
					dimm_changed = true;
				}
			} else {
				printk(BIOS_NOTICE, "SPD_CACHE: DIMM%d is new one\n", i);
				dimm_changed = true;
			}
		}
	}
	return dimm_changed;
}

/* Use to fill the struct spd_block with cache data.*/
enum cb_err spd_fill_from_cache(uint8_t *spd_cache, struct spd_block *blk)
{
	int i;
	u8 dram_type;

	/* Find the first present SPD */
	for (i = 0; i < SC_SPD_NUMS; i++)
		if (get_cached_dimm_present(spd_cache, i))
			break;

	if (i == SC_SPD_NUMS) {
		printk(BIOS_ERR, "SPD_CACHE: No DIMM is present.\n");
		return CB_ERR;
	}

	dram_type = *(spd_cache + SC_SPD_OFFSET(i) + SPD_DRAM_TYPE);

	if (dram_type == SPD_DRAM_DDR4)
		blk->len = SPD_PAGE_LEN_DDR4;
	else
		blk->len = SPD_PAGE_LEN;

	for (i = 0; i < SC_SPD_NUMS; i++)
		if (get_cached_dimm_present(spd_cache, i))
			blk->spd_array[i] = spd_cache + SC_SPD_OFFSET(i);
		else
			blk->spd_array[i] = NULL;

	return CB_SUCCESS;
}
