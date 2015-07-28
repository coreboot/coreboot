/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 * Foundation, Inc.
 */

#include <stdlib.h>
#include <string.h>
#include <smbios.h>
#include <console/console.h>
#include <version.h>
#include <device/device.h>
#include <arch/cpu.h>
#include <cpu/x86/name.h>
#include <elog.h>
#include <endian.h>
#include <memory_info.h>
#include <spd.h>
#include <cbmem.h>
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

/* this function will fill the corresponding manufacturer */
void smbios_fill_dimm_manufacturer_from_id(uint16_t mod_id, struct smbios_type17 *t)
{
	switch (mod_id) {
		case 0x2c80:
			t->manufacturer = smbios_add_string(t->eos,
							    "Crucial");
			break;
		case 0x9801:
			t->manufacturer = smbios_add_string(t->eos,
							    "Kingston");
			break;
		case 0x987f:
			t->manufacturer = smbios_add_string(t->eos,
							    "Hynix");
			break;
		case 0x9e02:
			t->manufacturer = smbios_add_string(t->eos,
							    "Corsair");
			break;
		case 0xad80:
			t->manufacturer = smbios_add_string(t->eos,
							    "Hynix/Hyundai");
			break;
		case 0xce80:
			t->manufacturer = smbios_add_string(t->eos,
							    "Samsung");
			break;
		case 0xfe02:
			t->manufacturer = smbios_add_string(t->eos,
							    "Elpida");
			break;
		case 0xff2c:
			t->manufacturer = smbios_add_string(t->eos,
							    "Micron");
			break;
		default: {
			char string_buffer[256];
			snprintf(string_buffer, sizeof(string_buffer),
						"Unknown (%x)", mod_id);
			t->manufacturer = smbios_add_string(t->eos,
							    string_buffer);
			break;
		}
	}
}

static int create_smbios_type17_for_dimm(struct dimm_info *dimm,
					 unsigned long *current, int *handle)
{
	struct smbios_type17 *t = (struct smbios_type17 *)*current;
	uint8_t length;
	char locator[40];

	memset(t, 0, sizeof(struct smbios_type17));
	t->memory_type = dimm->ddr_type;
	t->clock_speed = dimm->ddr_frequency;
	t->speed = dimm->ddr_frequency;
	t->type = SMBIOS_MEMORY_DEVICE;
	t->size = dimm->dimm_size;
	t->data_width = 8 * (1 << (dimm->bus_width & 0x7));
	t->total_width = t->data_width + 8 * ((dimm->bus_width & 0x18) >> 3);

	switch (dimm->mod_type) {
		case SPD_RDIMM:
		case SPD_MINI_RDIMM:
			t->form_factor = MEMORY_FORMFACTOR_RIMM;
			break;
		case SPD_UDIMM:
		case SPD_MICRO_DIMM:
		case SPD_MINI_UDIMM:
			t->form_factor = MEMORY_FORMFACTOR_DIMM;
			break;
		case SPD_SODIMM:
			t->form_factor = MEMORY_FORMFACTOR_SODIMM;
			break;
		default:
			t->form_factor = MEMORY_FORMFACTOR_UNKNOWN;
			break;
	}

	smbios_fill_dimm_manufacturer_from_id(dimm->mod_id, t);
	/* put '\0' in the end of data */
	length = sizeof(dimm->serial);
	dimm->serial[length - 1] = '\0';
	if (dimm->serial[0] == 0)
		t->serial_number = smbios_add_string(t->eos, "None");
	else
		t->serial_number = smbios_add_string(t->eos,
						   (const char *)dimm->serial);

	snprintf(locator, sizeof(locator), "Channel-%d-DIMM-%d",
		dimm->channel_num, dimm->dimm_num);
	t->device_locator = smbios_add_string(t->eos, locator);

	snprintf(locator, sizeof(locator), "BANK %d", dimm->bank_locator);
	t->bank_locator = smbios_add_string(t->eos, locator);

	/* put '\0' in the end of data */
	length = sizeof(dimm->module_part_number);
	dimm->module_part_number[length - 1] = '\0';
	t->part_number = smbios_add_string(t->eos,
				      (const char *)dimm->module_part_number);

	/* Synchronous = 1 */
	t->type_detail = 0x0080;
	/* no handle for error information */
	t->memory_error_information_handle = 0xFFFE;
	t->attributes = dimm->rank_per_dimm;
	t->handle = *handle;
	*handle += 1;
	t->length = sizeof(struct smbios_type17) - 2;
	return t->length + smbios_string_table_len(t->eos);
}

