/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * Memory map for qemu vexpress-a9 since
 * 6ec1588e09770ac7e9c60194faff6101111fc7f0 (Jul 2014):
 *
 * 0x0000_0000: NOR flash
 * 0x1000_0000: I/O map address
 * 0x6000_0000: RAM
 */

/*
 * This map is designed to work with new qemu vexpress memory layout and
 * with -bios option which neatly puts coreboot into flash and so payloads
 * can find CBFS and we don't risk overwriting CBFS.
 *
 * Prior to Jul 2014 qemu aliased 0 to begining of RAM instead of flash
 * and -bios was unusable as $pc pointed to 0 which was zero-filled as a
 * workaround we suggested using -kernel but this still had all the issues
 * of having fake-ROM in RAM. In fact it was even worse as fake ROM ends
 * up exactly at addresses needed to load Linux.
 */
SECTIONS
{
	/* TODO: does this thing emulate SRAM? */

	REGION(flash, 0, CONFIG_ROM_SIZE, 4K)

	DRAM_START(0x60000000)
	STACK(0x60000000, 64K)
	BOOTBLOCK(0x60010000, 128K)
	FMAP_CACHE(0x60030000, 4K)
	TIMESTAMP(0x60031000, 1K)
	CBFS_MCACHE(0x60031400, 7K)
	/* TODO: Implement MMU support and move TTB to a better location. */
	TTB(0x60034000, 16K)
	ROMSTAGE(0x60038000, 128K)
	RAMSTAGE(0x60060000, 16M)
	POSTRAM_CBFS_CACHE(0x61060000, 8M)
}
