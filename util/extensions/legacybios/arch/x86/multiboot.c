/*
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#include "config.h"
#include "types.h"
#include "multiboot.h"

/* Multiboot: Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

#ifdef DEBUG_CONSOLE
void cls(void);
void printk(const char *fmt, ...);
int uart_init(int port, unsigned long speed);
#endif

void legacybios(ucell romstart, ucell romend);

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR.  */

void cmain(unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

#ifdef DEBUG_CONSOLE
	uart_init(SERIAL_PORT, SERIAL_SPEED);
	/* Clear the screen.  */
	cls();
#endif

	/* Am I booted by a Multiboot-compliant boot loader?  */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printk("legacybios: loader not multiboot capable\n");
		return;
	}

	/* Set MBI to the address of the Multiboot information structure.  */
	mbi = (multiboot_info_t *) addr;

#ifdef DEBUG_BOOT
	/* Print out the flags.  */
	printk("flags = 0x%x\n", mbi->flags);

	/* Are mem_* valid?  */
	if (CHECK_FLAG(mbi->flags, 0))
		printk("mem_lower = %dKB, mem_upper = %dKB\n",
		       mbi->mem_lower, mbi->mem_upper);

	/* Is boot_device valid?  */
	if (CHECK_FLAG(mbi->flags, 1))
		printk("boot_device = 0x%x\n", mbi->boot_device);

	/* Is the command line passed?  */
	if (CHECK_FLAG(mbi->flags, 2))
		printk("cmdline = %s\n", (char *) mbi->cmdline);
#endif

#ifdef DEBUG_BOOT
	/* Bits 4 and 5 are mutually exclusive!  */
	if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
		printk("panic: binary claims to be a.out and elf.\n");
		return;
	}
#endif

	/* Are mods_* valid?  */
	if (CHECK_FLAG(mbi->flags, 3)) {
		module_t *mod;
		unsigned int i;
#ifdef DEBUG_BOOT
		printk("mods_count = %d, mods_addr = 0x%x\n",
		       mbi->mods_count, mbi->mods_addr);
#endif
		for (i = 0, mod = (module_t *) mbi->mods_addr;
		     i < mbi->mods_count; i++, mod += sizeof(module_t)) {
			legacybios(mod->mod_start,  mod->mod_end);
#ifdef DEBUG_BOOT
			printk
			    (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
			     mod->mod_start, mod->mod_end,
			     (char *) mod->string);
#endif
		}
	}

	return;
}
