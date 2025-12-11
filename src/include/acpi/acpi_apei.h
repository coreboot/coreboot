/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Contains ACPI Platform Error Interfaces (APEI) definitions and declarations.
 * These are mostly used by:
 * - BERT (Boot Error Record Table) ACPI Table.
 * - HEST (Hardware Error Source Table) ACPI Table.
 * - EINJ (Error Injection Table)
 */

#ifndef _ACPI_APEI_H_
#define _ACPI_APEI_H_

#include <acpi/acpi.h>

#define MAX_ERROR_BLOCK_SIZE      0x1000
#define MAX_MCA_ERROR_BLOCK_SIZE  0x0400

// Hardware Error Notification types. All other values are reserved
#define HARDWARE_ERROR_NOTIFICATION_POLLED              0
#define HARDWARE_ERROR_NOTIFICATION_EXTERNAL_INTERRUPT  1
#define HARDWARE_ERROR_NOTIFICATION_LOCAL_INTERRUPT     2
#define HARDWARE_ERROR_NOTIFICATION_SCI                 3
#define HARDWARE_ERROR_NOTIFICATION_NMI                 4
#define HARDWARE_ERROR_NOTIFICATION_CMCI                5
#define HARDWARE_ERROR_NOTIFICATION_MCE                 6
#define HARDWARE_ERROR_NOTIFICATION_GPIO                7

/* HEST (Hardware Error Source Table) */
typedef struct acpi_hest {
	acpi_header_t header;
	u32 error_source_count;
	/* error_source_struct(s) */
} __packed acpi_hest_t;

/* Error Source Descriptors */
typedef struct acpi_hest_esd {
	u16 type;
	u16 source_id;
	u16 resv;
	u8 flags;
	u8 enabled;
	u32 prealloc_erecords;		/* The number of error records to
					 * pre-allocate for this error source.
					 */
	u32 max_section_per_record;
} __packed acpi_hest_esd_t;
#define ACPI_APEI_ESD_TYPE_MCE          0 // Machine Check Exception
#define ACPI_APEI_ESD_TYPE_CMC          1 // Corrected Machine Check
#define ACPI_APEI_ESD_TYPE_NMI          2 // Non Maskable Interrupt
#define ACPI_APEI_ESD_TYPE_RPORT_AER    6 // PCIe Root Port AER
#define ACPI_APEI_ESD_TYPE_DEVICE_AER   7 // PCIe Device/Endpoint AER
#define ACPI_APEI_ESD_TYPE_BRIDGE_AER   8 // PCIe Bridge AER
#define ACPI_APEI_ESD_TYPE_GHE          9 // Generic Hardware Error
#define ACPI_APEI_ESD_TYPE_GHE_V2      10 // Generic Hardware Error
#define ACPI_APEI_ESD_TYPE_DMC         11 // Deferred Machine Check

#define ERROR_SOURCE_FLAG_FIRMWARE_FIRST  BIT(0)
#define ERROR_SOURCE_FLAG_GLOBAL          BIT(1)
#define ERROR_SOURCE_FLAG_GHES_ASSIST     BIT(2)

/* Hardware Error Notification */
typedef struct acpi_hest_hen {
	u8 type;
	u8 length;
	u16 conf_we;		/* Configuration Write Enable */
	u32 poll_interval;
	u32 vector;
	u32 sw2poll_threshold_val;
	u32 sw2poll_threshold_win;
	u32 error_threshold_val;
	u32 error_threshold_win;
} __packed acpi_hest_hen_t;

/* Generic Hardware Error Source Descriptor */
typedef struct acpi_ghes_esd {
	u16 type;
	u16 source_id;
	u16 related_src_id;
	u8 flags;
	u8 enabled;
	u32 prealloc_erecords;
	u32 max_section_per_record;
} __packed acpi_ghes_esd_t;

typedef struct ghes_record {
	acpi_ghes_esd_t esd;
	u32 max_raw_data_length;
	acpi_addr64_t sts_addr;
	acpi_hest_hen_t notify;
	u32 err_sts_blk_len;
} __packed ghes_record_t;

/* BERT (Boot Error Record Table) */
typedef struct acpi_bert {
	acpi_header_t header;
	u32 region_length;
	u64 error_region;
} __packed acpi_bert_t;

