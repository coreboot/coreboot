/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SMBIOS_H
#define SMBIOS_H

#include <types.h>
#include <memory_info.h>

unsigned long smbios_write_tables(unsigned long start);
int smbios_add_string(u8 *start, const char *str);
int smbios_string_table_len(u8 *start);

struct smbios_header;
int smbios_full_table_len(struct smbios_header *header, u8 *str_table_start);
void *smbios_carve_table(unsigned long start, u8 type, u8 length, u16 handle);

/* Used by mainboard to add an on-board device */
enum misc_slot_type;
enum misc_slot_length;
enum misc_slot_usage;
enum slot_data_bus_bandwidth;
int smbios_write_type9(unsigned long *current, int *handle,
			const char *name, const enum misc_slot_type type,
			const enum slot_data_bus_bandwidth bandwidth,
			const enum misc_slot_usage usage,
			const enum misc_slot_length length,
			const u16 id, u8 slot_char1, u8 slot_char2, u8 bus, u8 dev_func);
enum smbios_bmc_interface_type;
int smbios_write_type38(unsigned long *current, int *handle,
			const enum smbios_bmc_interface_type interface_type,
			const u8 ipmi_rev, const u8 i2c_addr, const u8 nv_addr,
			const u64 base_addr, const u8 base_modifier,
			const u8 irq);
int smbios_write_type41(unsigned long *current, int *handle,
			const char *name, u8 instance, u16 segment,
			u8 bus, u8 device, u8 function, u8 device_type);
enum smbios_temp_location;
enum smbios_temp_status;
int smbios_write_type28(unsigned long *current, int *handle,
			const char *name,
			const enum smbios_temp_location location,
			const enum smbios_temp_status status,
			u16 max_value, u16 min_value,
			u16 resolution, u16 tolerance,
			u16 accuracy,
			u32 oem,
			u16 nominal_value);

int smbios_write_type43(unsigned long *current, int *handle, const u32 vendor_id,
			const u8 major_spec_ver, const u8 minor_spec_ver,
			const u32 fw_ver1, const u32 fw_ver2, const char *description,
			const u64 characteristics, const u32 oem_defined);

struct device;
int get_smbios_data(struct device *dev, int *handle, unsigned long *current);

const char *smbios_system_manufacturer(void);
const char *smbios_system_product_name(void);
const char *smbios_system_serial_number(void);
const char *smbios_system_version(void);
void smbios_system_set_uuid(u8 *uuid);
const char *smbios_system_sku(void);

unsigned int smbios_cpu_get_max_speed_mhz(void);
unsigned int smbios_cpu_get_current_speed_mhz(void);
unsigned int smbios_cpu_get_voltage(void);

const char *smbios_mainboard_manufacturer(void);
const char *smbios_mainboard_product_name(void);
const char *smbios_mainboard_serial_number(void);
const char *smbios_mainboard_version(void);

const char *smbios_mainboard_bios_version(void);
const char *smbios_mainboard_asset_tag(void);
u8 smbios_mainboard_feature_flags(void);
const char *smbios_mainboard_location_in_chassis(void);
const char *smbios_chassis_version(void);
const char *smbios_chassis_serial_number(void);
const char *smbios_processor_serial_number(void);
u8 smbios_chassis_power_cords(void);

/* This string could be filled late in payload. */
void smbios_type0_bios_version(uintptr_t address);

void smbios_ec_revision(uint8_t *ec_major_revision, uint8_t *ec_minor_revision);

unsigned int smbios_processor_external_clock(void);
unsigned int smbios_processor_characteristics(void);
struct cpuid_result;
unsigned int smbios_processor_family(struct cpuid_result res);

unsigned int smbios_cache_error_correction_type(u8 level);
unsigned int smbios_cache_sram_type(void);
unsigned int smbios_cache_conf_operation_mode(u8 level);

/* Used by mainboard to add port information of type 8 */
struct port_information;
int smbios_write_type8(unsigned long *current, int *handle,
			const struct port_information *port,
			size_t num_ports);

#define BIOS_CHARACTERISTICS_PCI_SUPPORTED	(1 << 7)
#define BIOS_CHARACTERISTICS_PC_CARD		(1 << 8)
#define BIOS_CHARACTERISTICS_PNP		(1 << 9)
#define BIOS_CHARACTERISTICS_APM		(1 << 10)
#define BIOS_CHARACTERISTICS_UPGRADEABLE	(1 << 11)
#define BIOS_CHARACTERISTICS_SHADOW		(1 << 12)
#define BIOS_CHARACTERISTICS_BOOT_FROM_CD	(1 << 15)
#define BIOS_CHARACTERISTICS_SELECTABLE_BOOT	(1 << 16)
#define BIOS_CHARACTERISTICS_BIOS_SOCKETED	(1 << 17)

#define BIOS_EXT1_CHARACTERISTICS_ACPI		(1 << 0)
#define BIOS_EXT2_CHARACTERISTICS_TARGET	(1 << 2)

#define BIOS_MEMORY_ECC_SINGLE_BIT_CORRECTING	(1 << 3)
#define BIOS_MEMORY_ECC_DOUBLE_BIT_CORRECTING	(1 << 4)
#define BIOS_MEMORY_ECC_SCRUBBING		(1 << 5)

#define MEMORY_TYPE_DETAIL_OTHER		(1 << 1)
#define MEMORY_TYPE_DETAIL_UNKNOWN		(1 << 2)
#define MEMORY_TYPE_DETAIL_FAST_PAGED		(1 << 3)
#define MEMORY_TYPE_DETAIL_STATIC_COLUMN	(1 << 4)
#define MEMORY_TYPE_DETAIL_PSEUDO_STATIC	(1 << 5)
#define MEMORY_TYPE_DETAIL_RAMBUS		(1 << 6)
#define MEMORY_TYPE_DETAIL_SYNCHRONOUS		(1 << 7)
#define MEMORY_TYPE_DETAIL_CMOS			(1 << 8)
#define MEMORY_TYPE_DETAIL_EDO			(1 << 9)
#define MEMORY_TYPE_DETAIL_WINDOW_DRAM		(1 << 10)
#define MEMORY_TYPE_DETAIL_CACHE_DRAM		(1 << 11)
#define MEMORY_TYPE_DETAIL_NON_VOLATILE		(1 << 12)
#define MEMORY_TYPE_DETAIL_REGISTERED		(1 << 13)
#define MEMORY_TYPE_DETAIL_UNBUFFERED		(1 << 14)
#define MEMORY_TYPE_DETAIL_LRDIMM		(1 << 15)

