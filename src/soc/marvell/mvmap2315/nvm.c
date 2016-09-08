/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <console/console.h>
#include <soc/bdb.h>
#include <soc/clock.h>
#include <soc/flash.h>
#include <soc/load_validate.h>
#include <soc/nvm.h>

struct flash_ops nvm_callbacks = {
	.init = (void *)MVMAP2315_FLASH_INIT,
	.read = (void *)MVMAP2315_FLASH_READ,
	.write = (void *)MVMAP2315_FLASH_WRITE,
};

static void set_nvm_parameters(struct flash_params *eeprom_info, u32 offset,
			       u32 *buffer, u32 size)
{
	eeprom_info->offset = offset;
	eeprom_info->buff = (u32)buffer;
	eeprom_info->size = size;
	eeprom_info->id = 0x0;
	eeprom_info->partition = 0x0;
}

u32 nvm_init(void)
{
	int rc;

	clrbits_le32(&mvmap2315_apmu_clk->apaonclk_sdmmc_clkgenconfig,
		     MVMAP2315_SDMMC_CLK_RSTN);
	setbits_le32(&mvmap2315_apmu_clk->apaonclk_sdmmc_clkgenconfig,
		     MVMAP2315_SDMMC_CLK_RSTN);

	rc = nvm_callbacks.init(MVMAP2315_EEPROM, 0, 0);

	if (rc)
		printk(BIOS_DEBUG, "nvm_init failed with rc=%x.\n", rc);

	return rc;
}

u32 nvm_read(u32 offset, u32 *buffer, u32 size)
{
	struct flash_params eeprom_read_info;
	u32 rc;

	set_nvm_parameters(&eeprom_read_info, offset, buffer, size);

	rc = nvm_init();

	if (rc)
		return rc;

	rc = nvm_callbacks.read(MVMAP2315_EEPROM, 0, &eeprom_read_info);

	if (rc)
		printk(BIOS_INFO, "nvm_read callback failed, rc=%x\n", rc);

	return rc;
}

u32 nvm_write(u32 offset, u32 *buffer, u32 size)
{
	struct flash_params eeprom_read_info;
	u32 rc;

	set_nvm_parameters(&eeprom_read_info, offset, buffer, size);

	rc = nvm_init();

	if (rc)
		return rc;

	rc = nvm_callbacks.write(MVMAP2315_EEPROM, 0, &eeprom_read_info);

	if (rc)
		printk(BIOS_INFO, "nvm_write callback failed, rc=%x\n", rc);

	return rc;
}

void nvm_lockdown(void)
{
	setbits_le32(&mvmap2315_mcu_secconfig->boot_hw_lockdown_nvm,
		     MVMAP2315_NVM_LOCKDOWN_FLAG);
}
