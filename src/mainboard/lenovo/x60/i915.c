/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <delay.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <arch/coreboot_tables.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <device/pci.h>
#include <ec/google/chromeec/ec.h>
#include <cbfs_core.h>

#include <cpu/x86/tsc.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/msr.h>
#include "i915io.h"

enum {
	vmsg = 1, vio = 2, vspin = 4,
};

static int verbose = 0;

static unsigned int *mmio;
static unsigned int graphics;
static unsigned short addrport;
static unsigned short dataport;
static unsigned int physbase;
int oprom_is_loaded;

#define READ32(addr) io_i915_READ32(addr)
#define WRITE32(val, addr) io_i915_WRITE32(val, addr)

static unsigned long io_i915_READ32(unsigned long addr)
{
       unsigned long val;
       outl(addr, addrport);
       val = inl(dataport);
       return val;
}

static void io_i915_WRITE32(unsigned long val, unsigned long addr)
{
       outl(addr, addrport);
       outl(val, dataport);
}


/*
2560
4 words per
4 *p
10240
4k bytes per page
4096/p
2.50
1700 lines
1700 * p
4250.00
PTEs
*/

static void
setgtt(int start, int end, unsigned long base, int inc)
{
        int i;

	for(i = start; i < end; i++){
                u32 word = base + i*inc;
                WRITE32(word|1,(i*4)|1);
        }
}

static char *regname(unsigned long addr)
{
	static char name[16];
	sprintf(name, "0x%lx", addr);
	return name;
}

static unsigned long tickspermicrosecond = 1795;
static unsigned long long globalstart;

static unsigned long
microseconds(unsigned long long start, unsigned long long end)
{
	unsigned long ret;
	ret = ((end - start)/tickspermicrosecond);
	return ret;
}

static unsigned long globalmicroseconds(void)
{
	return microseconds(globalstart, rdtscll());
}

extern struct iodef iodefs[];

static int i915_init_done = 0;

int vbe_mode_info_valid(void);
int vbe_mode_info_valid(void)
{
	return i915_init_done;
}

void fill_lb_framebuffer(struct lb_framebuffer *framebuffer);
void fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	printk(BIOS_SPEW, "fill_lb_framebuffer: graphics is %p\n", (void *)graphics);
	framebuffer->physical_address = graphics;
	framebuffer->x_resolution = 1024;
	framebuffer->y_resolution = 768;
	framebuffer->bytes_per_line = 4096;
	framebuffer->bits_per_pixel = 32;
	framebuffer->red_mask_pos = 16;
	framebuffer->red_mask_size = 8;
	framebuffer->green_mask_pos = 8;
	framebuffer->green_mask_size = 8;
	framebuffer->blue_mask_pos = 0;
	framebuffer->blue_mask_size = 8;
	framebuffer->reserved_mask_pos = 0;
	framebuffer->reserved_mask_size = 0;

}

int i915lightup(unsigned int physbase, unsigned int iobase, unsigned int mmio,
	unsigned int gfx);

int i915lightup(unsigned int pphysbase,
	unsigned int piobase,
	unsigned int pmmio,
	unsigned int pgfx)
{
	int i, prev = 0;
	struct iodef *id, *lastidread = 0;
	unsigned long u, t;
	static unsigned long times[4096];
	mmio = (void *)pmmio;
	addrport = piobase;
	dataport = addrport + 4;
	physbase = pphysbase;
	graphics = pgfx;
	printk(BIOS_SPEW,
		"i915lightup: graphics %p mmio %p "
		"addrport %04x physbase %08x\n",
			(void *)graphics, mmio, addrport, physbase);
	globalstart = rdtscll();

	/* state machine! */
	for(i = 0, id = iodefs; id->op; i++, id++){
		switch(id->op){
		case M:
			if (verbose & vmsg) printk(BIOS_SPEW, "%ld: %s\n",
				globalmicroseconds(), id->msg);
			break;
		case R:
			u = READ32(id->addr);
			if (verbose & vio)printk(BIOS_SPEW, "%s: Got %08lx, expect %08lx\n",
				regname(id->addr), u, id->data);
			/* we're looking for something. */
			if (lastidread->addr == id->addr){
				/* they're going to be polling.
				 * just do it 1000 times
				 */
				for(t = 0; t < 1000 && id->data != u; t++){
					u = READ32(id->addr);
				}
				if (verbose & vspin) printk(BIOS_SPEW,
						"%s: # loops %ld got %08lx want %08lx\n",
						regname(id->addr),
						t, u, id->data);
			}
			lastidread = id;
			break;
		case W:
			if (verbose & vio)printk(BIOS_SPEW, "%s: outl %08lx\n", regname(id->addr),
									id->data);
			WRITE32(id->data, id->addr);
			if (id->addr == PCH_PP_CONTROL){
				switch(id->data & 0xf){
					case 8: break;
					case 7: break;
					default: udelay(100000);
				}
			}
			break;
		case V:
			if (id->count < 8){
				prev = verbose;
				verbose = id->count;
			} else {
				verbose = prev;
			}
			break;
		case I:
			break;
		default:
			printk(BIOS_SPEW, "BAD TABLE, opcode %d @ %d\n", id->op, i);
			return -1;
		}
		if (id->udelay)
			udelay(id->udelay);
		times[i] = globalmicroseconds();
	}
	/* optional, we don't even want to take timestamp overhead
	 * if we can avoid it. */
	if (0)
	for(i = 0, id = iodefs; id->op; i++, id++){
		switch(id->op){
		case R:
			printk(BIOS_SPEW, "%ld: R %08lx\n", times[i], id->addr);
			break;
		case W:
			printk(BIOS_SPEW, "%ld: W %08lx %08lx\n", times[i],
				id->addr, id->data);
			break;
		}
	}

	setgtt(0, 8192, physbase, 1024);
	printk(BIOS_SPEW, "memset %p to 0xff for %d bytes\n",
				(void *)graphics, 8192*1024);
	memset((void *)graphics, 0xff, 8192*1024);

	printk(BIOS_SPEW, "%ld microseconds\n", globalmicroseconds());
	i915_init_done = 1;
	oprom_is_loaded = 1;
	return 0;
}
