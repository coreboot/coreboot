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
#include <device/device.h>
#include <arch/cpu.h>
#include <cpu/x86/name.h>
#include <cbfs_core.h>
#include <arch/byteorder.h>
#include <elog.h>
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/gnvs.h>
#endif

static u8 smbios_checksum(u8 *p, u32 length)
{
	u8 ret = 0;
	while (length--)
		ret += *p++;
	return -ret;
}


int smbios_add_string(char *start, const char *str)
{
	int i = 1;
	char *p = start;

	for(;;) {
		if (!*p) {
			strcpy(p, str);
			p += strlen(str);
			*p++ = '\0';
			*p++ = '\0';
			return i;
		}

		if (!strcmp(p, str))
			return i;

		p += strlen(p)+1;
		i++;
	}
}

int smbios_string_table_len(char *start)
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

static int smbios_cpu_vendor(char *start)
{
	char tmp[13] = "Unknown";
	u32 *_tmp = (u32 *)tmp;
	struct cpuid_result res;

	if (cpu_have_cpuid()) {
		res = cpuid(0);
		_tmp[0] = res.ebx;
		_tmp[1] = res.edx;
		_tmp[2] = res.ecx;
		tmp[12] = '\0';
	}

	return smbios_add_string(start, tmp);
}

static int smbios_processor_name(char *start)
{
	char tmp[49] = "Unknown Processor Name";
	u32  *_tmp = (u32 *)tmp;
	struct cpuid_result res;
	int i;

	if (cpu_have_cpuid()) {
		res = cpuid(0x80000000);
		if (res.eax >= 0x80000004) {
			for (i = 0; i < 3; i++) {
				res = cpuid(0x80000002 + i);
				_tmp[i * 4 + 0] = res.eax;
				_tmp[i * 4 + 1] = res.ebx;
				_tmp[i * 4 + 2] = res.ecx;
				_tmp[i * 4 + 3] = res.edx;
			}
			tmp[48] = 0;
		}
	}
	return smbios_add_string(start, tmp);
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
#if !CONFIG_CHROMEOS
	t->bios_release_date = smbios_add_string(t->eos, COREBOOT_DMI_DATE);

	if (strlen(CONFIG_LOCALVERSION))
		t->bios_version = smbios_add_string(t->eos, CONFIG_LOCALVERSION);
	else
		t->bios_version = smbios_add_string(t->eos, COREBOOT_VERSION);
#else
#define SPACES \
	"                                                                  "
	t->bios_release_date = smbios_add_string(t->eos, COREBOOT_DMI_DATE);
	u32 version_offset = (u32)smbios_string_table_len(t->eos);
	t->bios_version = smbios_add_string(t->eos, SPACES);
	/* SMBIOS offsets start at 1 rather than 0 */
	vboot_data->vbt10 = (u32)t->eos + (version_offset - 1);
#endif

	{
		const struct cbfs_header *header;
		u32 romsize = CONFIG_ROM_SIZE;
		header = cbfs_get_header(CBFS_DEFAULT_MEDIA);
		if (header != CBFS_HEADER_INVALID_ADDRESS)
			romsize = ntohl(header->romsize);
		t->bios_rom_size = (romsize / 65535) - 1;
	}

	t->system_bios_major_release = 4;
	t->bios_characteristics =
		BIOS_CHARACTERISTICS_PCI_SUPPORTED |
#if CONFIG_CARDBUS_PLUGIN_SUPPORT
		BIOS_CHARACTERISTICS_PC_CARD |
#endif
		BIOS_CHARACTERISTICS_SELECTABLE_BOOT |
		BIOS_CHARACTERISTICS_UPGRADEABLE;

#if CONFIG_GENERATE_ACPI_TABLES
	t->bios_characteristics_ext1 = BIOS_EXT1_CHARACTERISTICS_ACPI;
#endif
	t->bios_characteristics_ext2 = BIOS_EXT2_CHARACTERISTICS_TARGET;
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

const char *__attribute__((weak)) smbios_mainboard_serial_number(void)
{
	return CONFIG_MAINBOARD_SERIAL_NUMBER;
}

const char *__attribute__((weak)) smbios_mainboard_version(void)
{
	return CONFIG_MAINBOARD_VERSION;
}

static int smbios_write_type1(unsigned long *current, int handle)
{
	struct smbios_type1 *t = (struct smbios_type1 *)*current;
	int len = sizeof(struct smbios_type1);

	memset(t, 0, sizeof(struct smbios_type1));
	t->type = SMBIOS_SYSTEM_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->manufacturer = smbios_add_string(t->eos, CONFIG_MAINBOARD_SMBIOS_VENDOR);
	t->product_name = smbios_add_string(t->eos, CONFIG_MAINBOARD_SMBIOS_PART_NUMBER);
	t->serial_number = smbios_add_string(t->eos, smbios_mainboard_serial_number());
	t->version = smbios_add_string(t->eos, smbios_mainboard_version());
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
	t->manufacturer = smbios_add_string(t->eos, CONFIG_MAINBOARD_SMBIOS_VENDOR);
	t->bootup_state = SMBIOS_STATE_SAFE;
	t->power_supply_state = SMBIOS_STATE_SAFE;
	t->thermal_state = SMBIOS_STATE_SAFE;
	t->_type = 3;
	t->security_status = SMBIOS_STATE_SAFE;
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int smbios_write_type4(unsigned long *current, int handle)
{
	struct cpuid_result res;
	struct smbios_type4 *t = (struct smbios_type4 *)*current;
	int len = sizeof(struct smbios_type4);

	/* Provide sane defaults even for CPU without CPUID */
	res.eax = res.edx = 0;
	res.ebx = 0x10000;

	if (cpu_have_cpuid()) {
		res = cpuid(1);
	}

	memset(t, 0, sizeof(struct smbios_type4));
	t->type = SMBIOS_PROCESSOR_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->processor_id[0] = res.eax;
	t->processor_id[1] = res.edx;
	t->processor_manufacturer = smbios_cpu_vendor(t->eos);
	t->processor_version = smbios_processor_name(t->eos);
	t->processor_family = (res.eax > 0) ? 0x0c : 0x6;
	t->processor_type = 3; /* System Processor */
	t->processor_upgrade = 0x06;
	t->core_count = (res.ebx >> 16) & 0xff;
	t->l1_cache_handle = 0xffff;
	t->l2_cache_handle = 0xffff;
	t->l3_cache_handle = 0xffff;
	t->processor_upgrade = 1;
	len = t->length + smbios_string_table_len(t->eos);
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

static int smbios_walk_device_tree(device_t tree, int *handle, unsigned long *current)
{
	device_t dev;
	int len = 0;

	for(dev = tree; dev; dev = dev->next) {
		printk(BIOS_INFO, "%s (%s)\n", dev_path(dev), dev_name(dev));

		if (dev->ops && dev->ops->get_smbios_data)
			len += dev->ops->get_smbios_data(dev, handle, current);
	}
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
#if CONFIG_ELOG
	len += elog_smbios_write_type15(&current, handle++);
#endif
	len += smbios_write_type32(&current, handle++);

	len += smbios_walk_device_tree(all_devices, &handle, &current);

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

	se->intermediate_checksum = smbios_checksum((u8 *)se + 0x10,
						    sizeof(struct smbios_entry) - 0x10);
	se->checksum = smbios_checksum((u8 *)se, sizeof(struct smbios_entry));
	return current;
}
