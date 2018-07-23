/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>,
 * Raptor Engineering
 * Copyright (C) various authors, the coreboot project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SMBIOS_H
#define SMBIOS_H

#include <types.h>
#include <compiler.h>

unsigned long smbios_write_tables(unsigned long start);
int smbios_add_string(u8 *start, const char *str);
int smbios_string_table_len(u8 *start);

/* Used by mainboard to add an on-board device */
enum smbios_bmc_interface_type;
int smbios_write_type38(unsigned long *current, int *handle,
			const enum smbios_bmc_interface_type interface_type,
			const u8 ipmi_rev, const u8 i2c_addr, const u8 nv_addr,
			const u64 base_addr, const u8 base_modifier,
			const u8 irq);
int smbios_write_type41(unsigned long *current, int *handle,
			const char *name, u8 instance, u16 segment,
			u8 bus, u8 device, u8 function);

const char *smbios_mainboard_manufacturer(void);
const char *smbios_mainboard_product_name(void);

const char *smbios_mainboard_serial_number(void);
const char *smbios_mainboard_version(void);
void smbios_mainboard_set_uuid(u8 *uuid);
const char *smbios_mainboard_bios_version(void);
const char *smbios_mainboard_asset_tag(void);
u8 smbios_mainboard_feature_flags(void);
const char *smbios_mainboard_location_in_chassis(void);

const char *smbios_mainboard_sku(void);
u8 smbios_mainboard_enclosure_type(void);
#ifdef CONFIG_MAINBOARD_FAMILY
const char *smbios_mainboard_family(void);
#endif

#define BIOS_CHARACTERISTICS_PCI_SUPPORTED  (1 << 7)
#define BIOS_CHARACTERISTICS_PC_CARD  (1 << 8)
#define BIOS_CHARACTERISTICS_PNP  (1 << 9)
#define BIOS_CHARACTERISTICS_APM (1 << 10)
#define BIOS_CHARACTERISTICS_UPGRADEABLE      (1 << 11)
#define BIOS_CHARACTERISTICS_SHADOW           (1 << 12)
#define BIOS_CHARACTERISTICS_BOOT_FROM_CD     (1 << 15)
#define BIOS_CHARACTERISTICS_SELECTABLE_BOOT  (1 << 16)
#define BIOS_CHARACTERISTICS_BIOS_SOCKETED    (1 << 17)

#define BIOS_EXT1_CHARACTERISTICS_ACPI    (1 << 0)
#define BIOS_EXT2_CHARACTERISTICS_TARGET  (1 << 2)

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
	MEMORY_DEVICE_OTHER = 0x01,
	MEMORY_DEVICE_UNKNOWN = 0x02,
	MEMORY_DEVICE_DRAM = 0x03,
	MEMORY_DEVICE_EDRAM = 0x04,
	MEMORY_DEVICE_VRAM = 0x05,
	MEMORY_DEVICE_SRAM = 0x06,
	MEMORY_DEVICE_RAM = 0x07,
	MEMORY_DEVICE_ROM = 0x08,
	MEMORY_DEVICE_FLASH = 0x09,
	MEMORY_DEVICE_EEPROM = 0x0A,
	MEMORY_DEVICE_FEPROM = 0x0B,
	MEMORY_DEVICE_EPROM = 0x0C,
	MEMORY_DEVICE_CDRAM = 0x0D,
	MEMORY_DEVICE_3DRAM = 0x0E,
	MEMORY_DEVICE_SDRAM = 0x0F,
	MEMORY_DEVICE_SGRAM = 0x10,
	MEMORY_DEVICE_RDRAM = 0x11,
	MEMORY_DEVICE_DDR = 0x12,
	MEMORY_DEVICE_DDR2 = 0x13,
	MEMORY_DEVICE_DDR2_FB_DIMM = 0x14,
	MEMORY_DEVICE_DDR3 = 0x18,
	MEMORY_DEVICE_DBD2 = 0x19,
	MEMORY_DEVICE_DDR4 = 0x1A,
	MEMORY_DEVICE_LPDDR = 0x1B,
	MEMORY_DEVICE_LPDDR2 = 0x1C,
	MEMORY_DEVICE_LPDDR3 = 0x1D,
	MEMORY_DEVICE_LPDDR4 = 0x1E,
} smbios_memory_device_type;

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
	SMBIOS_SYSTEM_SLOTS = 9,
	SMBIOS_OEM_STRINGS = 11,
	SMBIOS_EVENT_LOG = 15,
	SMBIOS_PHYS_MEMORY_ARRAY = 16,
	SMBIOS_MEMORY_DEVICE = 17,
	SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS = 19,
	SMBIOS_SYSTEM_BOOT_INFORMATION = 32,
	SMBIOS_IPMI_DEVICE_INFORMATION = 38,
	SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION = 41,
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

struct smbios_type0 {
	u8 type;
	u8 length;
	u16 handle;
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
	u8 eos[2];
} __packed;

struct smbios_type1 {
	u8 type;
	u8 length;
	u16 handle;
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
	u8 type;
	u8 length;
	u16 handle;
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

enum {
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
};

struct smbios_type3 {
	u8 type;
	u8 length;
	u16 handle;
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
	u8 type;
	u8 length;
	u16 handle;
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
	u8 eos[2];
} __packed;

struct smbios_type11 {
	u8 type;
	u8 length;
	u16 handle;
	u8 count;
	u8 eos[2];
} __packed;

struct smbios_type15 {
	u8 type;
	u8 length;
	u16 handle;
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

struct smbios_type16 {
	u8 type;
	u8 length;
	u16 handle;
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
	u8 type;
	u8 length;
	u16 handle;
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

struct smbios_type32 {
	u8 type;
	u8 length;
	u16 handle;
	u8 reserved[6];
	u8 boot_status;
	u8 eos[2];
} __packed;

struct smbios_type38 {
	u8 type;
	u8 length;
	u16 handle;
	u8 interface_type;
	u8 ipmi_rev;
	u8 i2c_slave_addr;
	u8 nv_storage_addr;
	u64 base_address;
	u8 base_address_modifier;
	u8 irq;
} __packed;

enum smbios_bmc_interface_type {
	SMBIOS_BMC_INTERFACE_UNKNOWN = 0,
	SMBIOS_BMC_INTERFACE_KCS,
	SMBIOS_BMC_INTERFACE_SMIC,
	SMBIOS_BMC_INTERFACE_BLOCK,
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
} smbios_onboard_device_type;

struct smbios_type41 {
	u8 type;
	u8 length;
	u16 handle;
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

struct smbios_type127 {
	u8 type;
	u8 length;
	u16 handle;
	u8 eos[2];
} __packed;

void smbios_fill_dimm_manufacturer_from_id(uint16_t mod_id,
	struct smbios_type17 *t);

smbios_board_type smbios_mainboard_board_type(void);

#endif
