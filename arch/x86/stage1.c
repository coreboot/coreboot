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
#include <cpu.h>
#include <globalvars.h>
#include <lar.h>
#include <string.h>
#include <tables.h>
#include <lib.h>
#include <mc146818rtc.h>
#include <cpu.h>
#include <multiboot.h>
#include <stage1.h>

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

/*
 * The name is slightly misleading because this is the initial stack pointer,
 * not the address of the first element on the stack.
 * NOTE: This function is very processor specific.
 */
void *bottom_of_stack(void)
{
	u32 onstack = (u32)&onstack;

	/* Check whether the variable onstack is inside the CAR stack area.
	 * If it is, assume we're still in CAR or the stack has not moved.
	 * Otherwise return initial %esp for the RAM-based stack location.
	 */
	if ((onstack >= CAR_STACK_BASE - CAR_STACK_SIZE) &&
	    (onstack < CAR_STACK_BASE))
		return (void *)CAR_STACK_BASE;
	/* OK, so current %esp is not inside the CAR stack area. */
	return (void *)RAM_STACK_BASE;
}

struct global_vars *global_vars(void)
{
	return *(struct global_vars **)(bottom_of_stack() - sizeof(struct global_vars *));
}

void global_vars_init(struct global_vars *globvars)
{
	memset(globvars, 0, sizeof(struct global_vars));
	*(struct global_vars **)(bottom_of_stack() - sizeof(struct global_vars *)) = globvars;
#ifdef CONFIG_CONSOLE_BUFFER
	/* Initialize the printk buffer. */
	printk_buffer_init();
#endif
	console_loglevel_init();

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

/** cycles
 * provide 64-bit high precision counter 
 * @returns Time in 64 bits
 */
u64 cycles(void)
{
	u64 ret;
	asm volatile ("rdtsc" : "=A" (ret));
	return ret;
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


static int run_address_multiboot(void *f, struct multiboot_info *mbi)
{
	int ret, dummy;
	__asm__ __volatile__ ("call *%4" : "=a" (ret), "=c" (dummy) : "a" (MB_MAGIC2), "b" (mbi), "c" (f) : "edx", "memory");
	return ret;
}

/**
 * This function is called from assembler code with its argument on the
 * stack. Force the compiler to generate always correct code for this case.
 * We have cache as ram running and can start executing code in C.
 * @param bist Built In Self Test, which is used to indicate status of self test.
 * bist is defined by the CPU hardware and is present in EAX on first instruction of coreboot. 
 * Its value is implementation defined.
 * @param init_detected This (optionally set) value is used on some platforms (e.g. k8) to indicate
 * that we are restarting after some sort of reconfiguration. Note that we could use it on geode but 
 * do not at present. 
 */
void __attribute__((stdcall)) stage1_phase1(u32 bist, u32 init_detected)
{
	struct global_vars globvars;
	int ret;
	struct mem_file archive;
	struct node_core_id me;

	post_code(POST_STAGE1_MAIN);

	/* before we do anything, we want to stop if we do not run
	 * on the bootstrap processor.
	 * stop_ap is responsible for NOT stopping the BSP
	 */
	stop_ap();

	/* Initialize global variables before we can think of using them.
	 */
	global_vars_init(&globvars);
	globvars.init_detected = init_detected;

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

	/* Switch the stack location from CAR to RAM, rebuild the stack,
	 * disable CAR and continue at stage1_phase3(). This is all wrapped in
	 * stage1_phase2() to make the code easier to follow.
	 * We will NEVER return.
	 */
	stage1_phase2();

	/* If we reach this point, something went terribly wrong. */
	die("The world is broken.\n");
}

/**
 * This function is called to take care of switching and rebuilding the stack
 * so that we can cope with processors which don't support a CAR area at low
 * addresses where CAR could be copied to RAM without problems.
 * This function handles everything related to switching off CAR and moving
 * important data from CAR to RAM.
 * 1.  Perform all work which can be done while CAR and RAM are both active.
 *     That's mainly moving the printk buffer around.
 * 2a. Optionally back up the new stack location (desirable for S3).
 * 2b. Optionally rebuild the stack at another location.
 * 2c. Switch stack pointer to the new stack if the stack was rebuilt.
 * 3.  Disable CAR.
 * 4.  Call or jump to stage1_phase3.
 * Steps 2a-4 have to be done in asm. That's what the oddly named disable_car()
 * function does.
 *
 * TODO: Some parts of the list above are not yet done, so the code will not
 * yet work on C7.
 */
void stage1_phase2()
{
#ifdef CONFIG_CONSOLE_BUFFER
	/* Move the printk buffer to PRINTK_BUF_ADDR_RAM */
	printk_buffer_move((void *)PRINTK_BUF_ADDR_RAM, PRINTK_BUF_SIZE_RAM);
#endif
	/* Turn off Cache-As-Ram */
	disable_car();

	/* If we reach this point, something went terribly wrong. */
	die("The world is broken.\n");
}

/**
 * This function is the second part of the former stage1_main() after
 * switching the stack and disabling CAR.
 */
void __attribute__((stdcall)) stage1_phase3()
{
	void *entry;
	int ret;
	struct mem_file archive;
	struct multiboot_info *mbi;

#ifdef CONFIG_PAYLOAD_ELF_LOADER
	struct mem_file result;
	int elfboot_mem(struct lb_memory *mem, void *where, int size);

	/* Why can't we statically init this hack? */
	unsigned char faker[64];
	struct lb_memory *mem = (struct lb_memory*) faker;

	mem->tag = LB_TAG_MEMORY;
	mem->size = 28;
	mem->map[0].start.lo = mem->map[0].start.hi = 0;
	mem->map[0].size.lo = (32*1024*1024);
	mem->map[0].size.hi = 0;
	mem->map[0].type = LB_MEM_RAM;
#endif /* CONFIG_PAYLOAD_ELF_LOADER */

	// location and size of image.
	init_archive(&archive);

	entry = load_file_segments(&archive, "normal/stage2");
	if (entry == (void *)-1)
		die("FATAL: Failed loading stage2.");
	mbi = run_address(entry);
	if (! mbi)
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
		run_address_multiboot(entry, mbi);
	} else {
		die("FATAL: No usable payload found.\n");
	}
	die ("FATAL: Last stage returned to coreboot.\n");
}