/* Generic Error Data Entry */
typedef struct acpi_hest_generic_data {
	guid_t section_type;
	u32 error_severity;
	u16 revision;
	u8 validation_bits;
	u8 flags;
	u32 data_length;
	guid_t fru_id;
	u8 fru_text[20];
	/* error data */
} __packed acpi_hest_generic_data_t;

/* Generic Error Data Entry v300 */
typedef struct acpi_hest_generic_data_v300 {
	guid_t section_type;
	u32 error_severity;
	u16 revision;
	u8 validation_bits;
	u8 flags;		/* see CPER Section Descriptor, Flags field */
	u32 data_length;
	guid_t fru_id;
	u8 fru_text[20];
	cper_timestamp_t timestamp;
	/* error data */
} __packed acpi_hest_generic_data_v300_t;
#define HEST_GENERIC_ENTRY_V300			0x300

/* Both Generic Error Status & Generic Error Data Entry, Error Severity field */
#define ACPI_GENERROR_SEV_RECOVERABLE		0
#define ACPI_GENERROR_SEV_FATAL			1
#define ACPI_GENERROR_SEV_CORRECTED		2
#define ACPI_GENERROR_SEV_NONE			3

/* Generic Error Data Entry, Validation Bits field */
#define ACPI_GENERROR_VALID_FRUID		BIT(0)
#define ACPI_GENERROR_VALID_FRUID_TEXT		BIT(1)
#define ACPI_GENERROR_VALID_TIMESTAMP		BIT(2)

/*
 * Generic Error Status Block
 *
 * If there is a raw data section at the end of the generic error status block after the
 * zero or more generic error data entries, raw_data_length indicates the length of the raw
 * section and raw_data_offset is the offset of the beginning of the raw data section from
 * the start of the acpi_generic_error_status block it is contained in. So if raw_data_length
 * is non-zero, raw_data_offset must be at least sizeof(acpi_generic_error_status_t).
 */
typedef struct acpi_generic_error_status {
	u32 block_status;
	u32 raw_data_offset;	/* must follow any generic entries */
	u32 raw_data_length;
	u32 data_length;	/* generic data */
	u32 error_severity;
	/* Generic Error Data structures, zero or more entries */
} __packed acpi_generic_error_status_t;

/* Generic Status Block, Block Status values */
#define GENERIC_ERR_STS_UNCORRECTABLE_VALID	BIT(0)
#define GENERIC_ERR_STS_CORRECTABLE_VALID	BIT(1)
#define GENERIC_ERR_STS_MULT_UNCORRECTABLE	BIT(2)
#define GENERIC_ERR_STS_MULT_CORRECTABLE	BIT(3)
#define GENERIC_ERR_STS_ENTRY_COUNT_SHIFT	4
#define GENERIC_ERR_STS_ENTRY_COUNT_MAX		0x3ff
#define GENERIC_ERR_STS_ENTRY_COUNT_MASK	\
					(GENERIC_ERR_STS_ENTRY_COUNT_MAX \
					<< GENERIC_ERR_STS_ENTRY_COUNT_SHIFT)

/* chipsets that select ACPI_BERT must implement this function */
enum cb_err acpi_soc_get_bert_region(void **region, size_t *length);

uintptr_t acpi_soc_fill_hest(acpi_hest_t *hest, uintptr_t current, void *log_mem);
unsigned long acpi_create_hest_error_source(acpi_hest_t *hest, acpi_hest_esd_t *esd, u16 type,
					    void *data, u16 len);

