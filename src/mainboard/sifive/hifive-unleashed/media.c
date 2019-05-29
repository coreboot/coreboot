/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Jonathan Neuschäfer
 * Copyright (C) 2019 HardenedLinux
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

#include <boot_device.h>
#include <soc/spi.h>

/* At 0x20000000: A 256MiB long memory-mapped view of the flash at QSPI0 */
static struct mem_region_device mdev =
	MEM_REGION_DEV_RO_INIT((void *)0x20000000, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &mdev.rdev;
}

const static struct fu540_spi_mmap_config spi_mmap_config = {
	.cmd_en = 1,
	.addr_len = 4,
	.pad_cnt = 6,
	.cmd_proto = FU540_SPI_PROTO_S,
	.addr_proto = FU540_SPI_PROTO_Q,
	.data_proto = FU540_SPI_PROTO_Q,
	.cmd_code = 0xec,
	.pad_code = 0
};

void boot_device_init(void)
{
	struct spi_slave slave;

	/* initialize spi controller */
	spi_setup_slave(0, 0, &slave);

	/* map flash to memory space */
	fu540_spi_mmap(&slave, &spi_mmap_config);
}