#define MEMORY_TECHNOLOGY_OTHER			0x01
#define MEMORY_TECHNOLOGY_UNKNOWN		0x02
#define MEMORY_TECHNOLOGY_DRAM			0x03
#define MEMORY_TECHNOLOGY_NVDIMM_N		0x04
#define MEMORY_TECHNOLOGY_NVDIMM_F		0x05
#define MEMORY_TECHNOLOGY_NVDIMM_P		0x06
#define MEMORY_TECHNOLOGY_INTEL_PERSISTENT	0x07

#define MEMORY_OPERATING_MODE_CAP_OTHER				(1 << 1)
#define MEMORY_OPERATING_MODE_CAP_UNKNOWN			(1 << 2)
#define MEMORY_OPERATING_MODE_CAP_VOLATILE			(1 << 3)
#define MEMORY_OPERATING_MODE_CAP_BYTE_ACCESS_PERSISTENT	(1 << 4)
#define MEMORY_OPERATING_MODE_CAP_BLOCK_ACCESS_PERSISTENT	(1 << 5)

typedef enum {
	MEMORY_BUS_WIDTH_8 = 0,
	MEMORY_BUS_WIDTH_16 = 1,
	MEMORY_BUS_WIDTH_32 = 2,
	MEMORY_BUS_WIDTH_64 = 3,
	MEMORY_BUS_WIDTH_128 = 4,
	MEMORY_BUS_WIDTH_256 = 5,
	MEMORY_BUS_WIDTH_512 = 6,
	MEMORY_BUS_WIDTH_1024 = 7,
	MEMORY_BUS_WIDTH_MAX = 7,
} smbios_memory_bus_width;

typedef enum {
	MEMORY_FORMFACTOR_OTHER = 0x01,
	MEMORY_FORMFACTOR_UNKNOWN = 0x02,
	MEMORY_FORMFACTOR_SIMM = 0x03,
	MEMORY_FORMFACTOR_SIP = 0x04,
	MEMORY_FORMFACTOR_CHIP = 0x05,
	MEMORY_FORMFACTOR_DIP = 0x06,
	MEMORY_FORMFACTOR_ZIP = 0x07,
	MEMORY_FORMFACTOR_PROPRIETARY_CARD = 0x08,
	MEMORY_FORMFACTOR_DIMM = 0x09,
	MEMORY_FORMFACTOR_TSOP = 0x0a,
	MEMORY_FORMFACTOR_ROC = 0x0b,
	MEMORY_FORMFACTOR_RIMM = 0x0c,
	MEMORY_FORMFACTOR_SODIMM = 0x0d,
	MEMORY_FORMFACTOR_SRIMM = 0x0e,
	MEMORY_FORMFACTOR_FBDIMM = 0x0f,
	MEMORY_FORMFACTOR_DIE = 0x10,
} smbios_memory_form_factor;

typedef enum {
	MEMORY_TYPE_OTHER = 0x01,
	MEMORY_TYPE_UNKNOWN = 0x02,
	MEMORY_TYPE_DRAM = 0x03,
	MEMORY_TYPE_EDRAM = 0x04,
	MEMORY_TYPE_VRAM = 0x05,
	MEMORY_TYPE_SRAM = 0x06,
	MEMORY_TYPE_RAM = 0x07,
	MEMORY_TYPE_ROM = 0x08,
	MEMORY_TYPE_FLASH = 0x09,
	MEMORY_TYPE_EEPROM = 0x0a,
	MEMORY_TYPE_FEPROM = 0x0b,
	MEMORY_TYPE_EPROM = 0x0c,
	MEMORY_TYPE_CDRAM = 0x0d,
	MEMORY_TYPE_3DRAM = 0x0e,
	MEMORY_TYPE_SDRAM = 0x0f,
	MEMORY_TYPE_SGRAM = 0x10,
	MEMORY_TYPE_RDRAM = 0x11,
	MEMORY_TYPE_DDR = 0x12,
	MEMORY_TYPE_DDR2 = 0x13,
	MEMORY_TYPE_DDR2_FBDIMM = 0x14,
	MEMORY_TYPE_DDR3 = 0x18,
	MEMORY_TYPE_FBD2 = 0x19,
	MEMORY_TYPE_DDR4 = 0x1a,
	MEMORY_TYPE_LPDDR = 0x1b,
	MEMORY_TYPE_LPDDR2 = 0x1c,
	MEMORY_TYPE_LPDDR3 = 0x1d,
	MEMORY_TYPE_LPDDR4 = 0x1e,
	MEMORY_TYPE_LOGICAL_NON_VOLATILE_DEVICE = 0x1f,
	MEMORY_TYPE_HBM = 0x20,
	MEMORY_TYPE_HBM2 = 0x21,
	MEMORY_TYPE_DDR5 = 0x22,
	MEMORY_TYPE_LPDDR5 = 0x23,
} smbios_memory_type;

typedef enum {
	MEMORY_ARRAY_LOCATION_OTHER = 0x01,
	MEMORY_ARRAY_LOCATION_UNKNOWN = 0x02,
	MEMORY_ARRAY_LOCATION_SYSTEM_BOARD = 0x03,
	MEMORY_ARRAY_LOCATION_ISA_ADD_ON = 0x04,
	MEMORY_ARRAY_LOCATION_EISA_ADD_ON = 0x05,
	MEMORY_ARRAY_LOCATION_PCI_ADD_ON = 0x06,
	MEMORY_ARRAY_LOCATION_MCA_ADD_ON = 0x07,
	MEMORY_ARRAY_LOCATION_PCMCIA_ADD_ON = 0x08,
	MEMORY_ARRAY_LOCATION_PROPRIETARY_ADD_ON = 0x09,
	MEMORY_ARRAY_LOCATION_NUBUS = 0x0a,
	MEMORY_ARRAY_LOCATION_PC_98_C20_ADD_ON = 0xa0,
	MEMORY_ARRAY_LOCATION_PC_98_C24_ADD_ON = 0xa1,
	MEMORY_ARRAY_LOCATION_PC_98_E_ADD_ON = 0xa2,
	MEMORY_ARRAY_LOCATION_PC_98_LOCAL_BUS_ADD_ON = 0xa3,
	MEMORY_ARRAY_LOCATION_CXL_FLEXBUS_1_0_ADD_ON = 0xa4,
} smbios_memory_array_location;

