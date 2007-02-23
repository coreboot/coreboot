/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>
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

#include <arch/types.h>
#include <arch/io.h>
#include <console/console.h>
#include <console/loglevel.h>
#include <lar.h>
#include <linuxbios_tables.h>
#include "config.h"

/* these prototypes should go into headers */
void uart_init(void);
void console_init(void);
void die(const char *msg);
int find_file(struct mem_file *archive, char *filename, struct mem_file *result);

// Is this value correct?
#define DCACHE_RAM_SIZE 0x8000


void post_code(u8 value)
{
	outb(value, 0x80);
}

static void stop_ap(void)
{
	// nothing yet
	post_code(0xf0);
}

static void enable_superio(void)
{
	// nothing yet
	post_code(0xf1);
}

static void enable_rom(void)
{
	// nothing here yet
	post_code(0xf2);
}

void stage1_main(u32 bist)
{
	int ret;
	struct mem_file archive, result;
	int elfboot_mem(struct lb_memory *mem, void *where, int size);

	/* we can't statically init this hack. */
	unsigned char *faker[64];
	struct lb_memory *mem = (struct lb_memory*) &faker;

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
	
	enable_superio();

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
	
	archive.len=LINUXBIOS_ROMSIZE_KB*1024;
	archive.start=(void *)(0UL-archive.len); 
	/* This won't work; the for loop in lib/lar.c will always 
	 * fail as adding len to start will be 0. 
	 * shave off 0x1000 since we know that is the boot block 
	 */
	archive.len-=0x1000;
printk(BIOS_INFO, "Start search at 0x%x, size %d\n", archive.start, archive.len);

	// FIXME check integrity


	// fixme: choose an initram image (normal/fallback)
	// find first initram
	ret = run_file(&archive, "normal/initram", (void *)(512*1024)); //CONFIG_CARBASE;

	if (ret)
		die("Failed ram init code\n");

	printk(BIOS_INFO, "Done ram init code\n");
	/* this is nasty. And, it has to be done this way. Sorry! */
	/* we have to turn off CAR, and do some other things, and it has to be done
	 * inline -- you can't call a function
	 */

        __asm__ volatile (
	/* 
	FIXME : backup stack in CACHE_AS_RAM into mmx and sse and after we get STACK up, we restore that.
		It is only needed if we want to go back
	*/
	
        /* We don't need cache as ram for now on */
        /* disable cache */
        "movl    %cr0, %eax\n\t"
        "orl    $(0x1<<30),%eax\n\t"
        "movl    %eax, %cr0\n\t"

        /* clear sth */
        "movl    $0x269, %ecx\n\t"  /* fix4k_c8000*/
        "xorl    %edx, %edx\n\t"
        "xorl    %eax, %eax\n\t"
	"wrmsr\n\t"
#if DCACHE_RAM_SIZE > 0x8000
	"movl    $0x268, %ecx\n\t"  /* fix4k_c0000*/
        "wrmsr\n\t"
#endif

        /* Set the default memory type and disable fixed and enable variable MTRRs */
        "movl    $0x2ff, %ecx\n\t"
//        "movl    $MTRRdefType_MSR, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        /* Enable Variable and Disable Fixed MTRRs */
        "movl    $0x00000800, %eax\n\t"
        "wrmsr\n\t"

#if defined(CLEAR_FIRST_1M_RAM)
        /* enable caching for first 1M using variable mtrr */
        "movl    $0x200, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0 | 1), %eax\n\t"
//	"movl     $(0 | MTRR_TYPE_WRCOMB), %eax\n\t"
        "wrmsr\n\t"

        "movl    $0x201, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" /* AMD 40 bit 0xff*/
        "movl    $((~(( 0 + 0x100000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"
#endif

        /* enable cache */
        "movl    %cr0, %eax\n\t"
        "andl    $0x9fffffff,%eax\n\t"
        "movl    %eax, %cr0\n\t"
#if defined(CLEAR_FIRST_1M_RAM)
        /* clear the first 1M */
        "movl    $0x0, %edi\n\t"
        "cld\n\t"
        "movl    $(0x100000>>2), %ecx\n\t"
        "xorl    %eax, %eax\n\t"
        "rep     stosl\n\t"

        /* disable cache */
        "movl    %cr0, %eax\n\t"
        "orl    $(0x1<<30),%eax\n\t"
        "movl    %eax, %cr0\n\t"

        /* enable caching for first 1M using variable mtrr */
        "movl    $0x200, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0 | 6), %eax\n\t"
//	"movl     $(0 | MTRR_TYPE_WRBACK), %eax\n\t"
        "wrmsr\n\t"

        "movl    $0x201, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" /* AMD 40 bit 0xff*/
        "movl    $((~(( 0 + 0x100000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"

        /* enable cache */
        "movl    %cr0, %eax\n\t"
        "andl    $0x9fffffff,%eax\n\t"
        "movl    %eax, %cr0\n\t"
	"invd\n\t"

	/* 
	FIXME: I hope we don't need to change esp and ebp value here, so we can restore value from mmx sse back
		But the problem is the range is some io related, So don't go back
	*/
#endif
        );

	ret = run_file(&archive, "normal/stage2", (void *)0x1000);
	if (ret)
		die("FATAL: Failed in stage2 code");

	printk(BIOS_INFO, "Done stage2 code\n");

	ret = find_file(&archive, "normal/payload", &result);
	if (ret) {
		printk(BIOS_INFO, "no such name %s\n", "payload");
		die("cachemain finding payload");
	}

	ret =  elfboot_mem(mem, result.start, result.len);

	printk(BIOS_INFO, "elfboot_mem returns %d\n", ret);

	die ("FATAL: This is as far as it goes\n");
}


