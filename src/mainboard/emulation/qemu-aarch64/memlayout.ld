/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mainboard/addressmap.h>
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
	REGION(flash, VIRT_FLASH_BASE, CONFIG_ROM_SIZE, 8)

	SYMBOL(secram_start, VIRT_SECRAM_BASE)
	REGION(ap_mailbox, VIRT_SECRAM_BASE, 32K, 8)
	BOOTBLOCK(VIRT_SECRAM_BASE + 0x8000, 64K)
	STACK(VIRT_SECRAM_BASE + 0x18000, 54K)
	CBFS_MCACHE(VIRT_SECRAM_BASE + 0x25800, 8K)
	FMAP_CACHE(VIRT_SECRAM_BASE + 0x27800, 2K)
	TIMESTAMP(VIRT_SECRAM_BASE + 0x28000, 1K)
	ROMSTAGE(VIRT_SECRAM_BASE + 0x29000, 128K)
	TTB(VIRT_SECRAM_BASE + 0x50000, 128K)
	/* Fixed by TF-A: BL31_BASE = BL31_LIMIT - 0x70000 */
	BL31(VIRT_SECRAM_BASE + 0x90000, 448K)
	RAMSTAGE(VIRT_SECRAM_BASE + 0x100000, 14M)
	POSTRAM_CBFS_CACHE(VIRT_SECRAM_BASE + 0xF00000, 1M)
	SYMBOL(secram_end, VIRT_SECRAM_BASE + VIRT_SECRAM_SIZE)

	DRAM_START(VIRT_DRAM_START)
	REGION(fdt, VIRT_DRAM_START, 1M, 8)
}
