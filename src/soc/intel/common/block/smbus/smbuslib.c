/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/dram/ddr3.h>
#include <device/dram/ddr4.h>
#include <spd.h>
#include <spd_bin.h>
#include <device/smbus_def.h>
#include <device/smbus_host.h>
#include "smbuslib.h"

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

static void switch_page(u8 spd_addr, u8 new_page)
{
	/*
	 *  By default,an SPD5 hub accepts 1 byte addressing pointing
	 *  to the first 128 bytes of memory. MR11[2:0] selects the page
	 *  pointer to address the entire 1024 bytes of non-volatile memory.
	 */
	smbus_write_byte(spd_addr, SPD5_MEMREG_REG(SPD5_MR11), new_page);
}

/*
 * Read the SPD data over the SMBus, at the specified SPD address,
 * starting at the specified starting offset and read the given amount of data.
 */
static void smbus_read_spd5(u8 *spd, u8 spd_addr, u16 size)
{
	u8 page = ~0;
	u32 max_page_size = MAX_SPD_PAGE_SIZE_SPD5;

	if (size > MAX_SPD_SIZE) {
		printk(BIOS_ERR, "Maximum SPD size reached\n");
		return;
	}
	for (int i = 0; i < size; i++) {
		u8 next_page = (u8) (i / max_page_size);
		if (next_page != page) {
			switch_page(spd_addr, next_page);
			page = next_page;
		}
		unsigned int byte_addr = SPD_HUB_MEMREG(i % max_page_size);
		spd[i] = smbus_read_byte(spd_addr, byte_addr);
	}
}

/* Read SPD5 MR0 and check if SPD Byte 0 matches the SPD5 HUB MR0 identifier.*/
static int is_spd5_hub(u8 spd_addr)
{
	u8 spd_hub_byte;

	spd_hub_byte = smbus_read_byte(spd_addr, SPD5_MEMREG_REG(SPD5_MR0));
	return spd_hub_byte == SPD5_MR0_SPD5_HUB_DEV;
}

/*
 * Reset the SPD page back to page 0 on an SPD5 Hub device at the
 * input SPD SMbus address.
 */
static void reset_page_spd5(u8 spd_addr)
{
	/* Set SPD5 MR11[2:0] = 0 (Page 0) */
	switch_page(spd_addr, 0);
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

	if (CONFIG(DRAM_SUPPORT_DDR5) && is_spd5_hub(addr)) {
		smbus_read_spd5(spd, addr, CONFIG_DIMM_SPD_SIZE);

		/* Reset the page for the next loop iteration */
		reset_page_spd5(addr);
	} else {
		/* IMC doesn't support i2c eeprom read. */
		if (CONFIG(SOC_INTEL_COMMON_BLOCK_IMC) ||
		    i2c_eeprom_read(addr, 0, SPD_SIZE_MAX_DDR3, spd) < 0) {
			printk(BIOS_INFO, "do_i2c_eeprom_read failed, using fallback\n");
			spd_read(spd, addr);
		}

		/* Check if module is DDR4, DDR4 spd is 512 byte. */
		if (spd[SPD_MEMORY_TYPE] == SPD_MEMORY_TYPE_DDR4_SDRAM &&
		    CONFIG_DIMM_SPD_SIZE > SPD_SIZE_MAX_DDR3) {
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

	blk->len = CONFIG_DIMM_SPD_SIZE;
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
