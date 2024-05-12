/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/dram/ddr3.h>
#include <device/dram/ddr4.h>
#include <spd.h>
#include <spd_bin.h>
#include <device/smbus_def.h>
#include <device/smbus_host.h>
#include "smbuslib.h"

static void update_spd_len(struct spd_block *blk)
{
	u8 i, j = 0;
	for (i = 0 ; i < CONFIG_DIMM_MAX; i++)
		if (blk->spd_array[i] != NULL)
			j |= blk->spd_array[i][SPD_MEMORY_TYPE];

	/* If spd used is DDR4, then its length is 512 byte. */
	if (j == SPD_MEMORY_TYPE_DDR4_SDRAM)
		blk->len = SPD_SIZE_MAX_DDR4;
	else
		blk->len = SPD_SIZE_MAX_DDR3;
}

static void spd_read(u8 *spd, u8 addr)
{
	u16 i;
	u8 step = 1;

	if (CONFIG(SPD_READ_BY_WORD))
		step = sizeof(uint16_t);

	for (i = 0; i < SPD_SIZE_MAX_DDR3; i += step) {
		if (CONFIG(SPD_READ_BY_WORD))
			((u16*)spd)[i / sizeof(uint16_t)] =
				 spd_read_word(addr, i);
		else
			spd[i] = spd_read_byte(addr, i);
	}
}

/* return -1 if SMBus errors otherwise return 0 */
static int get_spd(u8 *spd, u8 addr)
{
	if (CONFIG_DIMM_SPD_SIZE > SPD_SIZE_MAX_DDR3) {
		/* Restore to page 0 before reading */
		spd_write_byte(SPD_PAGE_0, 0, 0);
	}

	/* If address is not 0, it will return CB_ERR(-1) if no dimm */
	if (spd_read_byte(addr, 0) < 0) {
		printk(BIOS_INFO, "No memory dimm at address %02X\n",
			addr << 1);
		return -1;
	}

	/* IMC doesn't support i2c eeprom read. */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_IMC) ||
	    i2c_eeprom_read(addr, 0, SPD_SIZE_MAX_DDR3, spd) < 0) {
		printk(BIOS_INFO, "do_i2c_eeprom_read failed, using fallback\n");
		spd_read(spd, addr);
	}

	/* Check if module is DDR4, DDR4 spd is 512 byte. */
	if (spd[SPD_MEMORY_TYPE] == SPD_MEMORY_TYPE_DDR4_SDRAM && CONFIG_DIMM_SPD_SIZE > SPD_SIZE_MAX_DDR3) {
		/* Switch to page 1 */
		spd_write_byte(SPD_PAGE_1, 0, 0);

		/* IMC doesn't support i2c eeprom read. */
		if (CONFIG(SOC_INTEL_COMMON_BLOCK_IMC) ||
		    i2c_eeprom_read(addr, 0, SPD_SIZE_MAX_DDR3, spd + SPD_SIZE_MAX_DDR3) < 0) {
			printk(BIOS_INFO, "do_i2c_eeprom_read failed, using fallback\n");
			spd_read(spd + SPD_SIZE_MAX_DDR3, addr);
		}
		/* Restore to page 0 */
		spd_write_byte(SPD_PAGE_0, 0, 0);
	}
	return 0;
}

static u8 spd_data[CONFIG_DIMM_MAX * CONFIG_DIMM_SPD_SIZE];

void get_spd_smbus(struct spd_block *blk)
{
	u8 i;
	for (i = 0 ; i < CONFIG_DIMM_MAX; i++) {
		/**
		 * Slave address 0 is also available for IMC based SPD SMBus.
		 */
		if (!CONFIG(SOC_INTEL_COMMON_BLOCK_IMC) && blk->addr_map[i] == 0) {
			blk->spd_array[i] = NULL;
			continue;
		}

		if (get_spd(&spd_data[i * CONFIG_DIMM_SPD_SIZE], blk->addr_map[i]) == 0)
			blk->spd_array[i] = &spd_data[i * CONFIG_DIMM_SPD_SIZE];
		else
			blk->spd_array[i] = NULL;
	}

	update_spd_len(blk);
}

/*
 * get_spd_sn returns the SODIMM serial number. It only supports DDR3 and DDR4.
 *  return CB_SUCCESS, sn is the serial number and sn=0xffffffff if the dimm is not present.
 *  return CB_ERR, if dram_type is not supported or addr is a zero.
 */
enum cb_err get_spd_sn(u8 addr, u32 *sn)
{
	u8 i;
	u8 dram_type;
	int smbus_ret;

	/* addr is not a zero. */
	if (addr == 0x0)
		return CB_ERR;

	if (CONFIG_DIMM_SPD_SIZE > SPD_SIZE_MAX_DDR3) {
		/* Restore to page 0 before reading */
		spd_write_byte(SPD_PAGE_0, 0, 0);
	}

	/* If dimm is not present, set sn to 0xff. */
	smbus_ret = spd_read_byte(addr, SPD_MEMORY_TYPE);
	if (smbus_ret < 0) {
		printk(BIOS_INFO, "No memory dimm at address %02X\n", addr << 1);
		*sn = 0xffffffff;
		return CB_SUCCESS;
	}

	dram_type = smbus_ret & 0xff;

	/* Check if module is DDR4, DDR4 spd is 512 byte. */
	if (dram_type == SPD_MEMORY_TYPE_DDR4_SDRAM && CONFIG_DIMM_SPD_SIZE > SPD_SIZE_MAX_DDR3) {
		/* Switch to page 1 */
		spd_write_byte(SPD_PAGE_1, 0, 0);

		for (i = 0; i < SPD_SN_LEN; i++)
			*((u8 *)sn + i) = spd_read_byte(addr,
						i + DDR4_SPD_SN_OFF);

		/* Restore to page 0 */
		spd_write_byte(SPD_PAGE_0, 0, 0);
	} else if (dram_type == SPD_MEMORY_TYPE_SDRAM_DDR3) {
		for (i = 0; i < SPD_SN_LEN; i++)
			*((u8 *)sn + i) = spd_read_byte(addr,
							i + SPD_DDR3_SERIAL_NUM);
	} else {
		printk(BIOS_ERR, "Unsupported dram_type\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}