const char *__attribute__((weak)) smbios_mainboard_bios_version(void)
{
	if (strlen(CONFIG_LOCALVERSION))
		return CONFIG_LOCALVERSION;
	else
		return coreboot_version;
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
	t->bios_release_date = smbios_add_string(t->eos, coreboot_dmi_date);

	t->bios_version = smbios_add_string(t->eos, smbios_mainboard_bios_version());
#else
#define SPACES \
	"                                                                  "
	t->bios_release_date = smbios_add_string(t->eos, coreboot_dmi_date);
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	u32 version_offset = (u32)smbios_string_table_len(t->eos);
#endif
	t->bios_version = smbios_add_string(t->eos, SPACES);

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	/* SMBIOS offsets start at 1 rather than 0 */
	vboot_data->vbt10 = (u32)t->eos + (version_offset - 1);
#endif
#endif /* CONFIG_CHROMEOS */

	t->bios_rom_size = (CONFIG_ROM_SIZE / 65535) - 1;

	t->system_bios_major_release = 4;
	t->bios_characteristics =
		BIOS_CHARACTERISTICS_PCI_SUPPORTED |
#if CONFIG_CARDBUS_PLUGIN_SUPPORT
		BIOS_CHARACTERISTICS_PC_CARD |
#endif
		BIOS_CHARACTERISTICS_SELECTABLE_BOOT |
		BIOS_CHARACTERISTICS_UPGRADEABLE;

#if CONFIG_HAVE_ACPI_TABLES
	t->bios_characteristics_ext1 = BIOS_EXT1_CHARACTERISTICS_ACPI;
#endif
	t->bios_characteristics_ext2 = BIOS_EXT2_CHARACTERISTICS_TARGET;
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

#if !CONFIG_SMBIOS_PROVIDED_BY_MOBO

const char *__attribute__((weak)) smbios_mainboard_serial_number(void)
{
	return CONFIG_MAINBOARD_SERIAL_NUMBER;
}

const char *__attribute__((weak)) smbios_mainboard_version(void)
{
	return CONFIG_MAINBOARD_VERSION;
}

const char *__attribute__((weak)) smbios_mainboard_manufacturer(void)
{
	return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
}

const char *__attribute__((weak)) smbios_mainboard_product_name(void)
{
	return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

void __attribute__((weak)) smbios_mainboard_set_uuid(u8 *uuid)
{
	/* leave all zero */
}
#endif

#ifdef CONFIG_MAINBOARD_FAMILY
const char *smbios_mainboard_family(void)
{
	return CONFIG_MAINBOARD_FAMILY;
}
#endif /* CONFIG_MAINBOARD_FAMILY */

static int smbios_write_type1(unsigned long *current, int handle)
{
	struct smbios_type1 *t = (struct smbios_type1 *)*current;
	int len = sizeof(struct smbios_type1);

	memset(t, 0, sizeof(struct smbios_type1));
	t->type = SMBIOS_SYSTEM_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->manufacturer = smbios_add_string(t->eos, smbios_mainboard_manufacturer());
	t->product_name = smbios_add_string(t->eos, smbios_mainboard_product_name());
	t->serial_number = smbios_add_string(t->eos, smbios_mainboard_serial_number());
	t->version = smbios_add_string(t->eos, smbios_mainboard_version());
#ifdef CONFIG_MAINBOARD_FAMILY
	t->family = smbios_add_string(t->eos, smbios_mainboard_family());
#endif
	smbios_mainboard_set_uuid(t->uuid);
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int smbios_write_type2(unsigned long *current, int handle)
{
	struct smbios_type2 *t = (struct smbios_type2 *)*current;
	int len = sizeof(struct smbios_type2);

	memset(t, 0, sizeof(struct smbios_type2));
	t->type = SMBIOS_BOARD_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->manufacturer = smbios_add_string(t->eos, smbios_mainboard_manufacturer());
	t->product_name = smbios_add_string(t->eos, smbios_mainboard_product_name());
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
	t->manufacturer = smbios_add_string(t->eos, smbios_mainboard_manufacturer());
	t->bootup_state = SMBIOS_STATE_SAFE;
	t->power_supply_state = SMBIOS_STATE_SAFE;
	t->thermal_state = SMBIOS_STATE_SAFE;
	if(IS_ENABLED(CONFIG_SYSTEM_TYPE_LAPTOP)) {
		t->_type = SMBIOS_ENCLOSURE_NOTEBOOK;
	} else {
		t->_type = SMBIOS_ENCLOSURE_DESKTOP;
	}
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

static int smbios_write_type11(unsigned long *current, int *handle)
{
	struct smbios_type11 *t = (struct smbios_type11 *)*current;
	int len;
	struct device *dev;

	memset(t, 0, sizeof *t);
	t->type = SMBIOS_OEM_STRINGS;
	t->handle = *handle;
	t->length = len = sizeof *t - 2;

	for(dev = all_devices; dev; dev = dev->next) {
		if (dev->ops && dev->ops->get_smbios_strings)
			dev->ops->get_smbios_strings(dev, t);
	}

	if (t->count == 0) {
		memset(t, 0, sizeof *t);
		return 0;
	}

	len += smbios_string_table_len(t->eos);

	*current += len;
	(*handle)++;
	return len;
}

static int smbios_write_type17(unsigned long *current, int *handle)
{
	int len = sizeof(struct smbios_type17);
	int i;

	struct memory_info *meminfo;
	meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo == NULL)
		return 0;	/* can't find mem info in cbmem */

	printk(BIOS_INFO, "Create SMBIOS type 17\n");
	for (i = 0; i < meminfo->dimm_cnt && i < ARRAY_SIZE(meminfo->dimm); i++) {
		struct dimm_info *dimm;
		dimm = &meminfo->dimm[i];
		len = create_smbios_type17_for_dimm(dimm, current, handle);
		*current += len;
	}
	return meminfo->dimm_cnt * len;
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

int smbios_write_type41(unsigned long *current, int *handle,
			const char *name, u8 instance, u16 segment,
			u8 bus, u8 device, u8 function)
{
	struct smbios_type41 *t = (struct smbios_type41 *)*current;
	int len = sizeof(struct smbios_type41);

	memset(t, 0, sizeof(struct smbios_type41));
	t->type = SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION;
	t->handle = *handle;
	t->length = len - 2;
	t->reference_designation = smbios_add_string(t->eos, name);
	t->device_type = SMBIOS_DEVICE_TYPE_OTHER;
	t->device_status = 1;
	t->device_type_instance = instance;
	t->segment_group_number = segment;
	t->bus_number = bus;
	t->device_number = device;
	t->function_number = function;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	*handle += 1;
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

static int smbios_walk_device_tree(struct device *tree, int *handle, unsigned long *current)
{
	struct device *dev;
	int len = 0;

	for(dev = tree; dev; dev = dev->next) {
		printk(BIOS_INFO, "%s (%s)\n", dev_path(dev), dev_name(dev));

		if (dev->ops && dev->ops->get_smbios_data)
			len += dev->ops->get_smbios_data(dev, handle, current);
	}
	return len;
}

#define update_max(len, max_len, stmt) do { int tmp = stmt; max_len = MAX(max_len, tmp); len += tmp; } while(0)
unsigned long smbios_write_tables(unsigned long current)
{
	struct smbios_entry *se;
	unsigned long tables;
	int len = 0;
	int max_struct_size = 0;
	int handle = 0;

	current = ALIGN(current, 16);
	printk(BIOS_DEBUG, "%s: %08lx\n", __func__, current);

	se = (struct smbios_entry *)current;
	current += sizeof(struct smbios_entry);
	current = ALIGN(current, 16);

	tables = current;
	update_max(len, max_struct_size, smbios_write_type0(&current, handle++));
	update_max(len, max_struct_size, smbios_write_type1(&current, handle++));
	update_max(len, max_struct_size, smbios_write_type2(&current, handle++));
	update_max(len, max_struct_size, smbios_write_type3(&current, handle++));
	update_max(len, max_struct_size, smbios_write_type4(&current, handle++));
	update_max(len, max_struct_size, smbios_write_type11(&current, &handle));
#if CONFIG_ELOG
	update_max(len, max_struct_size, elog_smbios_write_type15(&current, handle++));
#endif
	update_max(len, max_struct_size, smbios_write_type17(&current, &handle));
	update_max(len, max_struct_size, smbios_write_type32(&current, handle++));

	update_max(len, max_struct_size, smbios_walk_device_tree(all_devices, &handle, &current));

	update_max(len, max_struct_size, smbios_write_type127(&current, handle++));

	memset(se, 0, sizeof(struct smbios_entry));
	memcpy(se->anchor, "_SM_", 4);
	se->length = sizeof(struct smbios_entry);
	se->major_version = 2;
	se->minor_version = 7;
	se->max_struct_size = max_struct_size;
	se->struct_count = handle;
	memcpy(se->intermediate_anchor_string, "_DMI_", 5);

	se->struct_table_address = (u32)tables;
	se->struct_table_length = len;

	se->intermediate_checksum = smbios_checksum((u8 *)se + 0x10,
						    sizeof(struct smbios_entry) - 0x10);
	se->checksum = smbios_checksum((u8 *)se, sizeof(struct smbios_entry));
	return current;
}
