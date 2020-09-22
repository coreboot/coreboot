/*
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

#define __STDC_FORMAT_MACROS

#include <libpayload.h>
#include <coreboot_tables.h>
#include <inttypes.h>

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
	return phys_to_virt(lib_sysinfo.cmos_option_table);
}

int options_checksum_valid(const struct nvram_accessor *nvram)
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
	u8 *value = valptr;
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

static int set_cmos_value(const struct nvram_accessor *nvram, u32 bitnum, u32 len, const void *valptr)
{
	const u8 *value = valptr;
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

static struct cb_cmos_entries *lookup_cmos_entry(struct cb_cmos_option_table *option_table, const char *name)
{
	struct cb_cmos_entries *cmos_entry;
	int len = name ? strnlen(name, CB_CMOS_MAX_NAME_LENGTH) : 0;

	/* CMOS entries are located right after the option table */
	cmos_entry = first_cmos_entry(option_table);
	while (cmos_entry) {
		if (memcmp((const char*)cmos_entry->name, name, len) == 0)
			return cmos_entry;
		cmos_entry = next_cmos_entry(cmos_entry);
	}

	printf("ERROR: No such CMOS option (%s)\n", name);
	return NULL;
}

struct cb_cmos_entries *first_cmos_entry(struct cb_cmos_option_table *option_table)
{
	return (struct cb_cmos_entries*)((unsigned char *)option_table + option_table->header_length);
}

struct cb_cmos_entries *next_cmos_entry(struct cb_cmos_entries *cmos_entry)
{
	struct cb_cmos_entries *next = (struct cb_cmos_entries*)((unsigned char *)cmos_entry + cmos_entry->size);
	if (next->tag == CB_TAG_OPTION)
		return next;
	else
		return NULL;
}

struct cb_cmos_enums *first_cmos_enum(struct cb_cmos_option_table *option_table)
{
	struct cb_cmos_entries *cmos_entry;
	/* CMOS entries are located right after the option table. Skip them */
	cmos_entry = (struct cb_cmos_entries *)((unsigned char *)option_table + option_table->header_length);
	while (cmos_entry->tag == CB_TAG_OPTION)
		cmos_entry = (struct cb_cmos_entries*)((unsigned char *)cmos_entry + cmos_entry->size);

	/* CMOS enums are located after CMOS entries. */
	return (struct cb_cmos_enums *)cmos_entry;
}

struct cb_cmos_enums *next_cmos_enum(struct cb_cmos_enums *cmos_enum)
{
	if (!cmos_enum) {
		return NULL;
	}

	cmos_enum = (struct cb_cmos_enums*)((unsigned char *)cmos_enum + cmos_enum->size);
	if (cmos_enum->tag == CB_TAG_OPTION_ENUM) {
		return cmos_enum;
	} else {
		return NULL;
	}
}

struct cb_cmos_enums *next_cmos_enum_of_id(struct cb_cmos_enums *cmos_enum, int id)
{
	while ((cmos_enum = next_cmos_enum(cmos_enum))) {
		if (cmos_enum->config_id == id) {
			return cmos_enum;
		}
	}
	return NULL;
}

struct cb_cmos_enums *first_cmos_enum_of_id(struct cb_cmos_option_table *option_table, int id)
{
	struct cb_cmos_enums *cmos_enum = first_cmos_enum(option_table);
	if (!cmos_enum) {
		return NULL;
	}
	if (cmos_enum->config_id == id) {
		return cmos_enum;
	}

	return next_cmos_enum_of_id(cmos_enum, id);
}

/* Either value or text must be NULL. Returns the field that matches "the other" for a given config_id */
static struct cb_cmos_enums *lookup_cmos_enum_core(struct cb_cmos_option_table *option_table, int config_id, const u8 *value, const char *text)
{
	int len = strnlen(text, CB_CMOS_MAX_TEXT_LENGTH);

	/* CMOS enums are located after CMOS entries. */
	struct cb_cmos_enums *cmos_enum;
	for (   cmos_enum = first_cmos_enum_of_id(option_table, config_id);
		cmos_enum;
		cmos_enum = next_cmos_enum_of_id(cmos_enum, config_id)) {
		if (((value == NULL) || (cmos_enum->value == *value)) &&
		    ((text == NULL) || (memcmp((const char*)cmos_enum->text, text, len) == 0))) {
			return cmos_enum;
		}
	}