typedef enum {
	MEMORY_ARRAY_USE_OTHER = 0x01,
	MEMORY_ARRAY_USE_UNKNOWN = 0x02,
	MEMORY_ARRAY_USE_SYSTEM = 0x03,
	MEMORY_ARRAY_USE_VIDEO = 0x04,
	MEMORY_ARRAY_USE_FLASH = 0x05,
	MEMORY_ARRAY_USE_NVRAM = 0x06,
	MEMORY_ARRAY_USE_CACHE = 0x07,
} smbios_memory_array_use;

typedef enum {
	MEMORY_ARRAY_ECC_OTHER = 0x01,
	MEMORY_ARRAY_ECC_UNKNOWN = 0x02,
	MEMORY_ARRAY_ECC_NONE = 0x03,
	MEMORY_ARRAY_ECC_PARITY = 0x04,
	MEMORY_ARRAY_ECC_SINGLE_BIT = 0x05,
	MEMORY_ARRAY_ECC_MULTI_BIT = 0x06,
	MEMORY_ARRAY_ECC_CRC = 0x07,
} smbios_memory_array_ecc;

#define SMBIOS_STATE_SAFE 3
typedef enum {
	SMBIOS_BIOS_INFORMATION = 0,
	SMBIOS_SYSTEM_INFORMATION = 1,
	SMBIOS_BOARD_INFORMATION = 2,
	SMBIOS_SYSTEM_ENCLOSURE = 3,
	SMBIOS_PROCESSOR_INFORMATION = 4,
	SMBIOS_CACHE_INFORMATION = 7,
	SMBIOS_PORT_CONNECTOR_INFORMATION = 8,
	SMBIOS_SYSTEM_SLOTS = 9,
	SMBIOS_OEM_STRINGS = 11,
	SMBIOS_EVENT_LOG = 15,
	SMBIOS_PHYS_MEMORY_ARRAY = 16,
	SMBIOS_MEMORY_DEVICE = 17,
	SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS = 19,
	SMBIOS_MEMORY_DEVICE_MAPPED_ADDRESS = 20,
	SMBIOS_TEMPERATURE_PROBE = 28,
	SMBIOS_SYSTEM_BOOT_INFORMATION = 32,
	SMBIOS_IPMI_DEVICE_INFORMATION = 38,
	SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION = 41,
	SMBIOS_TPM_DEVICE = 43,
	SMBIOS_END_OF_TABLE = 127,
} smbios_struct_type_t;

struct smbios_entry {
	u8 anchor[4];
	u8 checksum;
	u8 length;
	u8 major_version;
	u8 minor_version;
	u16 max_struct_size;
	u8 entry_point_rev;
	u8 formwatted_area[5];
	u8 intermediate_anchor_string[5];
	u8 intermediate_checksum;
	u16 struct_table_length;
	u32 struct_table_address;
	u16 struct_count;
	u8 smbios_bcd_revision;
} __packed;

struct smbios_entry30 {
	u8 anchor[5];
	u8 checksum;
	u8 length;
	u8 major_version;
	u8 minor_version;
	u8 smbios_doc_rev;
	u8 entry_point_rev;
	u8 reserved;
	u32 struct_table_length;
	u64 struct_table_address;
} __packed;

struct smbios_header {
	u8 type;
	u8 length;
	u16 handle;
} __packed;

struct smbios_type0 {
	struct smbios_header header;
	u8 vendor;
	u8 bios_version;
	u16 bios_start_segment;
	u8 bios_release_date;
	u8 bios_rom_size;
	u64 bios_characteristics;
	u8 bios_characteristics_ext1;
	u8 bios_characteristics_ext2;
	u8 system_bios_major_release;
	u8 system_bios_minor_release;
	u8 ec_major_release;
	u8 ec_minor_release;
	u16 extended_bios_rom_size;
	u8 eos[2];
} __packed;

typedef enum {
	SMBIOS_WAKEUP_TYPE_RESERVED = 0x00,
	SMBIOS_WAKEUP_TYPE_OTHER = 0x01,
	SMBIOS_WAKEUP_TYPE_UNKNOWN = 0x02,
	SMBIOS_WAKEUP_TYPE_APM_TIMER = 0x03,
	SMBIOS_WAKEUP_TYPE_MODEM_RING = 0x04,
	SMBIOS_WAKEUP_TYPE_LAN_REMOTE = 0x05,
	SMBIOS_WAKEUP_TYPE_POWER_SWITCH = 0x06,
	SMBIOS_WAKEUP_TYPE_PCI_PME = 0x07,
	SMBIOS_WAKEUP_TYPE_AC_POWER_RESTORED = 0x08,
} smbios_wakeup_type;

struct smbios_type1 {
	struct smbios_header header;
	u8 manufacturer;
	u8 product_name;
	u8 version;
	u8 serial_number;
	u8 uuid[16];
	u8 wakeup_type;
	u8 sku;
	u8 family;
	u8 eos[2];
} __packed;

#define SMBIOS_FEATURE_FLAGS_HOSTING_BOARD		(1 << 0)
#define SMBIOS_FEATURE_FLAGS_REQUIRES_DAUGHTER_CARD	(1 << 1)
#define SMBIOS_FEATURE_FLAGS_REMOVABLE			(1 << 2)
#define SMBIOS_FEATURE_FLAGS_REPLACEABLE		(1 << 3)
#define SMBIOS_FEATURE_FLAGS_HOT_SWAPPABLE		(1 << 4)

