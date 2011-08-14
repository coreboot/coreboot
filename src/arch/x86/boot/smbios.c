/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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

#include <stdlib.h>
#include <string.h>
#include <smbios.h>
#include <console/console.h>
#include <build.h>

static u8 smbios_checksum(u8 *p, u32 length)
{
	u8 ret = 0;
	while (length--)
		ret += *p++;
	return -ret;
}


static int smbios_add_string(char *start, const char *str)
{
	int i = 1;
	char *p = start;

	for(;;) {
		if (!*p) {
			strcpy(p, str);
			p += strlen(str) + 1;
			*p = '\0';
			return i;
		}

		if (!strcmp(p, str))
			return i;
		p += strlen(p)+1;
		i++;
	}
}

static int smbios_string_table_len(char *start)
{
	char *p = start;
	int i, len = 0;

	while(*p) {
		i = strlen(p) + 1;
		p += i;
		len += i;
	}
	return len + 1;
}

static int smbios_write_type0(unsigned long *current, int handle)
{
	struct smbios_type0 *t = (struct smbios_type0 *)*current;
	int len = sizeof(struct smbios_type0);

	memset(t, 0, sizeof(struct smbios_type0));
	t->type = SMBIOS_BIOS_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->vendor = smbios_add_string(t->eos, "coreboot");
	t->bios_release_date = smbios_add_string(t->eos, COREBOOT_DMI_DATE);
	t->bios_version = smbios_add_string(t->eos, COREBOOT_VERSION);

	t->bios_rom_size = (CONFIG_ROM_SIZE / 65535)-1;
	t->system_bios_major_release = 4;
	t->bios_characteristics =
		BIOS_CHARACTERISTICS_PCI_SUPPORTED |	\
		BIOS_CHARACTERISTICS_PC_CARD | \
		BIOS_CHARACTERISTICS_SELECTABLE_BOOT | \
		BIOS_CHARACTERISTICS_UPGRADEABLE;

#if CONFIG_GENERATE_ACPI_TABLES
	t->bios_characteristics_ext1 = BIOS_EXT1_CHARACTERISTICS_ACPI;
#endif
	t->bios_characteristics_ext2 = BIOS_EXT2_CHARACTERISTICS_TARGET;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int smbios_write_type1(unsigned long *current, int handle)
{
	struct smbios_type1 *t = (struct smbios_type1 *)*current;
	int len = sizeof(struct smbios_type1);

	memset(t, 0, sizeof(struct smbios_type1));
	t->type = SMBIOS_SYSTEM_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->manufacturer = smbios_add_string(t->eos, CONFIG_MAINBOARD_VENDOR);
	t->product_name = smbios_add_string(t->eos, CONFIG_MAINBOARD_PART_NUMBER);
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int smbios_write_type3(unsigned long *current, int handle)
{
	struct smbios_type3 *t = (struct smbios_type3 *)*current;
	int len = sizeof(struct smbios_type3);

	memset(t, 0, sizeof(struct smbios_type3));
	t->type = SMBIOS_SYSTEM_ENCLOSURE;
	t->handle = handle;
	t->length = len - 2;
	t->manufacturer = smbios_add_string(t->eos, CONFIG_MAINBOARD_VENDOR);
	t->bootup_state = SMBIOS_STATE_SAFE;
	t->power_supply_state = SMBIOS_STATE_SAFE;
	t->thermal_state = SMBIOS_STATE_SAFE;
	t->_type = 2;
	t->security_status = SMBIOS_STATE_SAFE;
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int smbios_write_type4(unsigned long *current, int handle)
{
	struct smbios_type4 *t = (struct smbios_type4 *)*current;
	int len = sizeof(struct smbios_type4);

	memset(t, 0, sizeof(struct smbios_type4));
	t->type = SMBIOS_PROCESSOR_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->processor_manufacturer = smbios_add_string(t->eos, "Intel");
	t->processor_family = 1;
	t->processor_type = 3;
	t->core_count = 1;
	t->l1_cache_handle = 0xffff;
	t->l2_cache_handle = 0xffff;
	t->l3_cache_handle = 0xffff;
	t->processor_upgrade = 1;
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int smbios_write_type16(unsigned long *current, int handle)
{
	struct smbios_type16 *t = (struct smbios_type16 *)*current;
	int len = sizeof(struct smbios_type16);

	memset(t, 0, sizeof(struct smbios_type16));
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->handle = handle;
	t->length = len - 2;
	t->location = 2; /* Unknown */
	t->use = 3; /* System memory */
	t->memory_error_correction = 6;
	*current += len;
	return len;
}

static int smbios_write_type32(unsigned long *current, int handle)
{
	struct smbios_type32 *t = (struct smbios_type32 *)*current;
	int len = sizeof(struct smbios_type32);

	memset(t, 0, sizeof(struct smbios_type32));
	t->type = SMBIOS_SYSTEM_BOOT_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	*current += len;
	return len;
}

static int smbios_write_type127(unsigned long *current, int handle)
{
	struct smbios_type127 *t = (struct smbios_type127 *)*current;
	int len = sizeof(struct smbios_type127);

	memset(t, 0, sizeof(struct smbios_type127));
	t->type = SMBIOS_END_OF_TABLE;
	t->handle = handle;
	t->length = len - 2;
	*current += len;
	return len;
}

unsigned long smbios_write_tables(unsigned long current)
{
	struct smbios_entry *se;
	unsigned long tables;
	int len, handle = 0;

	current = ALIGN(current, 16);
	printk(BIOS_DEBUG, "%s: %08lx\n", __func__, current);

	se = (struct smbios_entry *)current;
	current += sizeof(struct smbios_entry);
	current = ALIGN(current, 16);

	tables = current;
	len = smbios_write_type0(&current, handle++);
	len += smbios_write_type1(&current, handle++);
	len += smbios_write_type3(&current, handle++);
	len += smbios_write_type4(&current, handle++);
	len += smbios_write_type16(&current, handle++);
	len += smbios_write_type32(&current, handle++);
	len += smbios_write_type127(&current, handle++);


	memset(se, 0, sizeof(struct smbios_entry));
	memcpy(se->anchor, "_SM_", 4);
	se->length = sizeof(struct smbios_entry);
	se->major_version = 2;
	se->minor_version = 7;
	se->max_struct_size = 24;
	se->struct_count = handle;
	memcpy(se->intermediate_anchor_string, "_DMI_", 5);

	se->struct_table_address = (u32)tables;
	se->struct_table_length = len;

	se->intermediate_checksum = smbios_checksum((u8 *)se + 0x10, sizeof(struct smbios_entry) - 0x10);
	se->checksum = smbios_checksum((u8 *)se, sizeof(struct smbios_entry));
	return current;
}