	return NULL;
}

static struct cb_cmos_enums *lookup_cmos_enum_by_value(struct cb_cmos_option_table *option_table, int config_id, const u8 *value)
{
	return lookup_cmos_enum_core(option_table, config_id, value, NULL);
}

static struct cb_cmos_enums *lookup_cmos_enum_by_label(struct cb_cmos_option_table *option_table, int config_id, const char *label)
{
	return lookup_cmos_enum_core(option_table, config_id, NULL, label);
}

int get_option_with(const struct nvram_accessor *nvram, struct cb_cmos_option_table *option_table, void *dest, const char *name)
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

int get_option_from(struct cb_cmos_option_table *option_table, void *dest, const char *name)
{
	return get_option_with(use_nvram, option_table, dest, name);
}

int get_option(void *dest, const char *name)
{
	return get_option_from(get_system_option_table(), dest, name);
}

int set_option_with(const struct nvram_accessor *nvram, struct cb_cmos_option_table *option_table, const void *value, const char *name)
{
	struct cb_cmos_entries *cmos_entry = lookup_cmos_entry(option_table, name);
	if (cmos_entry) {
		set_cmos_value(nvram, cmos_entry->bit, cmos_entry->length, value);
		fix_options_checksum_with(nvram);
		return 0;
	}
	return 1;
}

int set_option(const void *value, const char *name)
{
	return set_option_with(use_nvram, get_system_option_table(), value, name);
}

int get_option_as_string(const struct nvram_accessor *nvram, struct cb_cmos_option_table *option_table, char **dest, const char *name)
{
	void *raw;
	struct cb_cmos_entries *cmos_entry = lookup_cmos_entry(option_table, name);
	if (!cmos_entry)
		return 1;
	int cmos_length = (cmos_entry->length+7)/8;

	/* ensure we have enough space for u64 */
	if (cmos_length < 8)
		cmos_length = 8;

	/* extra byte to ensure 0-terminated strings */
	raw = malloc(cmos_length+1);
	memset(raw, 0, cmos_length+1);

	int ret = get_option_with(nvram, option_table, raw, name);

	struct cb_cmos_enums *cmos_enum;
	switch (cmos_entry->config) {
		case 'h':
			/* only works on little endian.
			   26 bytes is enough for a 64bit value in decimal */
			*dest = malloc(26);
			sprintf(*dest, "%" PRIu64, *(u64 *)raw);
			break;
		case 's':
			*dest = strdup(raw);
			break;
		case 'e':
			cmos_enum = lookup_cmos_enum_by_value(option_table, cmos_entry->config_id, (u8*)raw);
			*dest = strdup((const char*)cmos_enum->text);
			break;
		default: /* fail */
			ret = 1;
	}
	free(raw);
	return ret;
}

int set_option_from_string(const struct nvram_accessor *nvram, struct cb_cmos_option_table *option_table, const char *value, const char *name)
{
	void *raw;
	struct cb_cmos_entries *cmos_entry = lookup_cmos_entry(option_table, name);
	if (!cmos_entry)
		return 1;

	struct cb_cmos_enums *cmos_enum;
	switch (cmos_entry->config) {
		case 'h':
			/* only works on little endian */
			raw = malloc(sizeof(u64));
			*(u64*)raw = strtoull(value, NULL, 0);
			break;
		case 's':
			raw = malloc(cmos_entry->length);
			if (!raw)
				return 1;
			memset(raw, 0x00, cmos_entry->length);
			strncpy(raw, value, cmos_entry->length);
			break;
		case 'e':
			cmos_enum = lookup_cmos_enum_by_label(option_table, cmos_entry->config_id, value);
			raw = malloc(sizeof(u32));
			*(u32*)raw = cmos_enum->value;
			break;
		default: /* fail */
			return 1;
	}

	int ret = set_option_with(nvram, option_table, raw, name);
	free(raw);
	return ret;
}