typedef enum {
	SMBIOS_BOARD_TYPE_UNKNOWN = 0x01,
	SMBIOS_BOARD_TYPE_OTHER = 0x02,
	SMBIOS_BOARD_TYPE_SERVER_BLADE = 0x03,
	SMBIOS_BOARD_TYPE_CONNECTIVITY_SWITCH = 0x04,
	SMBIOS_BOARD_TYPE_SYSTEM_MANAGEMENT_MODULE = 0x05,
	SMBIOS_BOARD_TYPE_PROCESSOR_MODULE = 0x06,
	SMBIOS_BOARD_TYPE_IO_MODULE = 0x07,
	SMBIOS_BOARD_TYPE_MEMORY_MODULE = 0x08,
	SMBIOS_BOARD_TYPE_DAUGHTER_BOARD = 0x09,
	SMBIOS_BOARD_TYPE_MOTHERBOARD = 0x0a,
	SMBIOS_BOARD_TYPE_PROCESSOR_MEMORY_MODULE = 0x0b,
	SMBIOS_BOARD_TYPE_PROCESSOR_IO_MODULE = 0x0c,
	SMBIOS_BOARD_TYPE_INTERCONNECT_BOARD = 0x0d,
} smbios_board_type;

struct smbios_type2 {
	struct smbios_header header;
	u8 manufacturer;
	u8 product_name;
	u8 version;
	u8 serial_number;
	u8 asset_tag;
	u8 feature_flags;
	u8 location_in_chassis;
	u16 chassis_handle;
	u8 board_type;
	u8 eos[2];
} __packed;

typedef enum {
	SMBIOS_ENCLOSURE_OTHER = 0x01,
	SMBIOS_ENCLOSURE_UNKNOWN = 0x02,
	SMBIOS_ENCLOSURE_DESKTOP = 0x03,
	SMBIOS_ENCLOSURE_LOW_PROFILE_DESKTOP = 0x04,
	SMBIOS_ENCLOSURE_PIZZA_BOX = 0x05,
	SMBIOS_ENCLOSURE_MINI_TOWER = 0x06,
	SMBIOS_ENCLOSURE_TOWER = 0x07,
	SMBIOS_ENCLOSURE_PORTABLE = 0x08,
	SMBIOS_ENCLOSURE_LAPTOP = 0x09,
	SMBIOS_ENCLOSURE_NOTEBOOK = 0x0a,
	SMBIOS_ENCLOSURE_HAND_HELD = 0x0b,
	SMBIOS_ENCLOSURE_DOCKING_STATION = 0x0c,
	SMBIOS_ENCLOSURE_ALL_IN_ONE = 0x0d,
	SMBIOS_ENCLOSURE_SUB_NOTEBOOK = 0x0e,
	SMBIOS_ENCLOSURE_SPACE_SAVING = 0x0f,
	SMBIOS_ENCLOSURE_LUNCH_BOX = 0x10,
	SMBIOS_ENCLOSURE_MAIN_SERVER_CHASSIS = 0x11,
	SMBIOS_ENCLOSURE_EXPANSION_CHASSIS = 0x12,
	SMBIOS_ENCLOSURE_SUBCHASSIS = 0x13,
	SMBIOS_ENCLOSURE_BUS_EXPANSION_CHASSIS = 0x14,
	SMBIOS_ENCLOSURE_PERIPHERAL_CHASSIS = 0x15,
	SMBIOS_ENCLOSURE_RAID_CHASSIS = 0x16,
	SMBIOS_ENCLOSURE_RACK_MOUNT_CHASSIS = 0x17,
	SMBIOS_ENCLOSURE_SEALED_CASE_PC = 0x18,
	SMBIOS_ENCLOSURE_MULTI_SYSTEM_CHASSIS = 0x19,
	SMBIOS_ENCLOSURE_COMPACT_PCI = 0x1a,
	SMBIOS_ENCLOSURE_ADVANCED_TCA = 0x1b,
	SMBIOS_ENCLOSURE_BLADE = 0x1c,
	SMBIOS_ENCLOSURE_BLADE_ENCLOSURE = 0x1d,
	SMBIOS_ENCLOSURE_TABLET = 0x1e,
	SMBIOS_ENCLOSURE_CONVERTIBLE = 0x1f,
	SMBIOS_ENCLOSURE_DETACHABLE = 0x20,
	SMBIOS_ENCLOSURE_IOT_GATEWAY = 0x21,
	SMBIOS_ENCLOSURE_EMBEDDED_PC = 0x22,
	SMBIOS_ENCLOSURE_MINI_PC = 0x23,
	SMBIOS_ENCLOSURE_STICK_PC = 0x24,
} smbios_enclosure_type;

struct smbios_type3 {
	struct smbios_header header;
	u8 manufacturer;
	u8 _type;
	u8 version;
	u8 serial_number;
	u8 asset_tag_number;
	u8 bootup_state;
	u8 power_supply_state;
	u8 thermal_state;
	u8 security_status;
	u32 oem_defined;
	u8 height;
	u8 number_of_power_cords;
	u8 element_count;
	u8 element_record_length;
	u8 sku_number;
	u8 eos[2];
} __packed;

struct smbios_type4 {
	struct smbios_header header;
	u8 socket_designation;
	u8 processor_type;
	u8 processor_family;
	u8 processor_manufacturer;
	u32 processor_id[2];
	u8 processor_version;
	u8 voltage;
	u16 external_clock;
	u16 max_speed;
	u16 current_speed;
	u8 status;
	u8 processor_upgrade;
	u16 l1_cache_handle;
	u16 l2_cache_handle;
	u16 l3_cache_handle;
	u8 serial_number;
	u8 asset_tag;
	u8 part_number;
	u8 core_count;
	u8 core_enabled;
	u8 thread_count;
	u16 processor_characteristics;
	u16 processor_family2;
	u16 core_count2;
	u16 core_enabled2;
	u16 thread_count2;
	u8 eos[2];
} __packed;

/* defines for smbios_type4 */

#define SMBIOS_PROCESSOR_STATUS_POPULATED		(1 << 6)
#define SMBIOS_PROCESSOR_STATUS_CPU_ENABLED		(1 << 0)

/* defines for supported_sram_type/current_sram_type */

