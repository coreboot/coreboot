/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <soc/flash_controller.h>
#include <soc/romstage.h>

/*
 * Minimal set of commands to read WPSR from SPI.
 * Returns 0 on success, < 0 on failure.
 */
int early_spi_read_wpsr(u8 *sr)
{
	uint8_t rdsr;
	int ret = 0;

	spi_init();

	/* sending NULL for spiflash struct parameter since we are not
	 * calling HWSEQ read_status() call via Probe.
	 */
	ret = pch_hwseq_read_status(NULL, &rdsr);
	if (ret) {
		printk(BIOS_ERR, "SPI rdsr failed\n");
		return ret;
	}
	*sr = rdsr & WPSR_MASK_SRP0_BIT;

	return 0;
}
