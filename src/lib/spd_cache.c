/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <crc_byte.h>
#include <device/dram/ddr3.h>
#include <device/dram/ddr4.h>
#include <device/dram/ddr5.h>
#include <fmap.h>
#include <security/vboot/antirollback.h>
#include <security/vboot/mrc_cache_hash_tpm.h>
#include <spd_cache.h>
#include <spd.h>
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
	uint8_t full_spd_buf[SC_SPD_TOTAL_LEN];
	int i;

	assert(blk->len <= SC_SPD_LEN);

	if (fmap_locate_area_as_rdev_rw(SPD_CACHE_FMAP_NAME, &rdev)) {
		printk(BIOS_ERR, "SPD_CACHE: Cannot access %s region\n", SPD_CACHE_FMAP_NAME);
		return CB_ERR;
	}

	/* Phase 1: Build a contiguous, normalized byte stream buffer */
	for (i = 0; i < SC_SPD_NUMS; i++) {
		uint8_t *slot_ptr = &full_spd_buf[SC_SPD_OFFSET(i)];

		if (blk->spd_array[i] == NULL) {
			/* Missing DIMMs are padded with 0xFF */
			memset(slot_ptr, 0xff, SC_SPD_LEN);
		} else {
			memcpy(slot_ptr, blk->spd_array[i], blk->len);
			if (blk->len < SC_SPD_LEN)
				memset(slot_ptr + blk->len, 0xff, SC_SPD_LEN - blk->len);
		}
	}

	/* Phase 2a: Erase and Write SPD data to SPI FLASH */
	if (rdev_eraseat(&rdev, 0, region_device_sz(&rdev)) < 0) {
		printk(BIOS_ERR, "SPD_CACHE: Cannot erase %s region\n", SPD_CACHE_FMAP_NAME);
		return CB_ERR;
	}

	/* Phase 2b: Write the entire normalized buffer so flash content matches full_spd_buf */
	if (rdev_writeat(&rdev, full_spd_buf, 0, sizeof(full_spd_buf)) < 0) {
		printk(BIOS_ERR, "SPD_CACHE: Cannot write full SPD buffer\n");
		return CB_ERR;
	}

	/* Phase 3: Integrity check / TPM hashing selection based on Kconfig */
	if (CONFIG(SPD_CACHE_TPM_HASH)) {
		mrc_cache_update_hash(SPD_CACHE_NV_INDEX, full_spd_buf, sizeof(full_spd_buf));
		printk(BIOS_INFO, "SPD_CACHE: Extending TPM PCR with SPD buffer measurement\n");
	} else {
		uint16_t data_crc = CRC(full_spd_buf, sizeof(full_spd_buf), crc16_byte);;

		if (rdev_writeat(&rdev, &data_crc, SC_CRC_OFFSET, SC_CRC_LEN) < 0) {
			printk(BIOS_ERR, "SPD_CACHE: Cannot write crc at 0x%04x\n", SC_CRC_OFFSET);
			return CB_ERR;
		}
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

/* Validates the in-memory SPD cache against the CRC16 algorithm */
static bool spd_cache_crc16_is_valid(uint8_t *spd_cache, size_t spd_cache_sz)
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
 * Check if the cached SPD TPM hash is valid.
 *
 * Compares the TPM hash currently stored in the system or TPM against the
 * calculated hash of the provided SPD cache buffer to verify integrity.
 */
static bool spd_cache_tpm_hash_is_valid(const uint8_t *spd_cache, size_t spd_cache_sz)
{
	if (spd_cache_sz < SC_SPD_TOTAL_LEN)
		return false;

	if (mrc_cache_verify_hash(SPD_CACHE_NV_INDEX, spd_cache, SC_SPD_TOTAL_LEN)) {
		printk(BIOS_INFO, "SPD_CACHE: TPM NV hash verification succeeded\n");
		return true;
	}

	printk(BIOS_WARNING, "SPD_CACHE: Hash mismatch against TPM NV index 0x%x\n",
	       SPD_CACHE_NV_INDEX);
	return false;
}

/* Use to verify the cache data is valid. */
bool spd_cache_is_valid(uint8_t *spd_cache, size_t spd_cache_sz)
{
	if (CONFIG(SPD_CACHE_TPM_HASH))
		return spd_cache_tpm_hash_is_valid(spd_cache, spd_cache_sz);

	return spd_cache_crc16_is_valid(spd_cache, spd_cache_sz);
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
 * Return the byte offset of the serial number within an SPD image, which
 * differs by DRAM generation. Must match the offset get_spd_sn() reads the
 * live serial number from.
 *
 * Returns 0 for unsupported DRAM types, since offset 0 never holds a serial
 * number.
 */
static size_t spd_serial_offset(uint8_t dram_type)
{
	switch (dram_type) {
	case SPD_MEMORY_TYPE_DDR5_SDRAM:
	case SPD_MEMORY_TYPE_LPDDR5_SDRAM:
	case SPD_MEMORY_TYPE_LPDDR5X_SDRAM:
		return DDR5_SPD_SN_OFF;
	case SPD_MEMORY_TYPE_DDR4_SDRAM:
		return DDR4_SPD_SN_OFF;
	case SPD_MEMORY_TYPE_SDRAM_DDR3:
		return SPD_DDR3_SERIAL_NUM;
	default:
		return 0;
	}
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
				u8 dram_type = spd_cache[SC_SPD_OFFSET(i) + SPD_MEMORY_TYPE];
				size_t sn_off = spd_serial_offset(dram_type);
				if (sn_off == 0) {
					printk(BIOS_ERR,
						"SPD_CACHE: DIMM%d unknown DRAM type 0x%02x\n",
						i, dram_type);
					return true;
				}
				if (memcmp(&sn, spd_cache + SC_SPD_OFFSET(i) + sn_off,
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

	dram_type = *(spd_cache + SC_SPD_OFFSET(i) + SPD_MEMORY_TYPE);

	switch (dram_type) {
	case SPD_MEMORY_TYPE_DDR4_SDRAM:
		blk->len = SPD_SIZE_MAX_DDR4;
		break;
	case SPD_MEMORY_TYPE_SDRAM_DDR3:
		blk->len = SPD_SIZE_MAX_DDR3;
		break;
	default:
		printk(BIOS_WARNING, "SPD_CACHE: DIMM%d unknown DRAM type 0x%02x\n",
			i, dram_type);
		__fallthrough;
	case SPD_MEMORY_TYPE_DDR5_SDRAM:
	case SPD_MEMORY_TYPE_LPDDR5_SDRAM:
	case SPD_MEMORY_TYPE_LPDDR5X_SDRAM:
		blk->len = CONFIG_DIMM_SPD_SIZE;
		break;
	}

	for (i = 0; i < SC_SPD_NUMS; i++)
		if (get_cached_dimm_present(spd_cache, i))
			blk->spd_array[i] = spd_cache + SC_SPD_OFFSET(i);
		else
			blk->spd_array[i] = NULL;

	return CB_SUCCESS;
}