#define SMBIOS_CACHE_SRAM_TYPE_OTHER			(1 << 0)
#define SMBIOS_CACHE_SRAM_TYPE_UNKNOWN			(1 << 1)
#define SMBIOS_CACHE_SRAM_TYPE_NON_BURST		(1 << 2)
#define SMBIOS_CACHE_SRAM_TYPE_BURST			(1 << 3)
#define SMBIOS_CACHE_SRAM_TYPE_PIPELINE_BURST		(1 << 4)
#define SMBIOS_CACHE_SRAM_TYPE_SYNCHRONOUS		(1 << 5)
#define SMBIOS_CACHE_SRAM_TYPE_ASYNCHRONOUS		(1 << 6)

/* enum for error_correction_type */

enum smbios_cache_error_corr {
	SMBIOS_CACHE_ERROR_CORRECTION_OTHER = 1,
	SMBIOS_CACHE_ERROR_CORRECTION_UNKNOWN,
	SMBIOS_CACHE_ERROR_CORRECTION_NONE,
	SMBIOS_CACHE_ERROR_CORRECTION_PARITY,
	SMBIOS_CACHE_ERROR_CORRECTION_SINGLE_BIT,
	SMBIOS_CACHE_ERROR_CORRECTION_MULTI_BIT,
};

/* enum for system_cache_type */

enum smbios_cache_type {
	SMBIOS_CACHE_TYPE_OTHER = 1,
	SMBIOS_CACHE_TYPE_UNKNOWN,
	SMBIOS_CACHE_TYPE_INSTRUCTION,
	SMBIOS_CACHE_TYPE_DATA,
	SMBIOS_CACHE_TYPE_UNIFIED,
};

/* enum for associativity */

enum smbios_cache_associativity {
	SMBIOS_CACHE_ASSOCIATIVITY_OTHER = 1,
	SMBIOS_CACHE_ASSOCIATIVITY_UNKNOWN,
	SMBIOS_CACHE_ASSOCIATIVITY_DIRECT,
	SMBIOS_CACHE_ASSOCIATIVITY_2WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_4WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_FULL,
	SMBIOS_CACHE_ASSOCIATIVITY_8WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_16WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_12WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_24WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_32WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_48WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_64WAY,
	SMBIOS_CACHE_ASSOCIATIVITY_20WAY,
};

/* defines for cache_configuration */

#define SMBIOS_CACHE_CONF_LEVEL(x) ((((x) - 1) & 0x7) << 0)
#define SMBIOS_CACHE_CONF_LOCATION(x) (((x) & 0x3) << 5)
#define SMBIOS_CACHE_CONF_ENABLED(x) (((x) & 0x1) << 7)
#define SMBIOS_CACHE_CONF_OPERATION_MODE(x) (((x) & 0x3) << 8)

/* defines for max_cache_size and installed_size */

#define SMBIOS_CACHE_SIZE_UNIT_1KB		(0 << 15)
#define SMBIOS_CACHE_SIZE_UNIT_64KB		(1 << 15)
#define SMBIOS_CACHE_SIZE_MASK			0x7fff
#define SMBIOS_CACHE_SIZE_OVERFLOW		0xffff

#define SMBIOS_CACHE_SIZE2_UNIT_1KB		(0 << 31)
#define SMBIOS_CACHE_SIZE2_UNIT_64KB		(1UL << 31)
#define SMBIOS_CACHE_SIZE2_MASK			0x7fffffff

/* define for cache operation mode */

#define SMBIOS_CACHE_OP_MODE_WRITE_THROUGH 0
#define SMBIOS_CACHE_OP_MODE_WRITE_BACK 1
#define SMBIOS_CACHE_OP_MODE_VARIES_WITH_MEMORY_ADDRESS 2
#define SMBIOS_CACHE_OP_MODE_UNKNOWN 3

struct smbios_type7 {
	struct smbios_header header;
	u8 socket_designation;
	u16 cache_configuration;
	u16 max_cache_size;
	u16 installed_size;
	u16 supported_sram_type;
	u16 current_sram_type;
	u8 cache_speed;
	u8 error_correction_type;
	u8 system_cache_type;
	u8 associativity;
	u32 max_cache_size2;
	u32 installed_size2;
	u8 eos[2];
} __packed;

/* enum for connector types */
typedef enum {
	CONN_NONE = 0x00,
	CONN_CENTRONICS = 0x01,
	CONN_MINI_CENTRONICS = 0x02,
	CONN_PROPRIETARY = 0x03,
	CONN_DB_25_PIN_MALE = 0x04,
	CONN_DB_25_PIN_FEMALE = 0x05,
	CONN_DB_15_PIN_MALE = 0x06,
	CONN_DB_15_PIN_FEMALE = 0x07,
	CONN_DB_9_PIN_MALE = 0x08,
	CONN_DB_9_PIN_FEMALE = 0x09,
	CONN_RJ_11 = 0x0A,
	CONN_RJ_45 = 0x0B,
	CONN_50_PIN_MINI_SCSI = 0x0C,
	CONN_MINI_DIN = 0x0D,
	CONN_MICRO_DIN = 0x0E,
	CONN_PS_2 = 0x0F,
	CONN_INFRARED = 0x10,
	CONN_HP_HIL = 0x11,
	CONN_ACCESS_BUS_USB = 0x12,
	CONN_SSA_SCSI = 0x13,
	CONN_CIRCULAR_DIN_8_MALE = 0x14,
	CONN_CIRCULAR_DIN_8_FEMALE = 0x15,
	CONN_ON_BOARD_IDE = 0x16,
	CONN_ON_BOARD_FLOPPY = 0x17,
	CONN_9_PIN_DUAL_INLINE = 0x18,
	CONN_25_PIN_DUAL_INLINE = 0x19,
	CONN_50_PIN_DUAL_INLINE = 0x1A,
	CONN_68_PIN_DUAL_INLINE = 0x1B,
	CONN_ON_BOARD_SOUND_INPUT_FROM_CD_ROM = 0x1C,
	CONN_MINI_CENTRONICS_TYPE14 = 0x1D,
	CONN_MINI_CENTRONICS_TYPE26 = 0x1E,
	CONN_MINI_JACK_HEADPHONES = 0x1F,
	CONN_BNC = 0x20,
	CONN_1394 = 0x21,
	CONN_SAS_SATA = 0x22,
	CONN_USB_TYPE_C = 0x23,
	CONN_PC_98 = 0xA0,
	CONN_PC_98_HIRESO = 0xA1,
	CONN_PC_H98 = 0xA2,
	CONN_PC98_NOTE = 0xA3,
	CONN_PC_98_FULL = 0xA4,
	CONN_OTHER = 0xFF,
} type8_connector_types;

