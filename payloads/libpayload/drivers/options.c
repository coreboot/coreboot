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

u8 *mem_accessor_base;

static u8 mem_read(u8 reg)
{
	return mem_accessor_base[reg];
}

static void mem_write(u8 val, u8 reg)
{
	mem_accessor_base[reg] = val;
}

struct nvram_accessor *use_nvram = &(struct nvram_accessor) {
	nvram_read,
	nvram_write
};

struct nvram_accessor *use_mem = &(struct nvram_accessor) {
	mem_read,
	mem_write
};

struct cb_cmos_option_table *get_system_option_table(void)
{
	return phys_to_virt(lib_sysinfo.option_table);
}

static int options_checksum_valid(const struct nvram_accessor *nvram)
{
	int i;
	int range_start = lib_sysinfo.cmos_range_start / 8;
	int range_end = lib_sysinfo.cmos_range_end / 8;
	int checksum_location = lib_sysinfo.cmos_checksum_location / 8;
	u16 checksum = 0, checksum_old;

	for(i = range_start; i <= range_end; i++) {
		checksum += nvram->read(i);
	}

	checksum_old = ((nvram->read(checksum_location)<<8) | nvram->read(checksum_location+1));

	return (checksum_old == checksum);
}

void fix_options_checksum_with(const struct nvram_accessor *nvram)
{
	int i;
	int range_start = lib_sysinfo.cmos_range_start / 8;
	int range_end = lib_sysinfo.cmos_range_end / 8;
	int checksum_location = lib_sysinfo.cmos_checksum_location / 8;
	u16 checksum = 0;

	for(i = range_start; i <= range_end; i++) {
		checksum += nvram->read(i);
	}

	nvram->write((checksum >> 8), checksum_location);
	nvram->write((checksum & 0xff), checksum_location + 1);
}

void fix_options_checksum(void)
{
	fix_options_checksum_with(use_nvram);
}

static int get_cmos_value(const struct nvram_accessor *nvram, u32 bitnum, u32 len, void *valptr)
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
		reg8 = nvram->read(addr);
		reg8 >>= bit;
		value[0] = reg8 & ((1 << len) -1);
		return 0;
	}

	/* When handling more than a byte, copy whole bytes */
	while (len > 0) {
		len -= 8;
		value[offs++]=nvram->read(addr++);
	}

	return 0;
}

static int set_cmos_value(const struct nvram_accessor *nvram, u32 bitnum, u32 len, void *valptr)
{
	u8 *value = (u8 *)valptr;
	int offs = 0;
	u32 addr, bit;
	u8 reg8;

	/* Convert to byte borders */
	addr=(bitnum / 8);
	bit=(bitnum % 8);

	/* Handle single byte or less */
	if (len <= 8) {
		reg8 = nvram->read(addr);
		reg8 &= ~(((1 << len) - 1) << bit);
		reg8 |= (value[0] & ((1 << len) - 1)) << bit;
		nvram->write(reg8, addr);
		return 0;
	}

	/* When handling more than a byte, copy whole bytes */
	while (len > 0) {
		len -= 8;
		nvram->write(value[offs++], addr++);
	}

	return 0;
}

static struct cb_cmos_entries *lookup_cmos_entry(struct cb_cmos_option_table *option_table, char *name)
{
	struct cb_cmos_entries *cmos_entry;
	int len = strnlen(name, CMOS_MAX_NAME_LENGTH);

	/* cmos entries are located right after the option table */

	for (   cmos_entry = (struct cb_cmos_entries*)((unsigned char *)option_table + option_table->header_length);
		cmos_entry->tag == CB_TAG_OPTION;
		cmos_entry = (struct cb_cmos_entries*)((unsigned char *)cmos_entry + cmos_entry->size)) {
		if (memcmp((const char*)cmos_entry->name, name, len))
			continue;
		return cmos_entry;
	}

	printf("ERROR: No such CMOS option (%s)\n", name);
	return NULL;
}

int get_option_with(const struct nvram_accessor *nvram, struct cb_cmos_option_table *option_table, void *dest, char *name)
{
	struct cb_cmos_entries *cmos_entry = lookup_cmos_entry(option_table, name);
	if (cmos_entry) {
		if(get_cmos_value(nvram, cmos_entry->bit, cmos_entry->length, dest))
			return 1;

		if(!options_checksum_valid(nvram))
			return 1;

		return 0;
	}
	return 1;
}

int get_option_from(struct cb_cmos_option_table *option_table, void *dest, char *name)
{
	return get_option_with(use_nvram, option_table, dest, name);
}

int get_option(void *dest, char *name)
{
	return get_option_from(get_system_option_table(), dest, name);
}

int set_option_with(const struct nvram_accessor *nvram, struct cb_cmos_option_table *option_table, void *value, char *name)
{
	struct cb_cmos_entries *cmos_entry = lookup_cmos_entry(option_table, name);
	if (cmos_entry) {
		set_cmos_value(nvram, cmos_entry->bit, cmos_entry->length, value);
		fix_options_checksum_with(nvram);
		return 0;
	}
	return 1;
}

int set_option(void *value, char *name)
{
	return set_option_with(use_nvram, get_system_option_table(), value, name);
}
