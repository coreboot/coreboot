/* SPDX-License-Identifier: GPL-2.0-only */

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
#include <commonlib/helpers.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <device/pci.h>
#if CONFIG(CHROMEOS)
#include <vendorcode/google/chromeos/gnvs.h>
#endif

#define update_max(len, max_len, stmt)		\
	do {					\
		int tmp = stmt;			\
						\
		max_len = MAX(max_len, tmp);	\
		len += tmp;			\
	} while (0)

static u8 smbios_checksum(u8 *p, u32 length)
{
	u8 ret = 0;
	while (length--)
		ret += *p++;
	return -ret;
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


int smbios_add_string(u8 *start, const char *str)
{
	int i = 1;
	char *p = (char *)start;

	/*
	 * Return 0 as required for empty strings.
	 * See Section 6.1.3 "Text Strings" of the SMBIOS specification.
	 */
	if (*str == '\0')
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

static int smbios_cpu_vendor(u8 *start)
{
	if (cpu_have_cpuid()) {
		u32 tmp[4];
		const struct cpuid_result res = cpuid(0);
		tmp[0] = res.ebx;
		tmp[1] = res.edx;
		tmp[2] = res.ecx;
		tmp[3] = 0;
		return smbios_add_string(start, (const char *)tmp);
	} else {
		return smbios_add_string(start, "Unknown");
	}
}

static int smbios_processor_name(u8 *start)
{
	u32 tmp[13];
	const char *str = "Unknown Processor Name";
	if (cpu_have_cpuid()) {
		int i;
		struct cpuid_result res = cpuid(0x80000000);
		if (res.eax >= 0x80000004) {
			int j = 0;
			for (i = 0; i < 3; i++) {
				res = cpuid(0x80000002 + i);
				tmp[j++] = res.eax;
				tmp[j++] = res.ebx;
				tmp[j++] = res.ecx;
				tmp[j++] = res.edx;
			}
			tmp[12] = 0;
			str = (const char *)tmp;
		}
	}
	return smbios_add_string(start, str);
}

/* this function will fill the corresponding manufacturer */
void smbios_fill_dimm_manufacturer_from_id(uint16_t mod_id,
	struct smbios_type17 *t)
{
	switch (mod_id) {
	case 0x9b85:
		t->manufacturer = smbios_add_string(t->eos,
						    "Crucial");
		break;
	case 0x4304:
		t->manufacturer = smbios_add_string(t->eos,
						    "Ramaxel");
		break;
	case 0x4f01:
		t->manufacturer = smbios_add_string(t->eos,
						    "Transcend");
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
	case 0xb004:
		t->manufacturer = smbios_add_string(t->eos,
						    "OCZ");
		break;
	case 0xad80:
		t->manufacturer = smbios_add_string(t->eos,
						    "Hynix/Hyundai");
		break;
	case 0x3486:
		t->manufacturer = smbios_add_string(t->eos,
						    "Super Talent");
		break;
	case 0xcd04:
		t->manufacturer = smbios_add_string(t->eos,
						    "GSkill");
		break;
	case 0xce80:
		t->manufacturer = smbios_add_string(t->eos,
						    "Samsung");
		break;
	case 0xfe02:
		t->manufacturer = smbios_add_string(t->eos,
						    "Elpida");
		break;
	case 0x2c80:
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
/* this function will fill the corresponding locator */
void __weak smbios_fill_dimm_locator(const struct dimm_info *dimm,
	struct smbios_type17 *t)
{
	char locator[40];

	snprintf(locator, sizeof(locator), "Channel-%d-DIMM-%d",
		dimm->channel_num, dimm->dimm_num);
	t->device_locator = smbios_add_string(t->eos, locator);

	snprintf(locator, sizeof(locator), "BANK %d", dimm->bank_locator);
	t->bank_locator = smbios_add_string(t->eos, locator);
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
static void smbios_fill_dimm_part_number(const char *part_number,
					 struct smbios_type17 *t)
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
		 dimm->serial[0], dimm->serial[1], dimm->serial[2],
		 dimm->serial[3]);

	t->serial_number = smbios_add_string(t->eos, serial);
}

static int create_smbios_type17_for_dimm(struct dimm_info *dimm,
					 unsigned long *current, int *handle)
{
	struct smbios_type17 *t = (struct smbios_type17 *)*current;

	memset(t, 0, sizeof(struct smbios_type17));
	t->memory_type = dimm->ddr_type;
	t->clock_speed = dimm->ddr_frequency;
	t->speed = dimm->ddr_frequency;
	t->type = SMBIOS_MEMORY_DEVICE;
	if (dimm->dimm_size < 0x7fff) {
		t->size = dimm->dimm_size;
	} else {
		t->size = 0x7fff;
		t->extended_size = dimm->dimm_size & 0x7fffffff;
	}
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
	smbios_fill_dimm_serial_number(dimm, t);
	smbios_fill_dimm_locator(dimm, t);

	/* put '\0' in the end of data */
	dimm->module_part_number[DIMM_INFO_PART_NUMBER_SIZE - 1] = '\0';
	smbios_fill_dimm_part_number((char *)dimm->module_part_number, t);

	/* Voltage Levels */
	t->configured_voltage = dimm->vdd_voltage;
	t->minimum_voltage = dimm->vdd_voltage;
	t->maximum_voltage = dimm->vdd_voltage;

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

const char *__weak smbios_mainboard_bios_version(void)
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
#if !CONFIG(CHROMEOS)
	t->bios_release_date = smbios_add_string(t->eos, coreboot_dmi_date);

	t->bios_version = smbios_add_string(t->eos,
		smbios_mainboard_bios_version());
#else
#define SPACES \
	"                                                                  "
	t->bios_release_date = smbios_add_string(t->eos, coreboot_dmi_date);
#if CONFIG(HAVE_ACPI_TABLES)
	u32 version_offset = (u32)smbios_string_table_len(t->eos);
#endif
	t->bios_version = smbios_add_string(t->eos, SPACES);

#if CONFIG(HAVE_ACPI_TABLES)
	/* SMBIOS offsets start at 1 rather than 0 */
	chromeos_get_chromeos_acpi()->vbt10 =
		(u32)t->eos + (version_offset - 1);
#endif
#endif /* CONFIG_CHROMEOS */

	uint32_t rom_size = CONFIG_ROM_SIZE;
	rom_size = MIN(CONFIG_ROM_SIZE, 16 * MiB);
	t->bios_rom_size = (rom_size / 65535) - 1;

	if (CONFIG_ROM_SIZE >= 1 * GiB) {
		t->extended_bios_rom_size =
			DIV_ROUND_UP(CONFIG_ROM_SIZE, GiB) | (1 << 14);
	} else {
		t->extended_bios_rom_size = DIV_ROUND_UP(CONFIG_ROM_SIZE, MiB);
	}

	t->system_bios_major_release = coreboot_major_revision;
	t->system_bios_minor_release = coreboot_minor_revision;

	t->bios_characteristics =
		BIOS_CHARACTERISTICS_PCI_SUPPORTED |
		BIOS_CHARACTERISTICS_SELECTABLE_BOOT |
		BIOS_CHARACTERISTICS_UPGRADEABLE;

	if (CONFIG(CARDBUS_PLUGIN_SUPPORT))
		t->bios_characteristics |= BIOS_CHARACTERISTICS_PC_CARD;

	if (CONFIG(HAVE_ACPI_TABLES))
		t->bios_characteristics_ext1 = BIOS_EXT1_CHARACTERISTICS_ACPI;

	t->bios_characteristics_ext2 = BIOS_EXT2_CHARACTERISTICS_TARGET;
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

const char *__weak smbios_mainboard_serial_number(void)
{
	return CONFIG_MAINBOARD_SERIAL_NUMBER;
}

const char *__weak smbios_mainboard_version(void)
{
	return CONFIG_MAINBOARD_VERSION;
}

const char *__weak smbios_mainboard_manufacturer(void)
{
	return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
}

const char *__weak smbios_mainboard_product_name(void)
{
	return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

const char *__weak smbios_mainboard_asset_tag(void)
{
	return "";
}

u8 __weak smbios_mainboard_feature_flags(void)
{
	return 0;
}

const char *__weak smbios_mainboard_location_in_chassis(void)
{
	return "";
}

smbios_board_type __weak smbios_mainboard_board_type(void)
{
	return SMBIOS_BOARD_TYPE_UNKNOWN;
}

/*
 * System Enclosure or Chassis Types as defined in SMBIOS specification.
 * The default value is SMBIOS_ENCLOSURE_DESKTOP (0x03) but laptop,
 * convertible, or tablet enclosure will be used if the appropriate
 * system type is selected.
 */
smbios_enclosure_type __weak smbios_mainboard_enclosure_type(void)
{
	if (CONFIG(SYSTEM_TYPE_LAPTOP))
		return SMBIOS_ENCLOSURE_LAPTOP;
	else if (CONFIG(SYSTEM_TYPE_TABLET))
		return SMBIOS_ENCLOSURE_TABLET;
	else if (CONFIG(SYSTEM_TYPE_CONVERTIBLE))
		return SMBIOS_ENCLOSURE_CONVERTIBLE;
	else if (CONFIG(SYSTEM_TYPE_DETACHABLE))
		return SMBIOS_ENCLOSURE_DETACHABLE;
	else
		return SMBIOS_ENCLOSURE_DESKTOP;
}

const char *__weak smbios_system_serial_number(void)
{
	return smbios_mainboard_serial_number();
}

const char *__weak smbios_system_version(void)
{
	return smbios_mainboard_version();
}

const char *__weak smbios_system_manufacturer(void)
{
	return smbios_mainboard_manufacturer();
}

const char *__weak smbios_system_product_name(void)
{
	return smbios_mainboard_product_name();
}

void __weak smbios_system_set_uuid(u8 *uuid)
{
	/* leave all zero */
}

unsigned int __weak smbios_cpu_get_max_speed_mhz(void)
{
	return 0; /* Unknown */
}

unsigned int __weak smbios_cpu_get_current_speed_mhz(void)
{
	return 0; /* Unknown */
}

const char *__weak smbios_system_sku(void)
{
	return "";
}

const char * __weak smbios_chassis_version(void)
{
	return "";
}

const char * __weak smbios_chassis_serial_number(void)
{
	return "";
}

const char * __weak smbios_processor_serial_number(void)
{
	return "";
}

static int get_socket_type(void)
{
	if (CONFIG(CPU_INTEL_SLOT_1))
		return 0x08;
	if (CONFIG(CPU_INTEL_SOCKET_MPGA604))
		return 0x13;
	if (CONFIG(CPU_INTEL_SOCKET_LGA775))
		return 0x15;

	return 0x02; /* Unknown */
}

static int smbios_write_type1(unsigned long *current, int handle)
{
	struct smbios_type1 *t = (struct smbios_type1 *)*current;
	int len = sizeof(struct smbios_type1);

	memset(t, 0, sizeof(struct smbios_type1));
	t->type = SMBIOS_SYSTEM_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->manufacturer = smbios_add_string(t->eos,
		smbios_system_manufacturer());
	t->product_name = smbios_add_string(t->eos,
		smbios_system_product_name());
	t->serial_number = smbios_add_string(t->eos,
		smbios_system_serial_number());
	t->sku = smbios_add_string(t->eos, smbios_system_sku());
	t->version = smbios_add_string(t->eos, smbios_system_version());
#ifdef CONFIG_MAINBOARD_FAMILY
	t->family = smbios_add_string(t->eos, CONFIG_MAINBOARD_FAMILY);
#endif
	smbios_system_set_uuid(t->uuid);
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int smbios_write_type2(unsigned long *current, int handle,
			      const int chassis_handle)
{
	struct smbios_type2 *t = (struct smbios_type2 *)*current;
	int len = sizeof(struct smbios_type2);

	memset(t, 0, sizeof(struct smbios_type2));
	t->type = SMBIOS_BOARD_INFORMATION;
	t->handle = handle;
	t->length = len - 2;
	t->manufacturer = smbios_add_string(t->eos,
		smbios_mainboard_manufacturer());
	t->product_name = smbios_add_string(t->eos,
		smbios_mainboard_product_name());
	t->serial_number = smbios_add_string(t->eos,
		smbios_mainboard_serial_number());
	t->version = smbios_add_string(t->eos, smbios_mainboard_version());
	t->asset_tag = smbios_add_string(t->eos, smbios_mainboard_asset_tag());
	t->feature_flags = smbios_mainboard_feature_flags();
	t->location_in_chassis = smbios_add_string(t->eos,
		smbios_mainboard_location_in_chassis());
	t->board_type = smbios_mainboard_board_type();
	t->chassis_handle = chassis_handle;
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
	t->manufacturer = smbios_add_string(t->eos,
		smbios_system_manufacturer());
	t->bootup_state = SMBIOS_STATE_SAFE;
	t->power_supply_state = SMBIOS_STATE_SAFE;
	t->thermal_state = SMBIOS_STATE_SAFE;
	t->_type = smbios_mainboard_enclosure_type();
	t->security_status = SMBIOS_STATE_SAFE;
	t->asset_tag_number = smbios_add_string(t->eos, smbios_mainboard_asset_tag());
	t->version = smbios_add_string(t->eos, smbios_chassis_version());
	t->serial_number = smbios_add_string(t->eos, smbios_chassis_serial_number());
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

	if (cpu_have_cpuid())
		res = cpuid(1);

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
	/*
	 * If CPUID leaf 11 is available, calculate "core count" by dividing
	 * SMT_ID (logical processors in a core) by Core_ID (number of cores).
	 * This seems to be the way to arrive to a number of cores mentioned on
	 * ark.intel.com.
	 */
	if (cpu_have_cpuid() && cpuid_get_max_func() >= 0xb) {
		uint32_t leaf_b_cores = 0, leaf_b_threads = 0;
		res = cpuid_ext(0xb, 1);
		leaf_b_cores = res.ebx;
		res = cpuid_ext(0xb, 0);
		leaf_b_threads = res.ebx;
		/* if hyperthreading is not available, pretend this is 1 */
		if (leaf_b_threads == 0) {
			leaf_b_threads = 1;
		}
		t->core_count = leaf_b_cores / leaf_b_threads;
	} else {
		t->core_count = (res.ebx >> 16) & 0xff;
	}
	/* Assume we enable all the cores always, capped only by MAX_CPUS */
	t->core_enabled = MIN(t->core_count, CONFIG_MAX_CPUS);
	t->l1_cache_handle = 0xffff;
	t->l2_cache_handle = 0xffff;
	t->l3_cache_handle = 0xffff;
	t->serial_number = smbios_add_string(t->eos, smbios_processor_serial_number());
	t->processor_upgrade = get_socket_type();
	len = t->length + smbios_string_table_len(t->eos);
	if (cpu_have_cpuid() && cpuid_get_max_func() >= 0x16) {
		t->max_speed = cpuid_ebx(0x16);
		t->current_speed = cpuid_eax(0x16); /* base frequency */
	} else {
		t->max_speed = smbios_cpu_get_max_speed_mhz();
		t->current_speed = smbios_cpu_get_current_speed_mhz();
	}
	*current += len;
	return len;
}

/*
 * Write SMBIOS type 7.
 * Fill in some fields with constant values, as gathering the information
 * from CPUID is impossible.
 */
static int
smbios_write_type7(unsigned long *current,
		   const int handle,
		   const u8 level,
		   const u8 sram_type,
		   const enum smbios_cache_associativity associativity,
		   const enum smbios_cache_type type,
		   const size_t max_cache_size,
		   const size_t cache_size)
{
	struct smbios_type7 *t = (struct smbios_type7 *)*current;
	int len = sizeof(struct smbios_type7);
	static unsigned int cnt = 0;
	char buf[8];

	memset(t, 0, sizeof(struct smbios_type7));
	t->type = SMBIOS_CACHE_INFORMATION;
	t->handle = handle;
	t->length = len - 2;

	snprintf(buf, sizeof(buf), "CACHE%x", cnt++);
	t->socket_designation = smbios_add_string(t->eos, buf);

	t->cache_configuration = SMBIOS_CACHE_CONF_LEVEL(level) |
		SMBIOS_CACHE_CONF_LOCATION(0) | /* Internal */
		SMBIOS_CACHE_CONF_ENABLED(1) | /* Enabled */
		SMBIOS_CACHE_CONF_OPERATION_MODE(3); /* Unknown */

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
	t->error_correction_type = SMBIOS_CACHE_ERROR_CORRECTION_UNKNOWN;
	t->system_cache_type = type;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

/* Convert the associativity as integer to the SMBIOS enum if available */
static enum smbios_cache_associativity
smbios_cache_associativity(const u8 num)
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

/*
 * Parse the "Deterministic Cache Parameters" as provided by Intel in
 * leaf 4 or AMD in extended leaf 0x8000001d.
 *
 * @param current Pointer to memory address to write the tables to
 * @param handle Pointer to handle for the tables
 * @param max_struct_size Pointer to maximum struct size
 * @param type4 Pointer to SMBIOS type 4 structure
 */
static int smbios_write_type7_cache_parameters(unsigned long *current,
					       int *handle,
					       int *max_struct_size,
					       struct smbios_type4 *type4)
{
	struct cpuid_result res;
	unsigned int cnt = 0;
	int len = 0;
	u32 leaf;

	if (!cpu_have_cpuid())
		return len;

	if (cpu_is_intel()) {
		res = cpuid(0);
		if (res.eax < 4)
			return len;
		leaf = 4;
	} else if (cpu_is_amd()) {
		res = cpuid(0x80000000);
		if (res.eax < 0x80000001)
			return len;

		res = cpuid(0x80000001);
		if (!(res.ecx & (1 << 22)))
			return len;

		leaf = 0x8000001d;
	} else {
		printk(BIOS_DEBUG, "SMBIOS: Unknown CPU\n");
		return len;
	}

	while (1) {
		enum smbios_cache_associativity associativity;
		enum smbios_cache_type type;

		res = cpuid_ext(leaf, cnt++);

		const u8 cache_type = CPUID_CACHE_TYPE(res);
		const u8 level = CPUID_CACHE_LEVEL(res);
		const size_t assoc = CPUID_CACHE_WAYS_OF_ASSOC(res) + 1;
		const size_t partitions = CPUID_CACHE_PHYS_LINE(res) + 1;
		const size_t cache_line_size = CPUID_CACHE_COHER_LINE(res) + 1;
		const size_t number_of_sets = CPUID_CACHE_NO_OF_SETS(res) + 1;
		const size_t cache_size = assoc * partitions * cache_line_size *
					number_of_sets;

		if (!cache_type)
			/* No more caches in the system */
			break;

		switch (cache_type) {
		case 1:
			type = SMBIOS_CACHE_TYPE_DATA;
			break;
		case 2:
			type = SMBIOS_CACHE_TYPE_INSTRUCTION;
			break;
		case 3:
			type = SMBIOS_CACHE_TYPE_UNIFIED;
			break;
		default:
			type = SMBIOS_CACHE_TYPE_UNKNOWN;
			break;
		}

		if (CPUID_CACHE_FULL_ASSOC(res))
			associativity = SMBIOS_CACHE_ASSOCIATIVITY_FULL;
		else
			associativity = smbios_cache_associativity(assoc);

		const int h = (*handle)++;

		update_max(len, *max_struct_size, smbios_write_type7(current, h,
			   level, SMBIOS_CACHE_SRAM_TYPE_UNKNOWN, associativity,
			   type, cache_size, cache_size));

		if (type4) {
			switch (level) {
			case 1:
				type4->l1_cache_handle = h;
				break;
			case 2:
				type4->l2_cache_handle = h;
				break;
			case 3:
				type4->l3_cache_handle = h;
				break;
			}
		}
	};

	return len;
}
int smbios_write_type9(unsigned long *current, int *handle,
			const char *name, const enum misc_slot_type type,
			const enum slot_data_bus_bandwidth bandwidth,
			const enum misc_slot_usage usage,
			const enum misc_slot_length length,
			u8 slot_char1, u8 slot_char2, u8 bus, u8 dev_func)
{
	struct smbios_type9 *t = (struct smbios_type9 *)*current;
	int len = sizeof(struct smbios_type9);

	memset(t, 0, sizeof(struct smbios_type9));
	t->type = SMBIOS_SYSTEM_SLOTS;
	t->handle = *handle;
	t->length = len - 2;
	if (name)
		t->slot_designation = smbios_add_string(t->eos, name);
	else
		t->slot_designation = smbios_add_string(t->eos, "SLOT");
	t->slot_type = type;
	/* TODO add slot_id supoort, will be "_SUN" for ACPI devices */
	t->slot_data_bus_width = bandwidth;
	t->current_usage = usage;
	t->slot_length = length;
	t->slot_characteristics_1 = slot_char1;
	t->slot_characteristics_2 = slot_char2;
	t->segment_group_number = 0;
	t->bus_number = bus;
	t->device_function_number = dev_func;
	t->data_bus_width = SlotDataBusWidthOther;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static int smbios_write_type11(unsigned long *current, int *handle)
{
	struct smbios_type11 *t = (struct smbios_type11 *)*current;
	int len;
	struct device *dev;

	memset(t, 0, sizeof(*t));
	t->type = SMBIOS_OEM_STRINGS;
	t->handle = *handle;
	t->length = len = sizeof(*t) - 2;

	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->ops && dev->ops->get_smbios_strings)
			dev->ops->get_smbios_strings(dev, t);
	}

	if (t->count == 0) {
		memset(t, 0, sizeof(*t));
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
	int totallen = 0;
	int i;

	struct memory_info *meminfo;
	meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo == NULL)
		return 0;	/* can't find mem info in cbmem */

	printk(BIOS_INFO, "Create SMBIOS type 17\n");
	for (i = 0; i < meminfo->dimm_cnt && i < ARRAY_SIZE(meminfo->dimm);
		i++) {
		struct dimm_info *dimm;
		dimm = &meminfo->dimm[i];
		len = create_smbios_type17_for_dimm(dimm, current, handle);
		*current += len;
		totallen += len;
	}
	return totallen;
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

int smbios_write_type38(unsigned long *current, int *handle,
			const enum smbios_bmc_interface_type interface_type,
			const u8 ipmi_rev, const u8 i2c_addr, const u8 nv_addr,
			const u64 base_addr, const u8 base_modifier,
			const u8 irq)
{
	struct smbios_type38 *t = (struct smbios_type38 *)*current;
	int len = sizeof(struct smbios_type38);

	memset(t, 0, sizeof(struct smbios_type38));
	t->type = SMBIOS_IPMI_DEVICE_INFORMATION;
	t->handle = *handle;
	t->length = len - 2;
	t->interface_type = interface_type;
	t->ipmi_rev = ipmi_rev;
	t->i2c_slave_addr = i2c_addr;
	t->nv_storage_addr = nv_addr;
	t->base_address = base_addr;
	t->base_address_modifier = base_modifier;
	t->irq = irq;

	*current += len;
	*handle += 1;

	return len;
}

int smbios_write_type41(unsigned long *current, int *handle,
			const char *name, u8 instance, u16 segment,
			u8 bus, u8 device, u8 function, u8 device_type)
{
	struct smbios_type41 *t = (struct smbios_type41 *)*current;
	int len = sizeof(struct smbios_type41);

	memset(t, 0, sizeof(struct smbios_type41));
	t->type = SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION;
	t->handle = *handle;
	t->length = len - 2;
	t->reference_designation = smbios_add_string(t->eos, name);
	t->device_type = device_type;
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

/* Generate Type41 entries from devicetree */
static int smbios_walk_device_tree_type41(struct device *dev, int *handle,
					unsigned long *current)
{
	static u8 type41_inst_cnt[SMBIOS_DEVICE_TYPE_COUNT + 1] = {};

	if (dev->path.type != DEVICE_PATH_PCI)
		return 0;
	if (!dev->on_mainboard)
		return 0;

	u8 device_type = smbios_get_device_type_from_dev(dev);

	if (device_type == SMBIOS_DEVICE_TYPE_OTHER ||
	    device_type == SMBIOS_DEVICE_TYPE_UNKNOWN)
		return 0;

	if (device_type > SMBIOS_DEVICE_TYPE_COUNT)
		return 0;

	const char *name = get_pci_subclass_name(dev);

	return smbios_write_type41(current, handle,
					name, // name
					type41_inst_cnt[device_type]++, // inst
					0, // segment
					dev->bus->secondary, //bus
					PCI_SLOT(dev->path.pci.devfn), // device
					PCI_FUNC(dev->path.pci.devfn), // func
					device_type);
}

/* Generate Type9 entries from devicetree */
static int smbios_walk_device_tree_type9(struct device *dev, int *handle,
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
				  1,
				  0,
				  dev->bus->secondary,
				  dev->path.pci.devfn);
}

static int smbios_walk_device_tree(struct device *tree, int *handle,
	unsigned long *current)
{
	struct device *dev;
	int len = 0;

	for (dev = tree; dev; dev = dev->next) {
		if (dev->enabled && dev->ops && dev->ops->get_smbios_data) {
			printk(BIOS_INFO, "%s (%s)\n", dev_path(dev),
				dev_name(dev));
			len += dev->ops->get_smbios_data(dev, handle, current);
		}
		len += smbios_walk_device_tree_type9(dev, handle, current);
		len += smbios_walk_device_tree_type41(dev, handle, current);
	}
	return len;
}

unsigned long smbios_write_tables(unsigned long current)
{
	struct smbios_entry *se;
	unsigned long tables;
	int len = 0;
	int max_struct_size = 0;
	int handle = 0;

	current = ALIGN_UP(current, 16);
	printk(BIOS_DEBUG, "%s: %08lx\n", __func__, current);

	se = (struct smbios_entry *)current;
	current += sizeof(struct smbios_entry);
	current = ALIGN_UP(current, 16);

	tables = current;
	update_max(len, max_struct_size, smbios_write_type0(&current,
		handle++));
	update_max(len, max_struct_size, smbios_write_type1(&current,
		handle++));
	update_max(len, max_struct_size, smbios_write_type2(&current,
		handle, handle + 1)); /* The chassis handle is the next one */
	handle++;
	update_max(len, max_struct_size, smbios_write_type3(&current,
		handle++));

	struct smbios_type4 *type4 = (struct smbios_type4 *)current;
	update_max(len, max_struct_size, smbios_write_type4(&current,
		handle++));
	len += smbios_write_type7_cache_parameters(&current, &handle,
		&max_struct_size, type4);
	update_max(len, max_struct_size, smbios_write_type11(&current,
		&handle));
	if (CONFIG(ELOG))
		update_max(len, max_struct_size,
			elog_smbios_write_type15(&current,handle++));
	update_max(len, max_struct_size, smbios_write_type17(&current,
		&handle));
	update_max(len, max_struct_size, smbios_write_type32(&current,
		handle++));

	update_max(len, max_struct_size, smbios_walk_device_tree(all_devices,
		&handle, &current));

	update_max(len, max_struct_size, smbios_write_type127(&current,
		handle++));

	memset(se, 0, sizeof(struct smbios_entry));
	memcpy(se->anchor, "_SM_", 4);
	se->length = sizeof(struct smbios_entry);
	se->major_version = 2;
	se->minor_version = 8;
	se->max_struct_size = max_struct_size;
	se->struct_count = handle;
	memcpy(se->intermediate_anchor_string, "_DMI_", 5);

	se->struct_table_address = (u32)tables;
	se->struct_table_length = len;

	se->intermediate_checksum = smbios_checksum((u8 *)se + 0x10,
						    sizeof(struct smbios_entry)
						    - 0x10);
	se->checksum = smbios_checksum((u8 *)se, sizeof(struct smbios_entry));
	return current;
}
