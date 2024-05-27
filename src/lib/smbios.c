/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <string.h>
#include <smbios.h>
#include <console/console.h>
#include <version.h>
#include <device/device.h>
#include <device/dram/spd.h>
#include <elog.h>
#include <endian.h>
#include <memory_info.h>
#include <spd.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <device/pci_ids.h>
#include <device/pci.h>
#include <drivers/vpd/vpd.h>
#include <stdio.h>
#include <stdlib.h>

static u8 smbios_checksum(u8 *p, u32 length)
{
	u8 ret = 0;
	while (length--)
		ret += *p++;
	return -ret;
}

int smbios_add_string(u8 *start, const char *str)
{
	int i = 1;
	char *p = (char *)start;

	/*
	 * Return 0 as required for empty strings.
	 * See Section 6.1.3 "Text Strings" of the SMBIOS specification.
	 */
	if (str == NULL || *str == '\0')
		return 0;

	for (;;) {
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

int smbios_string_table_len(u8 *start)
{
	char *p = (char *)start;
	int i, len = 0;

	while (*p) {
		i = strlen(p) + 1;
		p += i;
		len += i;
	}

	if (!len)
		return 2;

	return len + 1;
}

int smbios_full_table_len(struct smbios_header *header, u8 *str_table_start)
{
	return header->length + smbios_string_table_len(str_table_start);
}

void *smbios_carve_table(unsigned long start, u8 type, u8 length, u16 handle)
{
	struct smbios_header *t = (struct smbios_header *)start;

	assert(length >= sizeof(*t));
	memset(t, 0, length);
	t->type = type;
	t->length = length - 2;
	t->handle = handle;
	return t;
}

/* this function will fill the corresponding manufacturer */
void smbios_fill_dimm_manufacturer_from_id(uint16_t mod_id, struct smbios_type17 *t)
{
	const char *const manufacturer = spd_manufacturer_name(mod_id);

	if (manufacturer) {
		t->manufacturer = smbios_add_string(t->eos, manufacturer);
	} else {
		char string_buffer[256];

		snprintf(string_buffer, sizeof(string_buffer), "Unknown (%x)", mod_id);
		t->manufacturer = smbios_add_string(t->eos, string_buffer);
	}
}

static void trim_trailing_whitespace(char *buffer, size_t buffer_size)
{
	size_t len = strnlen(buffer, buffer_size);

	if (len == 0)
		return;

	for (char *p = buffer + len - 1; p >= buffer; --p) {
		if (*p == ' ')
			*p = 0;
		else
			break;
	}
}

/** This function will fill the corresponding part number */
static void smbios_fill_dimm_part_number(const char *part_number, struct smbios_type17 *t)
{
	int invalid;
	size_t i, len;
	char trimmed_part_number[DIMM_INFO_PART_NUMBER_SIZE];

	strncpy(trimmed_part_number, part_number, sizeof(trimmed_part_number));
	trimmed_part_number[sizeof(trimmed_part_number) - 1] = '\0';

	/*
	 * SPD mandates that unused characters be represented with a ' '.
	 * We don't want to publish the whitespace in the SMBIOS tables.
	 */
	trim_trailing_whitespace(trimmed_part_number, sizeof(trimmed_part_number));

	len = strlen(trimmed_part_number);

	invalid = 0; /* assume valid */
	for (i = 0; i < len; i++) {
		if (trimmed_part_number[i] < ' ') {
			invalid = 1;
			trimmed_part_number[i] = '*';
		}
	}

	if (len == 0) {
		/* Null String in Part Number will have "None" instead. */
		t->part_number = smbios_add_string(t->eos, "None");
	} else if (invalid) {
		char string_buffer[sizeof(trimmed_part_number) + 10];

		snprintf(string_buffer, sizeof(string_buffer), "Invalid (%s)",
			 trimmed_part_number);
		t->part_number = smbios_add_string(t->eos, string_buffer);
	} else {
		t->part_number = smbios_add_string(t->eos, trimmed_part_number);
	}
}

/* Encodes the SPD serial number into hex */
static void smbios_fill_dimm_serial_number(const struct dimm_info *dimm,
					   struct smbios_type17 *t)
{
	char serial[9];

	snprintf(serial, sizeof(serial), "%02hhx%02hhx%02hhx%02hhx",
		 dimm->serial[0], dimm->serial[1], dimm->serial[2], dimm->serial[3]);

	t->serial_number = smbios_add_string(t->eos, serial);
}

static const char *memory_device_type(u8 code)
{
	/* SMBIOS spec 3.6 7.18.2 */
	static const char * const type[] = {
		"Other",
		"Unknown",
		"DRAM",
		"EDRAM",
		"VRAM",
		"SRAM",
		"RAM",
		"ROM",
		"Flash",
		"EEPROM",
		"FEPROM",
		"EPROM",
		"CDRAM",
		"3DRAM",
		"SDRAM",
		"SGRAM",
		"RDRAM",
		"DDR",
		"DDR2",
		"DDR2 FB-DIMM",
		"Reserved",
		"Reserved",
		"Reserved",
		"DDR3",
		"FBD2",
		"DDR4", /* 0x1A */
		"LPDDR",
		"LPDDR2",
		"LPDDR3",
		"LPDDR4",
		"Logical non-volatile device",
		"HBM",
		"HBM2",
		"DDR5",
		"LPDDR5",
		"HBM3", /* 0x24 */
	};

	if (code >= MEMORY_TYPE_OTHER && code <= MEMORY_TYPE_HBM3)
		return type[code - 1];
	return "Unsupported";
}

static void dump_smbios_type17(struct dimm_info *dimm)
{
	printk(BIOS_INFO, "memory at Channel-%d-DIMM-%d", dimm->channel_num, dimm->dimm_num);
	printk(BIOS_INFO, " type is %s\n", memory_device_type(dimm->ddr_type));
	printk(BIOS_INFO, "memory part number is %s\n", dimm->module_part_number);
	if (dimm->max_speed_mts != 0)
		printk(BIOS_INFO, "memory max speed is %d MT/s\n", dimm->max_speed_mts);
	printk(BIOS_INFO, "memory speed is %d MT/s\n",
					dimm->configured_speed_mts ? : dimm->ddr_frequency);
	printk(BIOS_INFO, "memory size is %d MiB\n", dimm->dimm_size);
}

static int create_smbios_type17_for_dimm(struct dimm_info *dimm,
					 unsigned long *current, int *handle,
					 int type16_handle)
{
	struct spd_info info;
	get_spd_info(dimm->ddr_type, dimm->mod_type, &info);

	struct smbios_type17 *t = smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE,
						     sizeof(*t), *handle);

	t->memory_type = dimm->ddr_type;
	if (dimm->configured_speed_mts != 0)
		t->clock_speed = dimm->configured_speed_mts;
	else
		t->clock_speed = dimm->ddr_frequency;
	if (dimm->max_speed_mts != 0)
		t->speed = dimm->max_speed_mts;
	else
		t->speed = dimm->ddr_frequency;
	if (dimm->dimm_size < 0x7fff) {
		t->size = dimm->dimm_size;
	} else {
		t->size = 0x7fff;
		t->extended_size = dimm->dimm_size & 0x7fffffff;
	}
	t->data_width = 8 * (1 << (dimm->bus_width & 0x7));
	t->total_width = t->data_width + 8 * ((dimm->bus_width & 0x18) >> 3);
	t->form_factor = info.form_factor;

	smbios_fill_dimm_manufacturer_from_id(dimm->mod_id, t);
	smbios_fill_dimm_serial_number(dimm, t);
	smbios_fill_dimm_asset_tag(dimm, t);
	smbios_fill_dimm_locator(dimm, t);

	/* put '\0' in the end of data */
	dimm->module_part_number[DIMM_INFO_PART_NUMBER_SIZE - 1] = '\0';
	smbios_fill_dimm_part_number((char *)dimm->module_part_number, t);

	/* Voltage Levels */
	t->configured_voltage = dimm->vdd_voltage;
	t->minimum_voltage = dimm->vdd_voltage;
	t->maximum_voltage = dimm->vdd_voltage;

	/* Fill in type detail */
	t->type_detail = info.type_detail;

	/* Synchronous = 1 */
	t->type_detail |= MEMORY_TYPE_DETAIL_SYNCHRONOUS;
	/* no handle for error information */
	t->memory_error_information_handle = 0xFFFE;
	t->attributes = dimm->rank_per_dimm;
	t->phys_memory_array_handle = type16_handle;

	*handle += 1;
	return smbios_full_table_len(&t->header, t->eos);
}

static int create_smbios_type17_for_empty_slot(struct dimm_info *dimm,
					       unsigned long *current, int *handle,
					       int type16_handle)
{
	struct smbios_type17 *t = smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE,
						     sizeof(*t), *handle);
	t->phys_memory_array_handle = type16_handle;
	/* no handle for error information */
	t->memory_error_information_handle = 0xfffe;
	t->total_width = 0xffff; /* Unknown */
	t->data_width = 0xffff; /* Unknown */
	t->form_factor = 0x2; /* Unknown */
	smbios_fill_dimm_locator(dimm, t); /* Device and Bank */
	t->memory_type = 0x2; /* Unknown */
	t->type_detail = 0x2; /* Unknown */

	*handle += 1;
	if (CONFIG(DUMP_SMBIOS_TYPE17))
		dump_smbios_type17(dimm);

	return smbios_full_table_len(&t->header, t->eos);
}

