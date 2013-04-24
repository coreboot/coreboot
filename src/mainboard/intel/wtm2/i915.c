/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
#include <stdlib.h>
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
#include <boot/coreboot_tables.h>
#include "hda_verb.h"
#include <smbios.h>
#include <device/pci.h>
#include <ec/google/chromeec/ec.h>
#include <cbfs_core.h>

#include <cpu/x86/tsc.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <edid.h>
#include <device/i915.h>

/* how many bytes do we need for the framebuffer?
 * Well, this gets messy. To get an exact answer, we have
 * to ask the panel, but we'd rather zero the memory
 * and set up the gtt while the panel powers up. So,
 * we take a reasonable guess, secure in the knowledge that the
 * MRC has to overestimate the number of bytes used.
 * 8 MiB is a very safe guess. There may be a better way later, but
 * fact is, the initial framebuffer is only very temporary. And taking
 * a little long is ok; this is done much faster than the AUX
 * channel is ready for IO.
 */
#define FRAME_BUFFER_BYTES (8*MiB)
/* how many 4096-byte pages do we need for the framebuffer?
 * There are hard ways to get this, and easy ways:
 * there are FRAME_BUFFER_BYTES/4096 pages, since pages are 4096
 * on this chip (and in fact every Intel graphics chip we've seen).
 */
#define FRAME_BUFFER_PAGES (FRAME_BUFFER_BYTES/(4096))

static int verbose = 0;

static unsigned int *mmio;
static unsigned int graphics;
static unsigned short addrport;
static unsigned short dataport;
static unsigned int physbase;
extern int oprom_is_loaded;

const u32 link_edid_data[] = {
	0xffffff00, 0x00ffffff, 0x0379e430, 0x00000000,
	0x04011500, 0x96121ba5, 0xa2d54f02, 0x26935259,
	0x00545017, 0x01010000, 0x01010101, 0x01010101,
	0x01010101, 0x6f6d0101, 0xa4a0a000, 0x20306031,
	0xb510003a, 0x19000010, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x4c00fe00,
	0x69442047, 0x616c7073, 0x20200a79, 0xfe000000,
	0x31504c00, 0x45513932, 0x50532d31, 0x24003141,
};

static int ioread = 0, iowrite = 0;

static char *regname(unsigned long addr)
{
	static char name[16];
	sprintf(name, "0x%lx", addr);
	return name;
}

unsigned long io_i915_read32(unsigned long addr)
{
	unsigned long val;
	outl(addr, addrport);
	val = inl(dataport);
	ioread += 2;
	if (verbose & vio)printk(BIOS_SPEW, "%s: Got %08lx\n", regname(addr), val);
	return val;
}

void io_i915_write32(unsigned long val, unsigned long addr)
{
	if (verbose & vio)printk(BIOS_SPEW, "%s: outl %08lx\n", regname(addr), val);
	outl(addr, addrport);
	outl(val, dataport);
	iowrite += 2;
}

/* GTT is the Global Translation Table for the graphics pipeline.
 * It is used to translate graphics addresses to physical
 * memory addresses. As in the CPU, GTTs map 4K pages.
 * The setgtt function adds a further bit of flexibility:
 * it allows you to set a range (the first two parameters) to point
 * to a physical address (third parameter);the physical address is
 * incremented by a count (fourth parameter) for each GTT in the
 * range.
 * Why do it this way? For ultrafast startup,
 * we can point all the GTT entries to point to one page,
 * and set that page to 0s:
 * memset(physbase, 0, 4096);
 * setgtt(0, 4250, physbase, 0);
 * this takes about 2 ms, and is a win because zeroing
 * the page takes a up to 200 ms.
 * This call sets the GTT to point to a linear range of pages
 * starting at physbase.
 */

static void
setgtt(int start, int end, unsigned long base, int inc)
{
	int i;

	for(i = start; i < end; i++){
		u32 word = base + i*inc;
		io_i915_write32(word|1,(i*4)|1);
	}
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

static int i915_init_done = 0;

int i915lightup(unsigned int physbase, unsigned int iobase, unsigned int mmio,
		unsigned int gfx);

int i915lightup(unsigned int pphysbase, unsigned int piobase,
		unsigned int pmmio, unsigned int pgfx)
{
	int must_cycle_power = 0;

	/* frame buffer pointer */
	u32 *l;
	int i;
	unsigned long before_gtt, after_gtt;

	mmio = (void *)pmmio;
	addrport = piobase;
	dataport = addrport + 4;
	physbase = pphysbase;
	graphics = pgfx;
	printk(BIOS_SPEW,
	       "i915lightup: graphics %p mmio %p"
	       "addrport %04x physbase %08x\n",
	       (void *)graphics, mmio, addrport, physbase);
	globalstart = rdtscll();

	/* turn it on. The VBIOS does it this way, so we hope that's ok. */
	verbose = 0;
	io_i915_write32(0xabcd000f, PCH_PP_CONTROL);

	/* the AUX channel needs a small amount of time to spin up.
	 * Rather than udelay, do some useful work:
	 * Zero out the frame buffer memory,
	 * and set the global translation table (GTT)
	 */
	printk(BIOS_SPEW, "Set not-White (%08x) for %d pixels\n", 0xffffff,
	       FRAME_BUFFER_BYTES/sizeof(u32));
	for(l = (u32 *)graphics, i = 0;
		i < FRAME_BUFFER_BYTES/sizeof(u32); i++){
		l[i] = 0x1122ff;
	}
	printk(BIOS_SPEW, "GTT: set %d pages starting at %p\n",
				FRAME_BUFFER_PAGES, (void *)physbase);
	before_gtt = globalmicroseconds();
	setgtt(0, FRAME_BUFFER_PAGES, physbase, 4096);
	after_gtt = globalmicroseconds();

	/* The reset is basically harmless, and can be
	 * repeated by the VBIOS in any event.
	 */

	graphics_register_reset(DPA_AUX_CH_CTL, DPA_AUX_CH_DATA1, verbose);

	/* failures after this point can return without
	 * powering off the panel.
	 */

	if (1)
		goto fail;
	/* failures after this point MUST power off the panel
	 * and wait 600 ms.
	 */

	i915_init_done = 1;
	oprom_is_loaded = 1;
	return 1;

fail:
	printk(BIOS_SPEW, "Graphics could not be started;");
	if (must_cycle_power){
		printk(BIOS_SPEW, "Turn off power and wait ...");
		io_i915_write32(0xabcd0000, PCH_PP_CONTROL);
		udelay(600000);
	}
	printk(BIOS_SPEW, "Returning.\n");
	return 0;

}
