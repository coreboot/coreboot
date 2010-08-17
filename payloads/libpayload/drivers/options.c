/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload.h>
#include <coreboot_tables.h>

static int options_checksum_valid(void)
{
	int i;
	int range_start = lib_sysinfo.cmos_range_start / 8;
	int range_end = lib_sysinfo.cmos_range_end / 8;
	int checksum_location = lib_sysinfo.cmos_checksum_location / 8;
	u16 checksum = 0, checksum_old;

	for(i = range_start; i <= range_end; i++) {
		checksum += nvram_read(i);
	}
	checksum = (~checksum)&0xffff;

	checksum_old = ((nvram_read(checksum_location)<<8) | nvram_read(checksum_location+1));

	return (checksum_old == checksum);
}

void fix_options_checksum(void)
{
	int i;
	int range_start = lib_sysinfo.cmos_range_start / 8;
	int range_end = lib_sysinfo.cmos_range_end / 8;
	int checksum_location = lib_sysinfo.cmos_checksum_location / 8;
	u16 checksum = 0;

	for(i = range_start; i <= range_end; i++) {
		checksum += nvram_read(i);
	}
	checksum = (~checksum)&0xffff;

	nvram_write((checksum >> 8), checksum_location);
	nvram_write((checksum & 0xff), checksum_location + 1);
}

static int get_cmos_value(u32 bitnum, u32 len, void *valptr)
{
	u8 *value = (u8 *)valptr;
	int offs = 0;
	u32 addr, bit;
	u8 reg8;

	/* Convert to byte borders */
	addr=(bitnum / 8);
	bit=(bitnum % 8);

	/* Handle single byte or less */
	if(len <= 8) {
		reg8 = nvram_read(addr);
		reg8 >>= bit;
		value[0] = reg8 & ((1 << len) -1);
		return 0;
	}

	/* When handling more than a byte, copy whole bytes */
	while (len > 0) {
		len -= 8;
		value[offs++]=nvram_read(addr++);
	}

	return 0;
}

int get_option(void *dest, char *name)
{
	struct cb_cmos_option_table *option_table = phys_to_virt(lib_sysinfo.option_table);
	struct cb_cmos_entries *cmos_entry;
	int len = strnlen(name, CMOS_MAX_NAME_LENGTH);

	/* cmos entries are located right after the option table */

	for (   cmos_entry = (struct cb_cmos_entries*)((unsigned char *)option_table + option_table->header_length);
		cmos_entry->tag == CB_TAG_OPTION;
		cmos_entry = (struct cb_cmos_entries*)((unsigned char *)cmos_entry + cmos_entry->size)) {
		if (memcmp((const char*)cmos_entry->name, name, len))
			continue;
		if(get_cmos_value(cmos_entry->bit, cmos_entry->length, dest))
			return 1;

		if(!options_checksum_valid())
			return 1;

		return 0;
	}

	printf("ERROR: No such CMOS option (%s)\n", name);
	return 1;
}