#define VERSION_VPD "firmware_version"
static const char *vpd_get_bios_version(void)
{
	int size;
	const char *s;
	char *version;

	s = vpd_find(VERSION_VPD, &size, VPD_RO);
	if (!s) {
		printk(BIOS_ERR, "Find version from VPD %s failed\n", VERSION_VPD);
		return NULL;
	}

	version = malloc(size + 1);
	if (!version) {
		printk(BIOS_ERR, "Failed to malloc %d bytes for VPD version\n", size + 1);
		return NULL;
	}
	memcpy(version, s, size);
	version[size] = '\0';
	printk(BIOS_DEBUG, "Firmware version %s from VPD %s\n", version, VERSION_VPD);
	return version;
}

static const char *get_bios_version(void)
{
	const char *s;

#define SPACES \
	"                                                                  "

	if (CONFIG(CHROMEOS))
		return SPACES;

	if (CONFIG(VPD_SMBIOS_VERSION)) {
		s = vpd_get_bios_version();
		if (s != NULL)
			return s;
	}

	s = smbios_mainboard_bios_version();
	if (s != NULL)
		return s;

	if (strlen(CONFIG_LOCALVERSION) != 0) {
		printk(BIOS_DEBUG, "BIOS version set to CONFIG_LOCALVERSION: '%s'\n",
			CONFIG_LOCALVERSION);
		return CONFIG_LOCALVERSION;
	}

	printk(BIOS_DEBUG, "SMBIOS firmware version is set to coreboot_version: '%s'\n",
		coreboot_version);
	return coreboot_version;
}