/* EINJ APEI Standard Definitions */
/* EINJ Error Types
   Refer to the ACPI spec, EINJ section, for more info on bit definitions
*/
#define ACPI_EINJ_CPU_CE		(1 << 0)
#define ACPI_EINJ_CPU_UCE		(1 << 1)
#define ACPI_EINJ_CPU_UCE_FATAL		(1 << 2)
#define ACPI_EINJ_MEM_CE		(1 << 3)
#define ACPI_EINJ_MEM_UCE		(1 << 4)
#define ACPI_EINJ_MEM_UCE_FATAL		(1 << 5)
#define ACPI_EINJ_PCIE_CE		(1 << 6)
#define ACPI_EINJ_PCIE_UCE_NON_FATAL	(1 << 7)
#define ACPI_EINJ_PCIE_UCE_FATAL	(1 << 8)
#define ACPI_EINJ_PLATFORM_CE		(1 << 9)
#define ACPI_EINJ_PLATFORM_UCE		(1 << 10)
#define ACPI_EINJ_PLATFORM_UCE_FATAL	(1 << 11)
#define ACPI_EINJ_VENDOR_DEFINED	(1 << 31)
#define ACPI_EINJ_DEFAULT_CAP		(ACPI_EINJ_MEM_CE | ACPI_EINJ_MEM_UCE | \
					ACPI_EINJ_PCIE_CE | ACPI_EINJ_PCIE_UCE_FATAL)

/* EINJ actions */
#define ACTION_COUNT			9
#define BEGIN_INJECT_OP			0x00
#define GET_TRIGGER_ACTION_TABLE	0x01
#define SET_ERROR_TYPE			0x02
#define GET_ERROR_TYPE			0x03
#define END_INJECT_OP			0x04
#define EXECUTE_INJECT_OP		0x05
#define CHECK_BUSY_STATUS		0x06
#define GET_CMD_STATUS			0x07
#define SET_ERROR_TYPE_WITH_ADDRESS	0x08
#define TRIGGER_ERROR			0xFF

/* EINJ Instructions */
#define READ_REGISTER			0x00
#define READ_REGISTER_VALUE		0x01
#define WRITE_REGISTER			0x02
#define WRITE_REGISTER_VALUE		0x03
#define NO_OP				0x04

/* EINJ (Error Injection Table) */

/* Instruction entry */
typedef struct acpi_einj_action_table {
	u8 action;
	u8 instruction;
	u16 flags;
	acpi_addr64_t reg;
	u64 value;
	u64 mask;
} __packed acpi_einj_action_table_t;

typedef struct acpi_injection_header {
	u32 einj_header_size;
	u32 flags;
	u32 entry_count;
} __packed acpi_injection_header_t;

typedef struct acpi_einj_trigger_table {
	u32 header_size;
	u32 revision;
	u32 table_size;
	u32 entry_count;
	acpi_einj_action_table_t trigger_action[];
} __packed acpi_einj_trigger_table_t;

typedef struct set_error_type {
	u32 errtype;
	u32 vendorerrortype;
	u32 flags;
	u32 apicid;
	u64 memaddr;
	u64 memrange;
	u32 pciesbdf;
} __packed set_error_type_t;

#define EINJ_PARAM_NUM 6
typedef struct acpi_einj_smi {
	u64 op_state;
	u64 err_inject[EINJ_PARAM_NUM];
	u64 trigger_action_table;
	u64 err_inj_cap;
	u64 op_status;
	u64 cmd_sts;
	u64 einj_addr;
	u64 einj_addr_msk;
	set_error_type_t setaddrtable;
	u64 reserved[50];
} __packed acpi_einj_smi_t;

/* EINJ Flags */
#define EINJ_DEF_TRIGGER_PORT	0xb2
#define FLAG_PRESERVE		0x01
#define FLAG_IGNORE		0x00

/* EINJ Registers */
#define EINJ_REG_MEMORY(address) \
	{ \
	.space_id = ACPI_ADDRESS_SPACE_MEMORY, \
	.bit_width = 64, \
	.bit_offset = 0, \
	.access_size = ACPI_ACCESS_SIZE_QWORD_ACCESS, \
	.addr = address}

#define EINJ_REG_IO() \
	{ \
	.space_id = ACPI_ADDRESS_SPACE_IO, \
	.bit_width = 0x10, \
	.bit_offset = 0, \
	.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS, \
	.addr = EINJ_DEF_TRIGGER_PORT} /* HW dependent code can override this also */

typedef struct acpi_einj {
	acpi_header_t header;
	acpi_injection_header_t inj_header;
	acpi_einj_action_table_t action_table[ACTION_COUNT];
} __packed acpi_einj_t;

void acpi_create_einj(acpi_einj_t *einj, uintptr_t addr, u8 actions);

#endif
