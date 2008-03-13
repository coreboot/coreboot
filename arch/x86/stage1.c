/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <io.h>
#include <console.h>
#include <lar.h>
#include <string.h>
#include <tables.h>
#include <lib.h>
#include <mc146818rtc.h>
#include <cpu.h>

#ifdef CONFIG_PAYLOAD_ELF_LOADER
/* ah, well, what a mess! This is a hard code. FIX ME but how? 
 * By getting rid of ELF ...
 */
#define UNCOMPRESS_AREA (0x400000)
#endif /* CONFIG_PAYLOAD_ELF_LOADER */

/* these prototypes should go into headers */
void uart_init(void);
void die(const char *msg);
void hardware_stage1(void);
void disable_car(void);
void mainboard_pre_payload(void);

static void stop_ap(void)
{
	// nothing yet
	post_code(POST_STAGE1_STOP_AP);
}

static void enable_rom(void)
{
	// nothing here yet
	post_code(POST_STAGE1_ENABLE_ROM);
}

void init_archive(struct mem_file *archive)
{
	// FIXME this should be defined in the VPD area
	// but NOT IN THE CODE.

	/* The len field starts behind the reset vector on x86.
	 * The start is not correct for all platforms. sc520 will
	 * need some hands on here.
	 */
	archive->len = *(u32 *)0xfffffff4;
	archive->start =(void *)(0UL-archive->len);

	// FIXME check integrity

}

void *bottom_of_stack(void)
{
	/* -4-4 because CONFIG_CARBASE + CONFIG_CARSIZE - 4 is initial %esp */
	return (void *)(CONFIG_CARBASE + CONFIG_CARSIZE - 4 - 4);
}

void dump_mem_range(int msg_level, unsigned char *buf, int size)
{
	int i;
	printk(msg_level, "dumping memrange %p size %i:\n", buf, size);
	for (i = 0; i < size; i++) {
		printk(msg_level, "%02x ", buf[i]);
		if (i % 16 == 15)
			printk(msg_level, "\n");
	}
	return;
}


#ifdef CONFIG_PAYLOAD_ELF_LOADER
/* until we get rid of elf */
int legacy(struct mem_file *archive, char *name, void *where, struct lb_memory *mem)
{
	int ret;
	int elfboot_mem(struct lb_memory *mem, void *where, int size);
	ret = copy_file(archive, name, where);
	if (ret) {
		printk(BIOS_ERR, "'%s' found, but could not load it.\n", name);
	}

	ret =  elfboot_mem(mem, where, archive->reallen);

	printk(BIOS_ERR, "elfboot_mem returns %d\n", ret);
	return -1;
}
#endif /* CONFIG_PAYLOAD_ELF_LOADER */

/*
 * This function is called from assembler code with its argument on the
 * stack. Force the compiler to generate always correct code for this case.
 */
void __attribute__((stdcall)) stage1_main(u32 bist)
{
	int ret;
	struct mem_file archive, result;
#ifdef CONFIG_PAYLOAD_ELF_LOADER
	int elfboot_mem(struct lb_memory *mem, void *where, int size);
#endif /* CONFIG_PAYLOAD_ELF_LOADER */
	void *entry;

	/* we can't statically init this hack. */
	unsigned char faker[64];
	struct lb_memory *mem = (struct lb_memory*) faker;

	mem->tag = LB_TAG_MEMORY;
	mem->size = 28;
	mem->map[0].start.lo = mem->map[0].start.hi = 0;
	mem->map[0].size.lo = (32*1024*1024);
	mem->map[0].size.hi = 0;
	mem->map[0].type = LB_MEM_RAM;


	post_code(POST_STAGE1_MAIN);

	// before we do anything, we want to stop if we dont run
	// on the bootstrap processor.
	if (bist==0) {
		// stop secondaries
		stop_ap();
	}

	// We have cache as ram running and can start executing code in C.

#ifdef CONFIG_CONSOLE_BUFFER
	/* Initialize the printk buffer. */
	printk_buffer_init();
#endif

	hardware_stage1();

	//
	uart_init();	// initialize serial port

	/* Exactly from now on we can use printk to the serial port.
	 * Celebrate this by printing a LB banner.
	 */
	console_init();

	if (bist!=0) {
		printk(BIOS_INFO, "BIST FAILED: %08x", bist);
		die("");
	}

	// enable rom
	enable_rom();

	// location and size of image.

	init_archive(&archive);

	// find first initram
	if (check_normal_boot_flag()) {
		printk(BIOS_DEBUG, "Choosing normal boot.\n");
		ret = execute_in_place(&archive, "normal/initram/segment0");
	} else {
		printk(BIOS_DEBUG, "Choosing fallback boot.\n");
		ret = execute_in_place(&archive, "fallback/initram/segment0");
		/* Try a normal boot if fallback doesn't exist in the lar.
		 * TODO: There are other ways to do this.
		 * It could be ifdef or the boot flag could be forced.
		 */
		if (ret) {
			printk(BIOS_DEBUG, "Fallback failed. Try normal boot\n");
			ret = execute_in_place(&archive, "normal/initram/segment0");
		}
	}

	if (ret)
		die("Failed RAM init code\n");

	printk(BIOS_DEBUG, "Done RAM init code\n");

	/* Turn off Cache-As-Ram */
	disable_car();

#ifdef CONFIG_CONSOLE_BUFFER
	/* Move the printk buffer to PRINTK_BUF_ADDR_RAM */
	printk_buffer_move((void *)PRINTK_BUF_ADDR_RAM, PRINTK_BUF_SIZE_RAM);
#endif

	entry = load_file_segments(&archive, "normal/stage2");
	if (entry == (void *)-1)
		die("FATAL: Failed loading stage2.");
	ret = run_address(entry);
	if (ret)
		die("FATAL: Failed in stage2 code.");

	printk(BIOS_DEBUG, "Stage2 code done.\n");

#ifdef CONFIG_PAYLOAD_ELF_LOADER
	ret = find_file(&archive, "normal/payload", &result);
	if (! ret)
		legacy(&archive, "normal/payload", (void *)UNCOMPRESS_AREA, mem);
#endif /* CONFIG_PAYLOAD_ELF_LOADER */

	entry = load_file_segments(&archive, "normal/payload");
	if (entry != (void*)-1) {
		/* Final coreboot call before handing off to the payload. */
		mainboard_pre_payload();
		run_address(entry);
	} else {
		die("FATAL: No usable payload found.\n");
	}
	die ("FATAL: Last stage returned to coreboot.\n");
}


