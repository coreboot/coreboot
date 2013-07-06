#ifndef SMBIOS_H
#define SMBIOS_H

#include <types.h>

int smbios_write_type11(unsigned long *current, int handle, const char **oem_strings, int count);
unsigned long smbios_write_tables(unsigned long start);
int smbios_add_string(char *start, const char *str);
int smbios_string_table_len(char *start);

const char *smbios_mainboard_serial_number(void);
const char *smbios_mainboard_version(void);

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

#define SMBIOS_STATE_SAFE 3
typedef enum {
	SMBIOS_BIOS_INFORMATION=0,
	SMBIOS_SYSTEM_INFORMATION=1,
	SMBIOS_SYSTEM_ENCLOSURE=3,
	SMBIOS_PROCESSOR_INFORMATION=4,
	SMBIOS_CACHE_INFORMATION=7,
	SMBIOS_SYSTEM_SLOTS=9,
	SMBIOS_OEM_STRINGS=11,
	SMBIOS_EVENT_LOG=15,
	SMBIOS_PHYS_MEMORY_ARRAY=16,
	SMBIOS_MEMORY_DEVICE=17,
	SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS=19,
	SMBIOS_SYSTEM_BOOT_INFORMATION=32,
	SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION=41,
	SMBIOS_END_OF_TABLE=127,
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
} __attribute__((packed));

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
	char eos[2];
} __attribute__((packed));

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
	char eos[2];
} __attribute__((packed));

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
	char eos[2];
} __attribute__((packed));

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
	char eos[2];
} __attribute__((packed));

struct smbios_type11 {
	u8 type;
	u8 length;
	u16 handle;
	u8 count;
	char eos[2];
} __attribute__((packed));

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
	char eos[2];
} __attribute__((packed));

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
	char eos[2];
} __attribute__((packed));

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
	u16 extended_size;
	u16 clock_speed;

	char eos[2];
} __attribute__((packed));

struct smbios_type32 {
	u8 type;
	u8 length;
	u16 handle;
	u8 reserved[6];
	u8 boot_status;
	u8 eos[2];
} __attribute__((packed));

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
} __attribute__((packed));

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
	char eos[2];
} __attribute__((packed));

struct smbios_type127 {
	u8 type;
	u8 length;
	u16 handle;
	u8 eos[2];
} __attribute__((packed));

#endif
