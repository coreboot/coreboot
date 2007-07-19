/*
 * This file is part of the LinuxBIOS project.
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
#include <tables.h>
#include <lib.h>
#include <mc146818rtc.h>

#define UNCOMPRESS_AREA 0x60000

/* these prototypes should go into headers */
void uart_init(void);
void die(const char *msg);
int find_file(struct mem_file *archive, char *filename, struct mem_file *result);
void hardware_stage1(void);
void disable_car(void);

void post_code(u8 value)
{
	outb(value, 0x80);
}

static void stop_ap(void)
{
	// nothing yet
	post_code(0xf0);
}

static void enable_rom(void)
{
	// nothing here yet
	post_code(0xf2);
}

/*
 * This function is called from assembler code whith its argument on the
 * stack. Force the compiler to generate always correct code for this case.
 */
void __attribute__((stdcall)) stage1_main(u32 bist)
{
	int ret;
	struct mem_file archive, result;
	int elfboot_mem(struct lb_memory *mem, void *where, int size);

	/* we can't statically init this hack. */
	unsigned char faker[64];
	struct lb_memory *mem = (struct lb_memory*) faker;

	mem->tag = LB_TAG_MEMORY;
	mem->size = 28;
	mem->map[0].start.lo = mem->map[0].start.hi = 0;
	mem->map[0].size.lo = (32*1024*1024);
	mem->map[0].size.hi = 0;
	mem->map[0].type = LB_MEM_RAM;


	post_code(0x02);

	// before we do anything, we want to stop if we dont run
	// on the bootstrap processor.
	if (bist==0) {
		// stop secondaries
		stop_ap();
	}

	// We have cache as ram running and can start executing code in C.
	//

	hardware_stage1();

	//
	uart_init();	// initialize serial port
	console_init(); // print banner

	if (bist!=0) {
		printk(BIOS_INFO, "BIST FAILED: %08x", bist);
		die("");
	}

	// enable rom
	enable_rom();

	// location and size of image.

	// FIXME this should be defined in the VPD area
	// but NOT IN THE CODE.

	/* The len field starts behind the reset vector on x86.
	 * The start is not correct for all platforms. sc520 will
	 * need some hands on here.
	 */
	archive.len = *(u32 *)0xfffffff4;
	archive.start =(void *)(0UL-archive.len);

	// FIXME check integrity


	// find first initram
	if (check_normal_boot_flag()) {
		printk(BIOS_DEBUG, "Choosing normal boot.\n");
		ret = execute_in_place(&archive, "normal/initram");
	} else {
		printk(BIOS_DEBUG, "Choosing fallback boot.\n");
		ret = execute_in_place(&archive, "fallback/initram");
		/* Try a normal boot if fallback doesn't exists in the lar.
		 * TODO: There are other ways to do this.
		 * It could be ifdef or the boot flag could be forced.
		 */
		if (ret) {
			printk(BIOS_DEBUG, "Fallback failed. Try normal boot\n");
			ret = execute_in_place(&archive, "normal/initram");
		}
	}

	if (ret)
		die("Failed RAM init code\n");

	printk(BIOS_DEBUG, "Done RAM init code\n");


	/* Turn off Cache-As-Ram */
	disable_car();

	ret = run_file(&archive, "normal/stage2", (void *)0x1000);
	if (ret)
		die("FATAL: Failed in stage2 code.");

	printk(BIOS_DEBUG, "Stage2 code done.\n");

	ret = find_file(&archive, "normal/payload", &result);
	if (ret) {
		printk(BIOS_ERR, "No such file '%s'.\n", "normal/payload");
		die("FATAL: No payload found.\n");
	}
	ret = copy_file(&archive, "normal/payload", (void *)UNCOMPRESS_AREA);
	if (ret) {
		printk(BIOS_ERR, "'%s' found, but could not load it.\n", "normal/payload");
		die("FATAL: No usable payload found.\n");
	}

	ret =  elfboot_mem(mem, (void *)UNCOMPRESS_AREA, result.reallen);

	printk(BIOS_ERR, "elfboot_mem returns %d\n", ret);

	die ("FATAL: Last stage returned to LinuxBIOS.\n");
}


