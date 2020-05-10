/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <spd_bin.h>
#include <device/smbus_host.h>
#include <string.h>
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
				 do_smbus_read_word(SMBUS_IO_BASE, addr, i);
		else
			spd[i] = do_smbus_read_byte(SMBUS_IO_BASE, addr, i);
	}
}

/* return -1 if SMBus errors otherwise return 0 */
static int get_spd(u8 *spd, u8 addr)
{
	/* If address is not 0, it will return CB_ERR(-1) if no dimm */
	if (do_smbus_read_byte(SMBUS_IO_BASE, addr, 0) < 0) {
		printk(BIOS_INFO, "No memory dimm at address %02X\n",
			addr << 1);
		return -1;
	}
	smbus_read_spd(spd, addr);

	/* Check if module is DDR4, DDR4 spd is 512 byte. */
	if (spd[SPD_DRAM_TYPE] == SPD_DRAM_DDR4 &&
		CONFIG_DIMM_SPD_SIZE > SPD_PAGE_LEN) {
		/* Switch to page 1 */
		do_smbus_write_byte(SMBUS_IO_BASE, SPD_PAGE_1, 0, 0);
		smbus_read_spd(spd + SPD_PAGE_LEN, addr);
		/* Restore to page 0 */
		do_smbus_write_byte(SMBUS_IO_BASE, SPD_PAGE_0, 0, 0);
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
