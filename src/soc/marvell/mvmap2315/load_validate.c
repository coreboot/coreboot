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
#include <soc/digest.h>
#include <soc/flash.h>
#include <soc/load_validate.h>
#include <soc/reset.h>

static void set_flash_parameters(struct flash_params *flash_info,
				 struct bdb_hash *image_info)
{
	(*flash_info).offset
		= (u32)(image_info->offset);
	(*flash_info).buff
		= (u32)(image_info->load_address);
	(*flash_info).size = image_info->size;
	(*flash_info).id = 0x0;
	(*flash_info).partition = (u32)image_info->partition;
}

static void image_failure(void)
{
	struct mvmap2315_bootrom_info *bootrom_info
		= (void *)read32(
			&mvmap2315_mcu_secconfig->boot_callback_pointer);

	printk(BIOS_DEBUG, "Resetting system!!\n");

	if ((bootrom_info->sp_ro_status & MVMAP2315_BDB_TYPE)
					== PRIMARY_BDB) {
		write32(&mvmap2315_mcu_secconfig->pwr_clr_fail_a, 1);

		if (read32(
		&mvmap2315_mcu_secconfig->pwr_clr_fail_b)) {
			printk(BIOS_DEBUG, "primary & ALT BDB boot failed\n");
			printk(BIOS_DEBUG, "Rebooting to recovery mode...\n");
			write32(&mvmap2315_mcu_secconfig->pwr_clr_recovery, 1);
		} else {
			printk(BIOS_DEBUG, "primary BDB boot failed\n");
			printk(BIOS_DEBUG, "Rebooting using AlT BDB...\n");
			write32(&mvmap2315_mcu_secconfig->pwr_clr_bdb, 1);
		}
	} else if ((bootrom_info->sp_ro_status & MVMAP2315_BDB_TYPE)
					== ALTERNATE_BDB) {
		write32(&mvmap2315_mcu_secconfig->pwr_clr_fail_b, 1);

		if (read32(
		&mvmap2315_mcu_secconfig->pwr_clr_fail_a)) {
			printk(BIOS_DEBUG, "primary & ALT BDB boot failed\n");
			printk(BIOS_DEBUG, "Rebooting to recovery mode...\n");
			write32(&mvmap2315_mcu_secconfig->pwr_clr_recovery, 1);
		} else {
			printk(BIOS_DEBUG, "AlT BDB boot failed\n");
			printk(BIOS_DEBUG, "Rebooting using primary BDB...\n");
			write32(&mvmap2315_mcu_secconfig->pwr_clr_bdb, 0);
		}
	} else if ((bootrom_info->sp_ro_status & MVMAP2315_BDB_TYPE)
					== RECOVERY_BDB) {
		printk(BIOS_DEBUG, "Recovery image has errors!!!\n");
		printk(BIOS_DEBUG, "Re-entering recovery mode...\n");
		write32(&mvmap2315_mcu_secconfig->pwr_clr_recovery, 1);
	}

	mvmap2315_reset();
}

void load_and_validate(struct bdb_pointer *bdb_info, u32 image_type)
{
	struct bdb_hash *image_info;
	struct flash_params flash_info;
	struct mvmap2315_bootrom_info *bootrom_info
		= (void *)read32(
			&mvmap2315_mcu_secconfig->boot_callback_pointer);
	u8 image_digest[32];

	image_info = find_bdb_image(bdb_info, image_type);

	if (!image_info)
		image_failure();

	set_flash_parameters(&flash_info, image_info);

	if (flash_init(bootrom_info->flash_media, MVMAP2315_MMC_CLK_MHZ))
		image_failure();

	if (flash_partition(bootrom_info->flash_media, &flash_info))
		image_failure();

	if (flash_read(bootrom_info->flash_media, &flash_info))
		image_failure();

	if (flash_shutdown(bootrom_info->flash_media))
		image_failure();

	if (digest_init())
		image_failure();

	if (digest_sha_hash(((u8 *)(flash_info.buff)), image_info->size,
			    &image_digest[0], 32))
		image_failure();

	if (digest_cmp(&image_digest[0], &image_info->digest[0], 32))
		image_failure();
}