/* enum for port types */
typedef enum {
	TYPE_NONE_PORT = 0x00,
	TYPE_PARALLEL_PORT_XT_AT_COMPATIBLE = 0x01,
	TYPE_PARALLEL_PORT_PS_2 = 0x02,
	TYPE_PARALLEL_PORT_ECP = 0x03,
	TYPE_PARALLEL_PORT_EPP = 0x04,
	TYPE_PARALLEL_PORT_ECP_EPP = 0x05,
	TYPE_SERIAL_PORT_XT_AT_COMPATIBLE = 0x06,
	TYPE_SERIAL_PORT_16450_COMPATIBLE = 0x07,
	TYPE_SERIAL_PORT_16550_COMPATIBLE = 0x08,
	TYPE_SERIAL_PORT_16550A_COMPATIBLE = 0x09,
	TYPE_SCSI_PORT = 0x0A,
	TYPE_MIDI_PORT = 0x0B,
	TYPE_JOY_STICK_PORT = 0x0C,
	TYPE_KEYBOARD_PORT = 0x0D,
	TYPE_MOUSE_PORT = 0x0E,
	TYPE_SSA_SCSI = 0x0F,
	TYPE_USB = 0x10,
	TYPE_FIREWIRE_IEEE_P1394 = 0x11,
	TYPE_PCMCIA_TYPE_I = 0x12,
	TYPE_PCMCIA_TYPE_II = 0x13,
	TYPE_PCMCIA_TYPE_III = 0x14,
	TYPE_CARDBUS = 0x15,
	TYPE_ACCESS_BUS_PORT = 0x16,
	TYPE_SCSI_II = 0x17,
	TYPE_SCSI_WIDE = 0x18,
	TYPE_PC_98 = 0x19,
	TYPE_PC_98_HIRESO = 0x1A,
	TYPE_PC_H98 = 0x1B,
	TYPE_VIDEO_PORT = 0x1C,
	TYPE_AUDIO_PORT = 0x1D,
	TYPE_MODEM_PORT = 0x1E,
	TYPE_NETWORK_PORT = 0x1F,
	TYPE_SATA = 0x20,
	TYPE_SAS = 0x21,
	TYPE_MFDP = 0x22,
	TYPE_THUNDERBOLT = 0x23,
	TYPE_8251_COMPATIBLE = 0xA0,
	TYPE_8251_FIFO_COMPATIBLE = 0xA1,
	TYPE_OTHER_PORT = 0xFF,
} type8_port_types;

struct port_information {
	const char *internal_reference_designator;
	type8_connector_types internal_connector_type;
	const char *external_reference_designator;
	type8_connector_types external_connector_type;
	type8_port_types port_type;
};

struct smbios_type8 {
	struct smbios_header header;
	u8 internal_reference_designator;
	u8 internal_connector_type;
	u8 external_reference_designator;
	u8 external_connector_type;
	u8 port_type;
	u8 eos[2];
} __packed;

/* System Slots - Slot Type */
enum misc_slot_type {
	SlotTypeOther = 0x01,
	SlotTypeUnknown = 0x02,
	SlotTypeIsa = 0x03,
	SlotTypeMca = 0x04,
	SlotTypeEisa = 0x05,
	SlotTypePci = 0x06,
	SlotTypePcmcia = 0x07,
	SlotTypeVlVesa = 0x08,
	SlotTypeProprietary = 0x09,
	SlotTypeProcessorCardSlot = 0x0A,
	SlotTypeProprietaryMemoryCardSlot = 0x0B,
	SlotTypeIORiserCardSlot = 0x0C,
	SlotTypeNuBus = 0x0D,
	SlotTypePci66MhzCapable = 0x0E,
	SlotTypeAgp = 0x0F,
	SlotTypeApg2X = 0x10,
	SlotTypeAgp4X = 0x11,
	SlotTypePciX = 0x12,
	SlotTypeAgp8X = 0x13,
	SlotTypeM2Socket1_DP = 0x14,
	SlotTypeM2Socket1_SD = 0x15,
	SlotTypeM2Socket2 = 0x16,
	SlotTypeM2Socket3 = 0x17,
	SlotTypeMxmTypeI = 0x18,
	SlotTypeMxmTypeII = 0x19,
	SlotTypeMxmTypeIIIStandard = 0x1A,
	SlotTypeMxmTypeIIIHe = 0x1B,
	SlotTypeMxmTypeIV = 0x1C,
	SlotTypeMxm30TypeA = 0x1D,
	SlotTypeMxm30TypeB = 0x1E,
	SlotTypePciExpressGen2Sff_8639 = 0x1F,
	SlotTypePciExpressGen3Sff_8639 = 0x20,
	SlotTypePciExpressMini52pinWithBSKO = 0x21,
	SlotTypePciExpressMini52pinWithoutBSKO = 0x22,
	SlotTypePciExpressMini76pin = 0x23,
	SlotTypePC98C20 = 0xA0,
	SlotTypePC98C24 = 0xA1,
	SlotTypePC98E = 0xA2,
	SlotTypePC98LocalBus = 0xA3,
	SlotTypePC98Card = 0xA4,
	SlotTypePciExpress = 0xA5,
	SlotTypePciExpressX1 = 0xA6,
	SlotTypePciExpressX2 = 0xA7,
	SlotTypePciExpressX4 = 0xA8,
	SlotTypePciExpressX8 = 0xA9,
	SlotTypePciExpressX16 = 0xAA,
	SlotTypePciExpressGen2 = 0xAB,
	SlotTypePciExpressGen2X1 = 0xAC,
	SlotTypePciExpressGen2X2 = 0xAD,
	SlotTypePciExpressGen2X4 = 0xAE,
	SlotTypePciExpressGen2X8 = 0xAF,
	SlotTypePciExpressGen2X16 = 0xB0,
	SlotTypePciExpressGen3 = 0xB1,
	SlotTypePciExpressGen3X1 = 0xB2,
	SlotTypePciExpressGen3X2 = 0xB3,
	SlotTypePciExpressGen3X4 = 0xB4,
	SlotTypePciExpressGen3X8 = 0xB5,
	SlotTypePciExpressGen3X16 = 0xB6,
	SlotTypePciExpressGen4 = 0xB8,
	SlotTypePciExpressGen4x1 = 0xB9,
	SlotTypePciExpressGen4x2 = 0xBA,
	SlotTypePciExpressGen4x4 = 0xBB,
	SlotTypePciExpressGen4x8 = 0xBC,
	SlotTypePciExpressGen4x16 = 0xBD,
	SlotTypePciExpressGen5 = 0xBE,
	SlotTypePciExpressGen5x1 = 0xBF,
	SlotTypePciExpressGen5x2 = 0xC0,
	SlotTypePciExpressGen5x4 = 0xC1,
	SlotTypePciExpressGen5x8 = 0xC2,
	SlotTypePciExpressGen5x16 = 0xC3
};

