/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <memlayout.h>
#include <arch/header.ld>

/*
 * Memory map for QEMU virt machine since
 * a578cdfbdd8f9beff5ced52b7826ddb1669abbbf (June 2019):
 *
 * 0..128MiB (0x0000_0000..0x0080_0000) is the space for a flash device.
 * 128MiB..256MiB (0x0080_0000..0x0100_0000) is used for miscellaneous device I/O.
 * 256MiB..1GiB (0x0100_0000..0x4000_0000) is reserved for possible future PCI support.
 * 1GiB.. (0x4000_0000) is RAM and the size depends on initial RAM and device memory settings.
 */
SECTIONS
{
	REGION(flash, 0x00000000, CONFIG_ROM_SIZE, 8)

	BL31(0xe000000, 0x1000000)
	DRAM_START(0x40000000)
	REGION(fdt, 0x40000000, 1M, 8)
	BOOTBLOCK(0x60010000, 64K)
	STACK(0x60020000, 54K)
	CBFS_MCACHE(0x6002D800, 8K)
	FMAP_CACHE(0x6002F800, 2K)
        TIMESTAMP(0x60030000, 1K)
	ROMSTAGE(0x60031000, 128K)
	TTB(0x60070000, 128K)
	RAMSTAGE(0x600b0000, 16M)

	POSTRAM_CBFS_CACHE(0x61200000, 1M)
}
