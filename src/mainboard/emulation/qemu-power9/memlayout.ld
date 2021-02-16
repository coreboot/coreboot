/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x0)

	/*
	 * On POWER, 0 is wherever HRMOR points to rather than physical DRAM start.
	 * HRMOR is ORed with address, not added to it, meaning that memory space
	 * overlaps after 2^(least significant set bit of HRMOR). This becomes
	 * chaotic when nonconsecutive bits are set...
	 *
	 * Two and a half possible cases:
	 * 0. bootblock started with QEMU in hb-mode
	 *    - NIA =                          0x10  (bug?)
	 *    - HRMOR =                  0x08000000  (128M)
	 *    - no physical memory to enable/train, everything accessible from start
	 * 1. bootblock loaded by HBBL
	 *    - NIA =                             0
	 *    - HRMOR =                  0xF8000000  (4G - 128M)
	 *    - initialized L3 =           0x400000  (4M)
	 *    - top address before RAM = 0xF8400000
	 * 2. bootblock in SEEPROM, loaded by SBE
	 *    - NIA =                        0x3000  (placeholder for int. vectors)
	 *    - HRMOR =                  0xF8200000  (4G - 128 M + 2 M)
	 *    - initialized L3 =             0x8000  (bootblock/HBBL size = 32K)
	 *    - no way 32K will be enough, must initialize more L3 in bootblock
	 *    - HRMOR still applies, so memory overlaps every 2M
	 *
	 * Common subset (assuming 2. initializes as much memory as possible) is
	 * 0xF8200000-0xF8400000. 2M should be more than enough for pre-RAM code,
	 * but it isn't enough to load ramstage. We could implement postcar stage,
	 * but KISS: initialize L3 from _ebootblock to 0xF8980000: up to 9.5M into
	 * cache, leaving bottom 2M (0xF8000000-0xF8200000) either uninitialized
	 * (when started from SEEPROM) or just unused for anything but bootblock
	 * (loaded by HBBL). Last 0.5M of L3 cache is left for interrupt vectors
	 * normally located at address 0.
	 *
	 * Set HRMOR to 0 before jumping to C code in bootblock and forget it even
	 * exists.
	 *
	 * For QEMU s/0xF8/0x08/ in above description but code remains the same.
	 * L3 initialization is unnecessary in this case but won't break anything.
	 *
	 * TODO: there is a structure with SBE->HBBL data at 0 in 2nd option. It
	 * holds some useful data like XSCOM BAR and LPC BAR. If, for any reason,
	 * these addresses are different than default, they should be used instead
	 * of predefined values.
	 */

#if !CONFIG(BOOTBLOCK_IN_SEEPROM)
	BOOTBLOCK(           0x08000000, 32K)
#else
	BOOTBLOCK(           0x08203000, 20K)
#endif

	STACK(               0x08208000, 32K)
	PRERAM_CBMEM_CONSOLE(0x08210000, 128K)
	FMAP_CACHE(          0x08230000, 4K)
	CBFS_MCACHE(         0x08231000, 8K)
	TIMESTAMP(           0x08233000, 4K)

	ROMSTAGE(            0x08240000, 256K)

	/*
	 * bootblock_crt0.S assumes this is the last part of L3, leaving for
	 * interrupt vectors at least 0.5M because of cache associativity. If
	 * more CBFS_CACHE is needed, split this into pre-/postram caches.
	 */
	CBFS_CACHE(          0x08280000, 7M)

	RAMSTAGE(            0x09000000, 2M)
}