/* System Slots - Slot Data Bus Width. */
enum slot_data_bus_bandwidth {
	SlotDataBusWidthOther = 0x01,
	SlotDataBusWidthUnknown = 0x02,
	SlotDataBusWidth8Bit = 0x03,
	SlotDataBusWidth16Bit = 0x04,
	SlotDataBusWidth32Bit = 0x05,
	SlotDataBusWidth64Bit = 0x06,
	SlotDataBusWidth128Bit = 0x07,
	SlotDataBusWidth1X = 0x08,
	SlotDataBusWidth2X = 0x09,
	SlotDataBusWidth4X = 0x0A,
	SlotDataBusWidth8X = 0x0B,
	SlotDataBusWidth12X = 0x0C,
	SlotDataBusWidth16X = 0x0D,
	SlotDataBusWidth32X = 0x0E
};

/* System Slots - Current Usage. */
enum misc_slot_usage {
	SlotUsageOther        = 0x01,
	SlotUsageUnknown      = 0x02,
	SlotUsageAvailable    = 0x03,
	SlotUsageInUse        = 0x04,
	SlotUsageUnavailable  = 0x05
};

/* System Slots - Slot Length.*/
enum misc_slot_length {
	SlotLengthOther = 0x01,
	SlotLengthUnknown = 0x02,
	SlotLengthShort = 0x03,
	SlotLengthLong = 0x04
};

/* System Slots - Slot Characteristics 1. */
#define SMBIOS_SLOT_UNKNOWN		(1 << 0)
#define SMBIOS_SLOT_5V			(1 << 1)
#define SMBIOS_SLOT_3P3V		(1 << 2)
#define SMBIOS_SLOT_SHARED		(1 << 3)
#define SMBIOS_SLOT_PCCARD_16		(1 << 4)
#define SMBIOS_SLOT_PCCARD_CARDBUS	(1 << 5)
#define SMBIOS_SLOT_PCCARD_ZOOM		(1 << 6)
#define SMBIOS_SLOT_PCCARD_MODEM_RING	(1 << 7)
/* System Slots - Slot Characteristics 2. */
#define SMBIOS_SLOT_PME		(1 << 0)
#define SMBIOS_SLOT_HOTPLUG	(1 << 1)
#define SMBIOS_SLOT_SMBUS	(1 << 2)
#define SMBIOS_SLOT_BIFURCATION	(1 << 3)

struct slot_peer_groups {
	u16 peer_seg_num;
	u8 peer_bus_num;
	u8 peer_dev_fn_num;
	u8 peer_data_bus_width;
} __packed;

struct smbios_type9 {
	struct smbios_header header;
	u8 slot_designation;
	u8 slot_type;
	u8 slot_data_bus_width;
	u8 current_usage;
	u8 slot_length;
	u16 slot_id;
	u8 slot_characteristics_1;
	u8 slot_characteristics_2;
	u16 segment_group_number;
	u8 bus_number;
	u8 device_function_number;
	u8 data_bus_width;
	u8 peer_group_count;
	struct slot_peer_groups peer[0];
	u8 eos[2];
} __packed;

struct smbios_type11 {
	struct smbios_header header;
	u8 count;
	u8 eos[2];
} __packed;

struct smbios_type15 {
	struct smbios_header header;
	u16 area_length;
	u16 header_offset;
	u16 data_offset;
	u8 access_method;
	u8 log_status;
	u32 change_token;
	u32 address;
	u8 header_format;
	u8 log_type_descriptors;
	u8 log_type_descriptor_length;
	u8 eos[2];
} __packed;

enum {
	SMBIOS_EVENTLOG_ACCESS_METHOD_IO8 = 0,
	SMBIOS_EVENTLOG_ACCESS_METHOD_IO8X2,
	SMBIOS_EVENTLOG_ACCESS_METHOD_IO16,
	SMBIOS_EVENTLOG_ACCESS_METHOD_MMIO32,
	SMBIOS_EVENTLOG_ACCESS_METHOD_GPNV,
};

enum {
	SMBIOS_EVENTLOG_STATUS_VALID = 1, /* Bit 0 */
	SMBIOS_EVENTLOG_STATUS_FULL  = 2, /* Bit 1 */
};

#define SMBIOS_USE_EXTENDED_MAX_CAPACITY	(1ULL << 31)

struct smbios_type16 {
	struct smbios_header header;
	u8 location;
	u8 use;
	u8 memory_error_correction;
	u32 maximum_capacity;
	u16 memory_error_information_handle;
	u16 number_of_memory_devices;
	u64 extended_maximum_capacity;
	u8 eos[2];
} __packed;

struct smbios_type17 {
	struct smbios_header header;
	u16 phys_memory_array_handle;
	u16 memory_error_information_handle;
	u16 total_width;
	u16 data_width;
	u16 size;
	u8 form_factor;
	u8 device_set;
	u8 device_locator;
	u8 bank_locator;
	u8 memory_type;
	u16 type_detail;
	u16 speed;
	u8 manufacturer;
	u8 serial_number;
	u8 asset_tag;
	u8 part_number;
	u8 attributes;
	u32 extended_size;
	u16 clock_speed;
	u16 minimum_voltage;
	u16 maximum_voltage;
	u16 configured_voltage;
	u8 eos[2];
} __packed;

