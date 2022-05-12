/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <spd_bin.h>
#include <device/smbus_def.h>
#include <device/smbus_host.h>
#include "smbuslib.h"

static void update_spd_len(struct spd_block *blk)
{
	u8 i, j = 0;
	for (i = 0 ; i < CONFIG_DIMM_MAX; i++)
		if (blk->spd_array[i] != NULL)
			j |= blk->spd_array[i][SPD_DRAM_TYPE];

	/* If spd used is DDR4, then its length is 512 byte. */
	if (j == SPD_DRAM_DDR4)
		blk->len = SPD_PAGE_LEN_DDR4;
	else
		blk->len = SPD_PAGE_LEN;
}

static void smbus_read_spd(u8 *spd, u8 addr)
{
	u16 i;
	u8 step = 1;

	if (CONFIG(SPD_READ_BY_WORD))
		step = sizeof(uint16_t);

	for (i = 0; i < SPD_PAGE_LEN; i += step) {
		if (CONFIG(SPD_READ_BY_WORD))
			((u16*)spd)[i / sizeof(uint16_t)] =
				 smbus_read_word(addr, i);
		else
			spd[i] = smbus_read_byte(addr, i);
	}
}

/* return -1 if SMBus errors otherwise return 0 */
static int get_spd(u8 *spd, u8 addr)
{
	/* If address is not 0, it will return CB_ERR(-1) if no dimm */
	if (smbus_read_byte(addr, 0) < 0) {
		printk(BIOS_INFO, "No memory dimm at address %02X\n",
			addr << 1);
		return -1;
	}

	if (i2c_eeprom_read(addr, 0, SPD_PAGE_LEN, spd) < 0) {
		printk(BIOS_INFO, "do_i2c_eeprom_read failed, using fallback\n");
		smbus_read_spd(spd, addr);
	}

	/* Check if module is DDR4, DDR4 spd is 512 byte. */
	if (spd[SPD_DRAM_TYPE] == SPD_DRAM_DDR4 && CONFIG_DIMM_SPD_SIZE > SPD_PAGE_LEN) {
		/* Switch to page 1 */
		smbus_write_byte(SPD_PAGE_1, 0, 0);

		if (i2c_eeprom_read(addr, 0, SPD_PAGE_LEN, spd + SPD_PAGE_LEN) < 0) {
			printk(BIOS_INFO, "do_i2c_eeprom_read failed, using fallback\n");
			smbus_read_spd(spd + SPD_PAGE_LEN, addr);
		}
		/* Restore to page 0 */
		smbus_write_byte(SPD_PAGE_0, 0, 0);
	}
	return 0;
}

static u8 spd_data[CONFIG_DIMM_MAX * CONFIG_DIMM_SPD_SIZE];

void get_spd_smbus(struct spd_block *blk)
{
	u8 i;
	for (i = 0 ; i < CONFIG_DIMM_MAX; i++) {
		if (blk->addr_map[i] == 0) {
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

	/* If dimm is not present, set sn to 0xff. */
	smbus_ret = smbus_read_byte(addr, SPD_DRAM_TYPE);
	if (smbus_ret < 0) {
		printk(BIOS_INFO, "No memory dimm at address %02X\n", addr << 1);
		*sn = 0xffffffff;
		return CB_SUCCESS;
	}

	dram_type = smbus_ret & 0xff;

	/* Check if module is DDR4, DDR4 spd is 512 byte. */
	if (dram_type == SPD_DRAM_DDR4 && CONFIG_DIMM_SPD_SIZE > SPD_PAGE_LEN) {
		/* Switch to page 1 */
		smbus_write_byte(SPD_PAGE_1, 0, 0);

		for (i = 0; i < SPD_SN_LEN; i++)
			*((u8 *)sn + i) = smbus_read_byte(addr,
						i + DDR4_SPD_SN_OFF);

		/* Restore to page 0 */
		smbus_write_byte(SPD_PAGE_0, 0, 0);
	} else if (dram_type == SPD_DRAM_DDR3) {
		for (i = 0; i < SPD_SN_LEN; i++)
			*((u8 *)sn + i) = smbus_read_byte(addr,
							i + DDR3_SPD_SN_OFF);
	} else {
		printk(BIOS_ERR, "Unsupported dram_type\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}
