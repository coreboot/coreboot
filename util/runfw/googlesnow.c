/*
 * googlesnow - run google snow firmware in user mode
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdio.h>
#include <err.h>
#include <sys/mman.h>

#include <string.h>

enum addresses {
	regs = 0x10000000,
	sram = 0x2020000,
	sramsize = 0x77fff,
};

// 0x20234b0
main(int argc, char *argv[])
{
	int fd;
	int devmem;
	void *regbase;
	void *srambase;
	void (*coreboot)(void) = (void *)0x2023400;

	if (argc != 3)
		errx(1, "usage: runbios rom-file start-address");

	fd = open(argv[1], 2);
	if (fd < 0)
		err(1, argv[1]);
	devmem = open("/dev/mem", 2);
	if (devmem < 0)
		err(1, argv[1]);
	srambase = mmap((void *)sram, sramsize, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (-1 == (int)sram)
		err(1, "mmap");
	memset((void *)sram, 0xff, sramsize);

	/* now sort of do what bl1 does: read the 'rom' into the sram */
	/* skip the bl1. We don't run it. */
	if (pread(fd, coreboot, 30*1024, 0x2000) < 30*1024)
		err(1, "read 'rom'");
	/* you want to be in gdb at this point. */
	coreboot = (void *) (strtoul(argv[2], 0, 0));
	coreboot();
}
#if 0
/* cc -g googlesnow.c */
/* typical run
gdb a.out
(gdb) list 50

45              i2c = mmap((void *)0x12c60000, 0x10000, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, devmem, (off_t) 0x12c60000);
46              if (-1 == (int)i2c)
47                      err(1, "mmap i2c");
48
49              /* now sort of do what bl1 does: read the 'rom' into the sram */
50              if (pread(fd, coreboot, 30*1024, 0x2000) < 30*1024)
51                      err(1, "read 'rom'");
52              /* you want to be in gdb at this point. */
53              coreboot = (void *) strtoull(argv[2], 0, 0);
54              coreboot();
(gdb) br 54
Breakpoint 3 at 0x8798: file runbios.c, line 54.
(gdb) run ~/coreboot/build/coreboot.rom  0x020234b0
The program being debugged has been started already.
Start it from the beginning? (y or n) y

Starting program: /root/coreboot/util/runarmbios/a.out ~/coreboot/build/coreboot.rom  0x020234b0

Breakpoint 3, main (argc=3, argv=0x7efff7c4) at runbios.c:54
54              coreboot();
(gdb) symbol-file ~/coreboot/build/cbfs/fallback/bootblock.debug
Load new symbol table from "/root/coreboot/build/cbfs/fallback/bootblock.debug"? (y or n) y
Reading symbols from /root/coreboot/build/cbfs/fallback/bootblock.debug...done.
Error in re-setting breakpoint 3: No source file named runbios.c.
(gdb) display/i $pc
1: x/i $pc
=> 0x8798:      ldr     r3, [r11, #-8]
(gdb) stepi
0x0000879c in ?? ()
1: x/i $pc
=> 0x879c:      blx     r3
(gdb)
0x020234b0 in call_bootblock ()
1: x/i $pc
=> 0x20234b0 <call_bootblock>:  ldr     sp, [pc, #3800] ; 0x2024390
(gdb)
0x020234b4 in call_bootblock ()
1: x/i $pc
=> 0x20234b4 <call_bootblock+4>:        bic     sp, sp, #7
(gdb)
0x020234b8 in call_bootblock ()
1: x/i $pc
=> 0x20234b8 <call_bootblock+8>:        mov     r0, #0
(gdb)
0x020234bc in call_bootblock ()
1: x/i $pc
=> 0x20234bc <call_bootblock+12>:       blx     0x20244b8 <main>
(gdb)
main (bist=0) at src/arch/armv7/bootblock_simple.c:37
37      {
1: x/i $pc
=> 0x20244b8 <main>:    push    {r3, lr}
(gdb)
42                      bootblock_mainboard_init();
1: x/i $pc
=> 0x20244ba <main+2>:  bl      0x2023c08 <bootblock_mainboard_init>
(gdb)
bootblock_mainboard_init () at src/mainboard/google/snow/bootblock.c:2124
2124    {
1: x/i $pc
=> 0x2023c08 <bootblock_mainboard_init>:        stmdb   sp!, {r4, r5, r6, r7, r9, r10, r11, lr}
(gdb)
2127            i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
1: x/i $pc
=> 0x2023c0c <bootblock_mainboard_init+4>:      movs    r1, #0

Note you are now debugging source, and pretty much all commands work.
*/
#endif