static int smbios_write_type0(unsigned long *current, int handle)
{
	struct smbios_type0 *t = smbios_carve_table(*current, SMBIOS_BIOS_INFORMATION,
						    sizeof(*t), handle);

	t->vendor = smbios_add_string(t->eos, CONFIG_BIOS_VENDOR);
	t->bios_release_date = smbios_add_string(t->eos, coreboot_dmi_date);

	if (CONFIG(CHROMEOS_NVS)) {
		uintptr_t version_address = (uintptr_t)t->eos;
		/* SMBIOS offsets start at 1 rather than 0 */
		version_address += (u32)smbios_string_table_len(t->eos) - 1;
		smbios_type0_bios_version(version_address);
	}
	t->bios_version = smbios_add_string(t->eos, get_bios_version());
	uint32_t rom_size = CONFIG_ROM_SIZE;
	rom_size = MIN(CONFIG_ROM_SIZE, 16 * MiB);
	t->bios_rom_size = (rom_size / 65535) - 1;

	if (CONFIG_ROM_SIZE >= 1 * GiB)
		t->extended_bios_rom_size = DIV_ROUND_UP(CONFIG_ROM_SIZE, GiB) | (1 << 14);
	else
		t->extended_bios_rom_size = DIV_ROUND_UP(CONFIG_ROM_SIZE, MiB);

	t->system_bios_major_release = coreboot_major_revision;
	t->system_bios_minor_release = coreboot_minor_revision;

	smbios_ec_revision(&t->ec_major_release, &t->ec_minor_release);

	t->bios_characteristics =
		BIOS_CHARACTERISTICS_PCI_SUPPORTED |
		BIOS_CHARACTERISTICS_SELECTABLE_BOOT |
		BIOS_CHARACTERISTICS_UPGRADEABLE;

	if (CONFIG(CARDBUS_PLUGIN_SUPPORT))
		t->bios_characteristics |= BIOS_CHARACTERISTICS_PC_CARD;

	if (CONFIG(HAVE_ACPI_TABLES))
		t->bios_characteristics_ext1 = BIOS_EXT1_CHARACTERISTICS_ACPI;

	t->bios_characteristics_ext2 = BIOS_EXT2_CHARACTERISTICS_TARGET;
	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

unsigned int __weak smbios_processor_external_clock(void)
{
	return 0; /* Unknown */
}

unsigned int __weak smbios_processor_characteristics(void)
{
	return 0;
}

unsigned int __weak smbios_cache_error_correction_type(u8 level)
{
	return SMBIOS_CACHE_ERROR_CORRECTION_UNKNOWN;
}

unsigned int __weak smbios_cache_sram_type(void)
{
	return SMBIOS_CACHE_SRAM_TYPE_UNKNOWN;
}

unsigned int __weak smbios_cache_conf_operation_mode(u8 level)
{
	return SMBIOS_CACHE_OP_MODE_UNKNOWN; /* Unknown */
}

/* Returns the processor voltage in 100mV units */
unsigned int __weak smbios_cpu_get_voltage(void)
{
	return 0; /* Unknown */
}

static int smbios_write_type1(unsigned long *current, int handle)
{
	struct smbios_type1 *t = smbios_carve_table(*current, SMBIOS_SYSTEM_INFORMATION,
						    sizeof(*t), handle);

	t->manufacturer = smbios_add_string(t->eos, smbios_system_manufacturer());
	t->product_name = smbios_add_string(t->eos, smbios_system_product_name());
	t->serial_number = smbios_add_string(t->eos, smbios_system_serial_number());
	t->wakeup_type = smbios_system_wakeup_type();
	t->sku = smbios_add_string(t->eos, smbios_system_sku());
	t->version = smbios_add_string(t->eos, smbios_system_version());
#ifdef CONFIG_MAINBOARD_FAMILY
	t->family = smbios_add_string(t->eos, CONFIG_MAINBOARD_FAMILY);
#endif
	smbios_system_set_uuid(t->uuid);
	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int smbios_write_type2(unsigned long *current, int handle, const int chassis_handle)
{
	struct smbios_type2 *t = smbios_carve_table(*current, SMBIOS_BOARD_INFORMATION,
						    sizeof(*t), handle);

	t->manufacturer = smbios_add_string(t->eos, smbios_mainboard_manufacturer());
	t->product_name = smbios_add_string(t->eos, smbios_mainboard_product_name());
	t->serial_number = smbios_add_string(t->eos, smbios_mainboard_serial_number());
	t->version = smbios_add_string(t->eos, smbios_mainboard_version());
	t->asset_tag = smbios_add_string(t->eos, smbios_mainboard_asset_tag());
	t->feature_flags = smbios_mainboard_feature_flags();
	t->location_in_chassis = smbios_add_string(t->eos,
		smbios_mainboard_location_in_chassis());
	t->board_type = smbios_mainboard_board_type();
	t->chassis_handle = chassis_handle;
	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int smbios_write_type3(unsigned long *current, int handle)
{
	struct smbios_type3 *t = smbios_carve_table(*current, SMBIOS_SYSTEM_ENCLOSURE,
						    sizeof(*t), handle);

	t->manufacturer = smbios_add_string(t->eos, smbios_system_manufacturer());
	t->bootup_state = SMBIOS_STATE_SAFE;
	t->power_supply_state = SMBIOS_STATE_SAFE;
	t->thermal_state = SMBIOS_STATE_SAFE;
	t->_type = smbios_mainboard_enclosure_type();
	t->security_status = SMBIOS_STATE_SAFE;
	t->number_of_power_cords = smbios_chassis_power_cords();
	t->asset_tag_number = smbios_add_string(t->eos, smbios_mainboard_asset_tag());
	t->version = smbios_add_string(t->eos, smbios_chassis_version());
	t->serial_number = smbios_add_string(t->eos, smbios_chassis_serial_number());
	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

/*
 * Write SMBIOS type 7.
 * Fill in some fields with constant values, as gathering the information
 * from CPUID is impossible.
 */
int smbios_write_type7(unsigned long *current,
		       const int handle,
		       const u8 level,
		       const u8 sram_type,
		       const enum smbios_cache_associativity associativity,
		       const enum smbios_cache_type type,
		       const size_t max_cache_size,
		       const size_t cache_size)
{
	char buf[8];

	struct smbios_type7 *t = smbios_carve_table(*current, SMBIOS_CACHE_INFORMATION,
						    sizeof(*t), handle);

	snprintf(buf, sizeof(buf), "CACHE%x", level);
	t->socket_designation = smbios_add_string(t->eos, buf);

	t->cache_configuration = SMBIOS_CACHE_CONF_LEVEL(level) |
		SMBIOS_CACHE_CONF_LOCATION(0) | /* Internal */
		SMBIOS_CACHE_CONF_ENABLED(1) | /* Enabled */
		SMBIOS_CACHE_CONF_OPERATION_MODE(smbios_cache_conf_operation_mode(level));

	if (max_cache_size < (SMBIOS_CACHE_SIZE_MASK * KiB)) {
		t->max_cache_size = max_cache_size / KiB;
		t->max_cache_size2 = t->max_cache_size;

		t->max_cache_size |= SMBIOS_CACHE_SIZE_UNIT_1KB;
		t->max_cache_size2 |= SMBIOS_CACHE_SIZE2_UNIT_1KB;
	} else {
		if (max_cache_size < (SMBIOS_CACHE_SIZE_MASK * 64 * KiB))
			t->max_cache_size = max_cache_size / (64 * KiB);
		else
			t->max_cache_size = SMBIOS_CACHE_SIZE_OVERFLOW;
		t->max_cache_size2 = max_cache_size / (64 * KiB);

		t->max_cache_size |= SMBIOS_CACHE_SIZE_UNIT_64KB;
		t->max_cache_size2 |= SMBIOS_CACHE_SIZE2_UNIT_64KB;
	}

	if (cache_size < (SMBIOS_CACHE_SIZE_MASK * KiB)) {
		t->installed_size = cache_size / KiB;
		t->installed_size2 = t->installed_size;

		t->installed_size |= SMBIOS_CACHE_SIZE_UNIT_1KB;
		t->installed_size2 |= SMBIOS_CACHE_SIZE2_UNIT_1KB;
	} else {
		if (cache_size < (SMBIOS_CACHE_SIZE_MASK * 64 * KiB))
			t->installed_size = cache_size / (64 * KiB);
		else
			t->installed_size = SMBIOS_CACHE_SIZE_OVERFLOW;
		t->installed_size2 = cache_size / (64 * KiB);

		t->installed_size |= SMBIOS_CACHE_SIZE_UNIT_64KB;
		t->installed_size2 |= SMBIOS_CACHE_SIZE2_UNIT_64KB;
	}

	t->associativity = associativity;
	t->supported_sram_type = sram_type;
	t->current_sram_type = sram_type;
	t->cache_speed = 0; /* Unknown */
	t->error_correction_type = smbios_cache_error_correction_type(level);
	t->system_cache_type = type;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

/* Convert the associativity as integer to the SMBIOS enum if available */
enum smbios_cache_associativity smbios_cache_associativity(const u8 num)
{
	switch (num) {
	case 1:
		return SMBIOS_CACHE_ASSOCIATIVITY_DIRECT;
	case 2:
		return SMBIOS_CACHE_ASSOCIATIVITY_2WAY;
	case 4:
		return SMBIOS_CACHE_ASSOCIATIVITY_4WAY;
	case 8:
		return SMBIOS_CACHE_ASSOCIATIVITY_8WAY;
	case 12:
		return SMBIOS_CACHE_ASSOCIATIVITY_12WAY;
	case 16:
		return SMBIOS_CACHE_ASSOCIATIVITY_16WAY;
	case 20:
		return SMBIOS_CACHE_ASSOCIATIVITY_20WAY;
	case 24:
		return SMBIOS_CACHE_ASSOCIATIVITY_24WAY;
	case 32:
		return SMBIOS_CACHE_ASSOCIATIVITY_32WAY;
	case 48:
		return SMBIOS_CACHE_ASSOCIATIVITY_48WAY;
	case 64:
		return SMBIOS_CACHE_ASSOCIATIVITY_64WAY;
	case 0xff:
		return SMBIOS_CACHE_ASSOCIATIVITY_FULL;
	default:
		return SMBIOS_CACHE_ASSOCIATIVITY_UNKNOWN;
	};
}

int smbios_write_type8(unsigned long *current, int *handle,
				const struct port_information *port,
				size_t num_ports)
{
	unsigned int totallen = 0, i;

	for (i = 0; i < num_ports; i++, port++) {
		struct smbios_type8 *t = smbios_carve_table(*current,
							    SMBIOS_PORT_CONNECTOR_INFORMATION,
							    sizeof(*t), *handle);
		t->internal_reference_designator =
			smbios_add_string(t->eos, port->internal_reference_designator);
		t->internal_connector_type = port->internal_connector_type;
		t->external_reference_designator =
			smbios_add_string(t->eos, port->external_reference_designator);
		t->external_connector_type = port->external_connector_type;
		t->port_type = port->port_type;
		*handle += 1;
		const int len = smbios_full_table_len(&t->header, t->eos);
		*current += len;
		totallen += len;
	}
	return totallen;
}

int smbios_write_type9(unsigned long *current, int *handle,
			const char *name, const enum misc_slot_type type,
			const enum slot_data_bus_bandwidth bandwidth,
			const enum misc_slot_usage usage,
			const enum misc_slot_length length,
			const u16 id, u8 slot_char1, u8 slot_char2,
			u8 segment_group, u8 bus, u8 dev_func)
{
	struct smbios_type9 *t = smbios_carve_table(*current, SMBIOS_SYSTEM_SLOTS,
						    sizeof(*t), *handle);

	t->slot_designation = smbios_add_string(t->eos, name ? name : "SLOT");
	t->slot_type = type;
	/* TODO add slot_id support, will be "_SUN" for ACPI devices */
	t->slot_id = id;
	t->slot_data_bus_width = bandwidth;
	t->current_usage = usage;
	t->slot_length = length;
	t->slot_characteristics_1 = slot_char1;
	t->slot_characteristics_2 = slot_char2;
	t->segment_group_number = segment_group;
	t->bus_number = bus;
	t->device_function_number = dev_func;
	t->data_bus_width = SlotDataBusWidthOther;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static int smbios_write_type11(unsigned long *current, int *handle)
{
	struct device *dev;
	struct smbios_type11 *t = smbios_carve_table(*current, SMBIOS_OEM_STRINGS,
						     sizeof(*t), *handle);

	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->ops && dev->ops->get_smbios_strings)
			dev->ops->get_smbios_strings(dev, t);
	}

	if (t->count == 0) {
		memset(t, 0, sizeof(*t));
		return 0;
	}

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	(*handle)++;
	return len;
}

static int smbios_write_type16(unsigned long *current, int *handle)
{
	int i;
	uint64_t max_capacity;

	struct memory_info *meminfo;
	meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo == NULL)
		return 0;	/* can't find mem info in cbmem */

	printk(BIOS_INFO, "Create SMBIOS type 16\n");

	if (meminfo->max_capacity_mib == 0 || meminfo->number_of_devices == 0) {
		/* Fill in defaults if not provided */
		meminfo->number_of_devices = 0;
		meminfo->max_capacity_mib = 0;
		for (i = 0; i < meminfo->dimm_cnt && i < ARRAY_SIZE(meminfo->dimm); i++) {
			meminfo->max_capacity_mib += meminfo->dimm[i].dimm_size;
			meminfo->number_of_devices += !!meminfo->dimm[i].dimm_size;
		}
	}

	struct smbios_type16 *t = smbios_carve_table(*current, SMBIOS_PHYS_MEMORY_ARRAY,
						     sizeof(*t), *handle);

	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	t->memory_error_correction = meminfo->ecc_type;

	/* no error information handle available */
	t->memory_error_information_handle = 0xFFFE;
	max_capacity = meminfo->max_capacity_mib;
	if (max_capacity * (MiB / KiB) < SMBIOS_USE_EXTENDED_MAX_CAPACITY)
		t->maximum_capacity = max_capacity * (MiB / KiB);
	else {
		t->maximum_capacity = SMBIOS_USE_EXTENDED_MAX_CAPACITY;
		t->extended_maximum_capacity = max_capacity * MiB;
	}
	t->number_of_memory_devices = meminfo->number_of_devices;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	(*handle)++;
	return len;
}

static int smbios_write_type17(unsigned long *current, int *handle, int type16)
{
	int totallen = 0;
	int i;

	struct memory_info *meminfo;
	meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo == NULL)
		return 0;	/* can't find mem info in cbmem */

	printk(BIOS_INFO, "Create SMBIOS type 17\n");
	for (i = 0; i < meminfo->dimm_cnt && i < ARRAY_SIZE(meminfo->dimm); i++) {
		struct dimm_info *d = &meminfo->dimm[i];
		/*
		 * Windows 10 GetPhysicallyInstalledSystemMemory functions reads SMBIOS tables
		 * type 16 and type 17. The type 17 tables need to point to a type 16 table.
		 * Otherwise, the physical installed memory size is guessed from the system
		 * memory map, which results in a slightly smaller value than the actual size.
		 */
		int len;
		if (d->dimm_size > 0)
			len = create_smbios_type17_for_dimm(d, current, handle, type16);
		else
			len = create_smbios_type17_for_empty_slot(d, current, handle, type16);

		*current += len;
		totallen += len;
	}
	return totallen;
}

static int smbios_write_type19(unsigned long *current, int *handle, int type16)
{
	int i;

	struct memory_info *meminfo;
	meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo == NULL)
		return 0;	/* can't find mem info in cbmem */

	struct smbios_type19 *t = smbios_carve_table(*current,
						     SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS,
						     sizeof(*t), *handle);

	t->memory_array_handle = type16;

	for (i = 0; i < meminfo->dimm_cnt && i < ARRAY_SIZE(meminfo->dimm); i++) {
		if (meminfo->dimm[i].dimm_size > 0) {
			t->extended_ending_address += meminfo->dimm[i].dimm_size;
			t->partition_width++;
		}
	}
	t->extended_ending_address *= MiB;

	/* Check if it fits into regular address */
	if (t->extended_ending_address >= KiB &&
	    t->extended_ending_address < 0x40000000000ULL) {
		/*
		 * FIXME: The starting address is SoC specific, but SMBIOS tables are only
		 * exported on x86 where it's always 0.
		 */

		t->starting_address = 0;
		t->ending_address = t->extended_ending_address / KiB - 1;
		t->extended_starting_address = ~0;
		t->extended_ending_address = ~0;
	} else {
		t->starting_address = ~0;
		t->ending_address = ~0;
		t->extended_starting_address = 0;
		t->extended_ending_address--;
	}

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static int smbios_write_type20_table(unsigned long *current, int *handle, u32 addr_start,
		u32 addr_end, int type17_handle, int type19_handle)
{
	struct smbios_type20 *t = smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE_MAPPED_ADDRESS,
						     sizeof(*t), *handle);

	t->memory_device_handle = type17_handle;
	t->memory_array_mapped_address_handle = type19_handle;
	t->addr_start = addr_start;
	t->addr_end = addr_end;
	t->partition_row_pos = 0xff;
	t->interleave_pos = 0xff;
	t->interleave_depth = 0xff;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static int smbios_write_type20(unsigned long *current, int *handle,
		int type17_handle, int type19_handle)
{
	u32 start_addr = 0;
	int totallen = 0;
	int i;

	struct memory_info *meminfo;
	meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo == NULL)
		return 0;	/* can't find mem info in cbmem */

	printk(BIOS_INFO, "Create SMBIOS type 20\n");
	for (i = 0; i < meminfo->dimm_cnt && i < ARRAY_SIZE(meminfo->dimm); i++) {
		struct dimm_info *dimm;
		dimm = &meminfo->dimm[i];
		if (dimm->dimm_size == 0)
			continue;

		u32 end_addr = start_addr + (dimm->dimm_size << 10) - 1;
		totallen += smbios_write_type20_table(current, handle, start_addr, end_addr,
				type17_handle, type19_handle);
		start_addr = end_addr + 1;
	}
	return totallen;
}

int smbios_write_type28(unsigned long *current, int *handle,
			const char *name,
			const enum smbios_temp_location location,
			const enum smbios_temp_status status,
			u16 max_value, u16 min_value,
			u16 resolution, u16 tolerance,
			u16 accuracy,
			u32 oem,
			u16 nominal_value)
{
	struct smbios_type28 *t = smbios_carve_table(*current, SMBIOS_TEMPERATURE_PROBE,
						     sizeof(*t), *handle);

	t->description = smbios_add_string(t->eos, name ? name : "Temperature");
	t->location_and_status = location | (status << 5);
	t->maximum_value = max_value;
	t->minimum_value = min_value;
	t->resolution = resolution;
	t->tolerance = tolerance;
	t->accuracy = accuracy;
	t->oem_defined = oem;
	t->nominal_value = nominal_value;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static int smbios_write_type32(unsigned long *current, int handle)
{
	struct smbios_type32 *t = smbios_carve_table(*current, SMBIOS_SYSTEM_BOOT_INFORMATION,
						     sizeof(*t), handle);

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

int smbios_write_type38(unsigned long *current, int *handle,
			const enum smbios_bmc_interface_type interface_type,
			const u8 ipmi_rev, const u8 i2c_addr, const u8 nv_addr,
			const u64 base_addr, const u8 base_modifier,
			const u8 irq)
{
	struct smbios_type38 *t = smbios_carve_table(*current, SMBIOS_IPMI_DEVICE_INFORMATION,
						     sizeof(*t), *handle);

	t->interface_type = interface_type;
	t->ipmi_rev = ipmi_rev;
	t->i2c_slave_addr = i2c_addr;
	t->nv_storage_addr = nv_addr;
	t->base_address = base_addr;
	t->base_address_modifier = base_modifier;
	t->irq = irq;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

int smbios_write_type39(unsigned long *current, int *handle,
			u8 unit_group, const char *loc, const char *dev_name,
			const char *man, const char *serial_num,
			const char *tag_num, const char *part_num,
			const char *rev_lvl, u16 max_pow_cap,
			const struct power_supply_ch *ps_ch)
{
	struct smbios_type39 *t = smbios_carve_table(*current,
						SMBIOS_SYSTEM_POWER_SUPPLY,
						sizeof(*t), *handle);

	uint16_t val = 0;
	uint16_t ps_type, ps_status, vol_switch, ps_unplug, ps_present, hot_rep;

	t->power_unit_group = unit_group;
	t->location = smbios_add_string(t->eos, loc);
	t->device_name = smbios_add_string(t->eos, dev_name);
	t->manufacturer = smbios_add_string(t->eos, man);
	t->serial_number = smbios_add_string(t->eos, serial_num);
	t->asset_tag_number = smbios_add_string(t->eos, tag_num);
	t->model_part_number = smbios_add_string(t->eos, part_num);
	t->revision_level = smbios_add_string(t->eos, rev_lvl);
	t->max_power_capacity = max_pow_cap;

	ps_type = ps_ch->power_supply_type & 0xF;
	ps_status = ps_ch->power_supply_status & 0x7;
	vol_switch = ps_ch->input_voltage_range_switch & 0xF;
	ps_unplug = ps_ch->power_supply_unplugged & 0x1;
	ps_present = ps_ch->power_supply_present & 0x1;
	hot_rep = ps_ch->power_supply_hot_replaceble & 0x1;

	val |= (ps_type << 10);
	val |= (ps_status << 7);
	val |= (vol_switch << 3);
	val |= (ps_unplug << 2);
	val |= (ps_present << 1);
	val |= hot_rep;
	t->power_supply_characteristics = val;

	t->input_voltage_probe_handle = 0xFFFF;
	t->cooling_device_handle = 0xFFFF;
	t->input_current_probe_handle = 0xFFFF;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

int smbios_write_type41(unsigned long *current, int *handle,
			const char *name, u8 instance, u16 segment_group,
			u8 bus, u8 device, u8 function, u8 device_type)
{
	struct smbios_type41 *t = smbios_carve_table(*current,
						SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION,
						sizeof(*t), *handle);

	t->reference_designation = smbios_add_string(t->eos, name);
	t->device_type = device_type;
	t->device_status = 1;
	t->device_type_instance = instance;
	t->segment_group_number = segment_group;
	t->bus_number = bus;
	t->device_number = device;
	t->function_number = function;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

int smbios_write_type43(unsigned long *current, int *handle, const u32 vendor_id,
			const u8 major_spec_ver, const u8 minor_spec_ver,
			const u32 fw_ver1, const u32 fw_ver2, const char *description,
			const u64 characteristics, const u32 oem_defined)
{
	struct smbios_type43 *t = smbios_carve_table(*current, SMBIOS_TPM_DEVICE,
						     sizeof(*t), *handle);

	t->vendor_id = vendor_id;
	t->major_spec_ver = major_spec_ver;
	t->minor_spec_ver = minor_spec_ver;
	t->fw_ver1 = fw_ver1;
	t->fw_ver2 = fw_ver2;
	t->characteristics = characteristics;
	t->oem_defined = oem_defined;
	t->description = smbios_add_string(t->eos, description);

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static int smbios_write_type127(unsigned long *current, int handle)
{
	struct smbios_type127 *t = smbios_carve_table(*current, SMBIOS_END_OF_TABLE,
						      sizeof(*t), handle);

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

/* Get the device type 41 from the dev struct */
static u8 smbios_get_device_type_from_dev(struct device *dev)
{
	u16 pci_basesubclass = (dev->class >> 8) & 0xFFFF;

	switch (pci_basesubclass) {
	case PCI_CLASS_NOT_DEFINED:
		return SMBIOS_DEVICE_TYPE_OTHER;
	case PCI_CLASS_DISPLAY_VGA:
	case PCI_CLASS_DISPLAY_XGA:
	case PCI_CLASS_DISPLAY_3D:
	case PCI_CLASS_DISPLAY_OTHER:
		return SMBIOS_DEVICE_TYPE_VIDEO;
	case PCI_CLASS_STORAGE_SCSI:
		return SMBIOS_DEVICE_TYPE_SCSI;
	case PCI_CLASS_NETWORK_ETHERNET:
		return SMBIOS_DEVICE_TYPE_ETHERNET;
	case PCI_CLASS_NETWORK_TOKEN_RING:
		return SMBIOS_DEVICE_TYPE_TOKEN_RING;
	case PCI_CLASS_MULTIMEDIA_VIDEO:
	case PCI_CLASS_MULTIMEDIA_AUDIO:
	case PCI_CLASS_MULTIMEDIA_PHONE:
	case PCI_CLASS_MULTIMEDIA_OTHER:
		return SMBIOS_DEVICE_TYPE_SOUND;
	case PCI_CLASS_STORAGE_ATA:
		return SMBIOS_DEVICE_TYPE_PATA;
	case PCI_CLASS_STORAGE_SATA:
		return SMBIOS_DEVICE_TYPE_SATA;
	case PCI_CLASS_STORAGE_SAS:
		return SMBIOS_DEVICE_TYPE_SAS;
	default:
		return SMBIOS_DEVICE_TYPE_UNKNOWN;
	}
}

static bool smbios_get_type41_instance_id(struct device *dev, u8 device_type, u8 *instance_id)
{
#if CONFIG(SMBIOS_TYPE41_PROVIDED_BY_DEVTREE)
	*instance_id = dev->smbios_instance_id;
	return dev->smbios_instance_id_valid;
#else
	static u8 type41_inst_cnt[SMBIOS_DEVICE_TYPE_COUNT + 1] = {};

	if (device_type == SMBIOS_DEVICE_TYPE_OTHER ||
	    device_type == SMBIOS_DEVICE_TYPE_UNKNOWN)
		return false;

	if (device_type > SMBIOS_DEVICE_TYPE_COUNT)
		return false;

	*instance_id = type41_inst_cnt[device_type]++;
	return true;
#endif
}

static const char *smbios_get_type41_refdes(struct device *dev)
{
#if CONFIG(SMBIOS_TYPE41_PROVIDED_BY_DEVTREE)
	if (dev->smbios_refdes)
		return dev->smbios_refdes;
#endif
#if CONFIG(PCI)
	return get_pci_subclass_name(dev);
#else
	return "???";
#endif
}

static int smbios_generate_type41_from_devtree(struct device *dev, int *handle,
					       unsigned long *current)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return 0;
	if (!dev->on_mainboard)
		return 0;

	const u8 device_type = smbios_get_device_type_from_dev(dev);

	u8 instance_id;

	if (!smbios_get_type41_instance_id(dev, device_type, &instance_id))
		return 0;

	const char *name = smbios_get_type41_refdes(dev);

	return smbios_write_type41(current, handle,
					name, // name
					instance_id, // inst
					dev->upstream->segment_group, // segment group
					dev->upstream->secondary, //bus
					PCI_SLOT(dev->path.pci.devfn), // device
					PCI_FUNC(dev->path.pci.devfn), // func
					device_type);
}

static int smbios_generate_type9_from_devtree(struct device *dev, int *handle,
					      unsigned long *current)
{
	enum misc_slot_usage usage;
	enum slot_data_bus_bandwidth bandwidth;
	enum misc_slot_type type;
	enum misc_slot_length length;

	if (dev->path.type != DEVICE_PATH_PCI)
		return 0;

	if (!dev->smbios_slot_type && !dev->smbios_slot_data_width &&
	    !dev->smbios_slot_designation && !dev->smbios_slot_length)
		return 0;

	if (dev_is_active_bridge(dev))
		usage = SlotUsageInUse;
	else if (dev->enabled)
		usage = SlotUsageAvailable;
	else
		usage = SlotUsageUnknown;

	if (dev->smbios_slot_data_width)
		bandwidth = dev->smbios_slot_data_width;
	else
		bandwidth = SlotDataBusWidthUnknown;

	if (dev->smbios_slot_type)
		type = dev->smbios_slot_type;
	else
		type = SlotTypeUnknown;

	if (dev->smbios_slot_length)
		length = dev->smbios_slot_length;
	else
		length = SlotLengthUnknown;

	return smbios_write_type9(current, handle,
				  dev->smbios_slot_designation,
				  type,
				  bandwidth,
				  usage,
				  length,
				  0,
				  1,
				  0,
				  dev->upstream->segment_group,
				  dev->upstream->secondary,
				  dev->path.pci.devfn);
}

int get_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	int len = 0;

	len += smbios_generate_type9_from_devtree(dev, handle, current);
	len += smbios_generate_type41_from_devtree(dev, handle, current);

	return len;
}

static int smbios_walk_device_tree(struct device *tree, int *handle, unsigned long *current)
{
	struct device *dev;
	int len = 0;

	for (dev = tree; dev; dev = dev->next) {
		if (!dev->enabled)
			continue;

		if (dev->ops && dev->ops->get_smbios_data) {
			printk(BIOS_INFO, "%s (%s)\n", dev_path(dev), dev_name(dev));
			len += dev->ops->get_smbios_data(dev, handle, current);
		} else {
			len += get_smbios_data(dev, handle, current);
		}
	}
	return len;
}

unsigned long smbios_write_tables(unsigned long current)
{
	struct smbios_entry *se = NULL;
	struct smbios_entry30 *se3;
	unsigned long tables;
	int len = 0;
	int max_struct_size = 0;
	int handle = 0;

	current = ALIGN_UP(current, 16);
	printk(BIOS_DEBUG, "%s: %08lx\n", __func__, current);

	// only add a 32 bit entry point if SMBIOS table is below 4G
	if (current < UINT32_MAX) {
		se = (struct smbios_entry *)current;
		current += sizeof(*se);
		current = ALIGN_UP(current, 16);
	}

	se3 = (struct smbios_entry30 *)current;
	current += sizeof(*se3);
	current = ALIGN_UP(current, 16);

	tables = current;
	update_max(len, max_struct_size, smbios_write_type0(&current, handle++));
	update_max(len, max_struct_size, smbios_write_type1(&current, handle++));

	/* The chassis handle is the next one */
	update_max(len, max_struct_size, smbios_write_type2(&current, handle, handle + 1));
	handle++;
	update_max(len, max_struct_size, smbios_write_type3(&current, handle++));

	struct smbios_type4 *type4 = (struct smbios_type4 *)current;
	update_max(len, max_struct_size, smbios_write_type4(&current, handle++));
	len += smbios_write_type7_cache_parameters(&current, &handle, &max_struct_size, type4);
	update_max(len, max_struct_size, smbios_write_type11(&current, &handle));
	if (CONFIG(ELOG))
		update_max(len, max_struct_size,
			elog_smbios_write_type15(&current, handle++));

	const int type16 = handle;
	update_max(len, max_struct_size, smbios_write_type16(&current, &handle));
	const int type17 = handle;
	update_max(len, max_struct_size, smbios_write_type17(&current, &handle, type16));
	const int type19 = handle;
	update_max(len, max_struct_size, smbios_write_type19(&current, &handle, type16));
	update_max(len, max_struct_size,
			smbios_write_type20(&current, &handle, type17, type19));
	update_max(len, max_struct_size, smbios_write_type32(&current, handle++));

	update_max(len, max_struct_size, smbios_walk_device_tree(all_devices,
								 &handle, &current));

	update_max(len, max_struct_size, smbios_write_type127(&current, handle++));

	if (se) {
		/* Install SMBIOS 2.1 entry point */
		memset(se, 0, sizeof(*se));
		memcpy(se->anchor, "_SM_", 4);
		se->length = sizeof(*se);
		se->major_version = 3;
		se->minor_version = 0;
		se->max_struct_size = max_struct_size;
		se->struct_count = handle;
		memcpy(se->intermediate_anchor_string, "_DMI_", 5);

		se->struct_table_address = (u32)tables;
		se->struct_table_length = len;

		se->intermediate_checksum = smbios_checksum((u8 *)se + 0x10, sizeof(*se) - 0x10);
		se->checksum = smbios_checksum((u8 *)se, sizeof(*se));
	}

	/* Install SMBIOS 3.0 entry point */
	memset(se3, 0, sizeof(*se3));
	memcpy(se3->anchor, "_SM3_", 5);
	se3->length = sizeof(*se3);
	se3->major_version = 3;
	se3->minor_version = 0;

	se3->struct_table_address = (u64)tables;
	se3->struct_table_length = len;

	se3->checksum = smbios_checksum((u8 *)se3, sizeof(*se3));

	return current;
}