struct smbios_type19 {
	struct smbios_header header;
	u32 starting_address;
	u32 ending_address;
	u16 memory_array_handle;
	u8 partition_width;
	u64 extended_starting_address;
	u64 extended_ending_address;
	u8 eos[2];
} __packed;

struct smbios_type20 {
	struct smbios_header header;
	u32 addr_start;
	u32 addr_end;
	u16 memory_device_handle;
	u16 memory_array_mapped_address_handle;
	u8 partition_row_pos;
	u8 interleave_pos;
	u8 interleave_depth;
	u64 ext_addr_start;
	u64 ext_addr_end;
	u8 eos[2];
} __packed;

/* Bit[7..5] = Temp status */
enum smbios_temp_status {
	SMBIOS_TEMP_STATUS_OTHER = 0x01,
	SMBIOS_TEMP_STATUS_UNKNOWN,
	SMBIOS_TEMP_STATUS_OK,
	SMBIOS_TEMP_STATUS_NONCRITICAL,
	SMBIOS_TEMP_STATUS_CRITICAL,
	SMBIOS_TEMP_STATUS_NONREC,			// Non-Recoverable.
};

/* Bit[4..0] = Temp location */
enum smbios_temp_location {
	SMBIOS_TEMP_LOCATION_OTHER = 0x01,
	SMBIOS_TEMP_LOCATION_UNKNOWN,
	SMBIOS_TEMP_LOCATION_PROCESSOR,
	SMBIOS_TEMP_LOCATION_DISK,
	SMBIOS_TEMP_LOCATION_BAY,			// Peripheral Bay.
	SMBIOS_TEMP_LOCATION_SMM,			// System Management Module.
	SMBIOS_TEMP_LOCATION_BOARD,			// Motherboard.
	SMBIOS_TEMP_LOCATION_MM,			// Memory.
	SMBIOS_TEMP_LOCATION_PM,			// Processor Module.
	SMBIOS_TEMP_LOCATION_POW,			// Power Unit.
	SMBIOS_TEMP_LOCATION_ADDCARD,
};

struct smbios_type28 {
	struct smbios_header header;
	u8 description;
	u8 location_and_status;
	u16 maximum_value;
	u16 minimum_value;
	u16 resolution;
	u16 tolerance;
	u16 accuracy;
	u32 oem_defined;
	u16 nominal_value;
	u8 eos[2];
} __packed;


struct smbios_type32 {
	struct smbios_header header;
	u8 reserved[6];
	u8 boot_status;
	u8 eos[2];
} __packed;

struct smbios_type38 {
	struct smbios_header header;
	u8 interface_type;
	u8 ipmi_rev;
	u8 i2c_slave_addr;
	u8 nv_storage_addr;
	u64 base_address;
	u8 base_address_modifier;
	u8 irq;
	u8 eos[2];
} __packed;

enum smbios_bmc_interface_type {
	SMBIOS_BMC_INTERFACE_UNKNOWN = 0,
	SMBIOS_BMC_INTERFACE_KCS,
	SMBIOS_BMC_INTERFACE_SMIC,
	SMBIOS_BMC_INTERFACE_BLOCK,
	SMBIOS_BMC_INTERFACE_SMBUS,
};

typedef enum {
	SMBIOS_DEVICE_TYPE_OTHER = 0x01,
	SMBIOS_DEVICE_TYPE_UNKNOWN,
	SMBIOS_DEVICE_TYPE_VIDEO,
	SMBIOS_DEVICE_TYPE_SCSI,
	SMBIOS_DEVICE_TYPE_ETHERNET,
	SMBIOS_DEVICE_TYPE_TOKEN_RING,
	SMBIOS_DEVICE_TYPE_SOUND,
	SMBIOS_DEVICE_TYPE_PATA,
	SMBIOS_DEVICE_TYPE_SATA,
	SMBIOS_DEVICE_TYPE_SAS,
	SMBIOS_DEVICE_TYPE_WIRELESS_LAN,
	SMBIOS_DEVICE_TYPE_BLUETOOTH,
	SMBIOS_DEVICE_TYPE_WWAN,
	SMBIOS_DEVICE_TYPE_EMMC,
	SMBIOS_DEVICE_TYPE_NVME,
	SMBIOS_DEVICE_TYPE_UFS,
} smbios_onboard_device_type;

#define SMBIOS_DEVICE_TYPE_COUNT 10

struct smbios_type41 {
	struct smbios_header header;
	u8 reference_designation;
	u8 device_type: 7;
	u8 device_status: 1;
	u8 device_type_instance;
	u16 segment_group_number;
	u8 bus_number;
	u8 function_number: 3;
	u8 device_number: 5;
	u8 eos[2];
} __packed;


#define SMBIOS_TPM_DEVICE_CHARACTERISTICS_NOT_SUPPORTED (1ULL << 2)
#define SMBIOS_TPM_DEVICE_FAMILY_CONFIGURABLE_VIA_FW_UPD (1ULL << 3)
#define SMBIOS_TPM_DEVICE_FAMILY_CONFIGURABLE_VIA_PLATFORM_SW_SUPPORT (1ULL << 4)
#define SMBIOS_TPM_DEVICE_FAMILY_CONFIGURABLE_VIA_OEM_PROPRIETARY (1ULL << 5)

struct smbios_type43 {
	struct smbios_header header;
	u32 vendor_id;
	u8 major_spec_ver;
	u8 minor_spec_ver;
	u32 fw_ver1;
	u32 fw_ver2;
	u8 description;
	u64 characteristics;
	u32 oem_defined;
	u8 eos[2];
} __packed;

struct smbios_type127 {
	struct smbios_header header;
	u8 eos[2];
} __packed;

void smbios_fill_dimm_manufacturer_from_id(uint16_t mod_id,
	struct smbios_type17 *t);
void smbios_fill_dimm_asset_tag(const struct dimm_info *dimm,
	struct smbios_type17 *t);
void smbios_fill_dimm_locator(const struct dimm_info *dimm,
	struct smbios_type17 *t);

smbios_wakeup_type smbios_system_wakeup_type(void);
smbios_board_type smbios_mainboard_board_type(void);
smbios_enclosure_type smbios_mainboard_enclosure_type(void);

#endif
