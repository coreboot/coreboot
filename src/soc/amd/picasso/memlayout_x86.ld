/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>
#include <arch/header.ld>
#include <soc/psp_transfer.h>

#define EARLY_RESERVED_DRAM_START(addr)		SYMBOL(early_reserved_dram, addr)
#define EARLY_RESERVED_DRAM_END(addr)		SYMBOL(eearly_reserved_dram, addr)

#define PSP_SHAREDMEM_DRAM_START(addr)		SYMBOL(psp_sharedmem_dram, addr)
#define PSP_SHAREDMEM_DRAM_END(addr)		SYMBOL(epsp_sharedmem_dram, addr)

/*
 *
 *                     +--------------------------------+
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *   reserved_dram_end +--------------------------------+
 *                     |                                |
 *                     |       verstage (if reqd)       |
 *                     |          (VERSTAGE_SIZE)       |
 *                     +--------------------------------+ VERSTAGE_ADDR
 *                     |                                |
 *                     |            FSP-M               |
 *                     |         (FSP_M_SIZE)           |
 *                     +--------------------------------+ FSP_M_ADDR
 *                     |                                |X86_RESET_VECTOR = ROMSTAGE_ADDR + ROMSTAGE_SIZE - 0x10
 *                     |           romstage             |
 *                     |        (ROMSTAGE_SIZE)         |
 *                     +--------------------------------+ ROMSTAGE_ADDR
 *                     |           bootblock            |
 *                     |     (C_ENV_BOOTBLOCK_SIZE)     |
 *                     +--------------------------------+ BOOTBLOCK_ADDR
 *                     |          Unused hole           |
 *                     +--------------------------------+
 *                     |     FMAP cache (FMAP_SIZE)     |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + 0x40 + VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE + PRERAM_CBMEM_CONSOLE_SIZE + 0x200
 *                     |  Early Timestamp region (512B) |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + 0x40 + VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE + PRERAM_CBMEM_CONSOLE_SIZE
 *                     |      Preram CBMEM console      |
 *                     |   (PRERAM_CBMEM_CONSOLE_SIZE)  |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + 0x40 + VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE
 *                     |   PSP shared (vboot workbuf)   |
 *                     |(VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE)      |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + 0x40
 *                     |     Transfer Info Structure    |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE
 *                     |          APOB (64KiB)          |
 *                     +--------------------------------+ PSP_APOB_DRAM_ADDRESS
 *                     |        Early BSP stack         |
 *                     |   (EARLYRAM_BSP_STACK_SIZE)    |
 * reserved_dram_start +--------------------------------+ EARLY_RESERVED_DRAM_BASE
 *                     |              DRAM              |
 *                     +--------------------------------+ 0x100000
 *                     |           Option ROM           |
 *                     +--------------------------------+ 0xc0000
 *                     |           Legacy VGA           |
 *                     +--------------------------------+ 0xa0000
 *                     |              DRAM              |
 *                     +--------------------------------+ 0x0
 */
SECTIONS
{
	DRAM_START(0x0)

	EARLY_RESERVED_DRAM_START(CONFIG_EARLY_RESERVED_DRAM_BASE)

	EARLYRAM_STACK(., CONFIG_EARLYRAM_BSP_STACK_SIZE)
	REGION(apob, CONFIG_PSP_APOB_DRAM_ADDRESS, 64K, 1)

#if CONFIG(VBOOT)
	PSP_SHAREDMEM_DRAM_START(CONFIG_PSP_SHAREDMEM_BASE)
	_transfer_buffer = .;
	REGION(transfer_info, ., TRANSFER_INFO_SIZE, 4)
	VBOOT2_WORK(., VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE)
#endif

	PRERAM_CBMEM_CONSOLE(., CONFIG_PRERAM_CBMEM_CONSOLE_SIZE)
	TIMESTAMP(., TIMESTAMP_BUFFER_SIZE)
	FMAP_CACHE(., FMAP_SIZE)
#if CONFIG(VBOOT)
	_etransfer_buffer = .;
	PSP_SHAREDMEM_DRAM_END(CONFIG_PSP_SHAREDMEM_BASE + CONFIG_PSP_SHAREDMEM_SIZE)
#endif
	_ = ASSERT((CONFIG_BOOTBLOCK_ADDR + CONFIG_C_ENV_BOOTBLOCK_SIZE - 0x10) == CONFIG_X86_RESET_VECTOR, "Reset vector should be -0x10 from end of bootblock");
	_ = ASSERT(CONFIG_BOOTBLOCK_ADDR == ((CONFIG_BOOTBLOCK_ADDR + 0xFFFF) & 0xFFFF0000), "Bootblock must be 16 bit aligned");
	BOOTBLOCK(CONFIG_BOOTBLOCK_ADDR, CONFIG_C_ENV_BOOTBLOCK_SIZE)
	ROMSTAGE(CONFIG_ROMSTAGE_ADDR, CONFIG_ROMSTAGE_SIZE)
	REGION(fspm, CONFIG_FSP_M_ADDR, CONFIG_FSP_M_SIZE, 1)
#if CONFIG(VBOOT_SEPARATE_VERSTAGE)
	VERSTAGE(CONFIG_VERSTAGE_ADDR, CONFIG_VERSTAGE_SIZE)
#endif

	EARLY_RESERVED_DRAM_END(.)

	RAMSTAGE(CONFIG_RAMBASE, 8M)
}

#if ENV_BOOTBLOCK
/* Bootblock specific scripts which provide more SECTION directives. */
#include <cpu/x86/16bit/entry16.ld>
#include <cpu/x86/16bit/reset16.ld>
#endif  /* ENV_BOOTBLOCK */
