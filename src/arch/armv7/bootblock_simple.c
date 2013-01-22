/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <bootblock_common.h>
#include <arch/bootblock_exit.h>
#include <arch/cbfs.h>
#include <arch/hlt.h>

static int boot_cpu(void)
{
	/*
	 * FIXME: This is a stub for now. All non-boot CPUs should be
	 * waiting for an interrupt. We could move the chunk of assembly
	 * which puts them to sleep in here...
	 */
	return 1;
}

// TODO(hungte) Find a better way to enable cbfs code here.
// mem* and ulzma are now workarounds for bootblock compilation.

#include "lib/cbfs.c"

void *memcpy(void *dest, const void *src, size_t n) {
	char *d = (char *)dest;
	const char *s = (const char*)src;
	while (n-- > 0)
		*d++ = *s++;
	return dest;
}

void *memset(void *dest, int c, size_t n) {
	char *d = (char*)dest;
	while (n-- > 0)
		*d++ = c;
	return dest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t n) {
	const char *s1 = (const char*)ptr1, *s2 = (const char*)ptr2;
	int c;
	while (n-- > 0)
		if ((c = *s1++ - *s2++))
			return c;
	return 0;
}

unsigned long ulzma(unsigned char *src, unsigned char *dest) {
	// TODO remove this.
	return -1;
}

// end of stubs

// ROM media implementation.
static int snow_rom_open(struct cbfs_media *media) {
	return 0;
}

static int snow_rom_close(struct cbfs_media *media) {
	return 0;
}

void main(unsigned long bist)
{
	const char *target1 = "fallback/romstage";
	unsigned long romstage_entry;
	struct cbfs_media media = {
		.context = NULL,
		.open = snow_rom_open,
		.close = snow_rom_close,
	};

	if (boot_cpu()) {
		bootblock_cpu_init();
		bootblock_mainboard_init();
	}

	printk(BIOS_INFO, "bootblock main(): loading romstage\n");
	romstage_entry = (unsigned long)cbfs_load_stage(&media, target1);

	printk(BIOS_INFO, "bootblock main(): jumping to romstage\n");
	if (romstage_entry) bootblock_exit(romstage_entry);
	hlt();
}
