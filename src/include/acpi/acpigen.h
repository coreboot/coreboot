/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_ACPIGEN_H__
#define __ACPI_ACPIGEN_H__

#include <stddef.h>
#include <stdint.h>
#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_pld.h>
#include <device/pci_type.h>

/* Values that can be returned for ACPI Device _STA method */
#define ACPI_STATUS_DEVICE_PRESENT	(1 << 0)
#define ACPI_STATUS_DEVICE_ENABLED	(1 << 1)
#define ACPI_STATUS_DEVICE_SHOW_IN_UI	(1 << 2)
#define ACPI_STATUS_DEVICE_STATE_OK	(1 << 3)

#define ACPI_STATUS_DEVICE_ALL_OFF	0
#define ACPI_STATUS_DEVICE_ALL_ON	(ACPI_STATUS_DEVICE_PRESENT |\
					 ACPI_STATUS_DEVICE_ENABLED |\
					 ACPI_STATUS_DEVICE_SHOW_IN_UI |\
					 ACPI_STATUS_DEVICE_STATE_OK)
#define ACPI_STATUS_DEVICE_HIDDEN_ON	(ACPI_STATUS_DEVICE_PRESENT |\
					 ACPI_STATUS_DEVICE_ENABLED |\
					 ACPI_STATUS_DEVICE_STATE_OK)

/* ACPI Op/Prefix Codes */
enum {
	ZERO_OP			= 0x00,
	ONE_OP			= 0x01,
	ALIAS_OP		= 0x06,
	NAME_OP			= 0x08,
	BYTE_PREFIX		= 0x0A,
	WORD_PREFIX		= 0x0B,
	DWORD_PREFIX		= 0x0C,
	STRING_PREFIX		= 0x0D,
	QWORD_PREFIX		= 0x0E,
	SCOPE_OP		= 0x10,
	BUFFER_OP		= 0x11,
	PACKAGE_OP		= 0x12,
	VARIABLE_PACKAGE_OP	= 0x13,
	METHOD_OP		= 0x14,
	EXTERNAL_OP		= 0x15,
	DUAL_NAME_PREFIX	= 0x2E,
	MULTI_NAME_PREFIX	= 0x2F,
	EXT_OP_PREFIX		= 0x5B,
	 MUTEX_OP		= 0x01,
	 EVENT_OP		= 0x01,
	 SF_RIGHT_OP		= 0x10,
	 SF_LEFT_OP		= 0x11,
	 COND_REFOF_OP		= 0x12,
	 CREATEFIELD_OP		= 0x13,
	 LOAD_TABLE_OP		= 0x1f,
	 LOAD_OP		= 0x20,
	 STALL_OP		= 0x21,
	 SLEEP_OP		= 0x22,
	 ACQUIRE_OP		= 0x23,
	 SIGNAL_OP		= 0x24,
	 WAIT_OP		= 0x25,
	 RST_OP			= 0x26,
	 RELEASE_OP		= 0x27,
	 FROM_BCD_OP		= 0x28,
	 TO_BCD_OP		= 0x29,
	 UNLOAD_OP		= 0x2A,
	 REVISON_OP		= 0x30,
	 DEBUG_OP		= 0x31,
	 FATAL_OP		= 0x32,
	 TIMER_OP		= 0x33,
	 OPREGION_OP		= 0x80,
	 FIELD_OP		= 0x81,
	 DEVICE_OP		= 0x82,
	 PROCESSOR_OP		= 0x83,
	 POWER_RES_OP		= 0x84,
	 THERMAL_ZONE_OP	= 0x85,
	 INDEX_FIELD_OP		= 0x86,
	 BANK_FIELD_OP		= 0x87,
	 DATA_REGION_OP		= 0x88,
	ROOT_PREFIX		= 0x5C,
	PARENT_PREFIX		= 0x5E,
	LOCAL0_OP		= 0x60,
	LOCAL1_OP		= 0x61,
	LOCAL2_OP		= 0x62,
	LOCAL3_OP		= 0x63,
	LOCAL4_OP		= 0x64,
	LOCAL5_OP		= 0x65,
	LOCAL6_OP		= 0x66,
	LOCAL7_OP		= 0x67,
	ARG0_OP			= 0x68,
	ARG1_OP			= 0x69,
	ARG2_OP			= 0x6A,
	ARG3_OP			= 0x6B,
	ARG4_OP			= 0x6C,
	ARG5_OP			= 0x6D,
	ARG6_OP			= 0x6E,
	STORE_OP		= 0x70,
	REF_OF_OP		= 0x71,
	ADD_OP			= 0x72,
	CONCATENATE_OP		= 0x73,
	SUBTRACT_OP		= 0x74,
	INCREMENT_OP		= 0x75,
	DECREMENT_OP		= 0x76,
	MULTIPLY_OP		= 0x77,
	DIVIDE_OP		= 0x78,
	SHIFT_LEFT_OP		= 0x79,
	SHIFT_RIGHT_OP		= 0x7A,
	AND_OP			= 0x7B,
	NAND_OP			= 0x7C,
	OR_OP			= 0x7D,
	NOR_OP			= 0x7E,
	XOR_OP			= 0x7F,
	NOT_OP			= 0x80,
	FD_SHIFT_LEFT_BIT_OR	= 0x81,
	FD_SHIFT_RIGHT_BIT_OR	= 0x82,
	DEREF_OP		= 0x83,
	CONCATENATE_TEMP_OP	= 0x84,
	MOD_OP			= 0x85,
	NOTIFY_OP		= 0x86,
	SIZEOF_OP		= 0x87,
	INDEX_OP		= 0x88,
	MATCH_OP		= 0x89,
	CREATE_DWORD_OP		= 0x8A,
	CREATE_WORD_OP		= 0x8B,
	CREATE_BYTE_OP		= 0x8C,
	CREATE_BIT_OP		= 0x8D,
	OBJ_TYPE_OP		= 0x8E,
	CREATE_QWORD_OP		= 0x8F,
	LAND_OP			= 0x90,
	LOR_OP			= 0x91,
	LNOT_OP			= 0x92,
	LEQUAL_OP		= 0x93,
	LGREATER_OP		= 0x94,
	LLESS_OP		= 0x95,
	TO_BUFFER_OP		= 0x96,
	TO_DEC_STRING_OP	= 0x97,
	TO_HEX_STRING_OP	= 0x98,
	TO_INTEGER_OP		= 0x99,
	TO_STRING_OP		= 0x9C,
	CP_OBJ_OP		= 0x9D,
	MID_OP			= 0x9E,
	CONTINUE_OP		= 0x9F,
	IF_OP			= 0xA0,
	ELSE_OP			= 0xA1,
	WHILE_OP		= 0xA2,
	NOOP_OP			= 0xA3,
	RETURN_OP		= 0xA4,
	BREAK_OP		= 0xA5,
	COMMENT_OP		= 0xA9,
	BREAKPIONT_OP		= 0xCC,
	ONES_OP			= 0xFF,
};

#define FIELDLIST_OFFSET(X)		{ .type = OFFSET, \
					  .name = "",\
					  .bits = X * 8, \
					}
#define FIELDLIST_NAMESTR(X, Y)		{ .type = NAME_STRING, \
					  .name = X, \
					  .bits = Y, \
					}
#define FIELDLIST_RESERVED(X)		{ .type = RESERVED, \
					  .name = "", \
					  .bits = X, \
					}

#define FIELD_ANYACC			0
#define FIELD_BYTEACC			1
#define FIELD_WORDACC			2
#define FIELD_DWORDACC			3
#define FIELD_QWORDACC			4
#define FIELD_BUFFERACC			5
#define FIELD_NOLOCK			(0<<4)
#define FIELD_LOCK			(1<<4)
#define FIELD_PRESERVE			(0<<5)
#define FIELD_WRITEASONES		(1<<5)
#define FIELD_WRITEASZEROS		(2<<5)

enum field_type {
	OFFSET,
	NAME_STRING,
	RESERVED,
	FIELD_TYPE_MAX,
};

struct fieldlist {
	enum field_type type;
	const char *name;
	u32 bits;
};

#define OPREGION(rname, space, offset, len)	{.name = rname, \
						 .regionspace = space, \
						 .regionoffset = offset, \
						 .regionlen = len, \
						}

enum region_space {
	SYSTEMMEMORY,
	SYSTEMIO,
	PCI_CONFIG,
	EMBEDDEDCONTROL,
	SMBUS,
	CMOS,
	PCIBARTARGET,
	IPMI,
	GPIO_REGION,
	GPSERIALBUS,
	PCC,
	FIXED_HARDWARE = 0x7F,
	REGION_SPACE_MAX,
};

enum acpi_resource_type {
	RSRC_TYPE_MEM = 0,
	RSRC_TYPE_IO = 1,
	RSRC_TYPE_BUS = 2
};

enum acpi_decode_type {
	DECODE_10,
	DECODE_16
};

enum acpi_read_write_type {
	READ_ONLY,
	READ_WRITE
};

enum acpi_cacheable_type {
	NON_CACHEABLE,
	CACHEABLE
};

enum acpi_resource_subtype {
	DWORD_IO,
	DWORD_MEMORY,
	IO,
	MEMORY_32_FIXED,
	QWORD_MEMORY,
	WORD_BUS_NUMBER
};

/* macros for ACPI Table 6.49 Memory Resource Flag (Resource Type = 0) */
#define MEM_RSRC_FLAG_TRNSL_TYPE_STATIC		0x0
#define MEM_RSRC_FLAG_TRNSL_TYPE_TRANSLATION	(0x1 << 5)
#define MEM_RSRC_FLAG_ADDR_RNG_MEM		0x0
#define MEM_RSRC_FLAG_ADDR_RNG_RSV		(0x1 << 3)
#define MEM_RSRC_FLAG_ADDR_RNG_ACPI		(0x2 << 3)
#define MEM_RSRC_FLAG_ADDR_RNG_NVA		(0x3 << 3)
#define MEM_RSRC_FLAG_MEM_ATTR_NON_CACHE	0x0
#define MEM_RSRC_FLAG_MEM_ATTR_CACHE		(0x1 << 1)
#define MEM_RSRC_FLAG_MEM_ATTR_CACHE_WRT	(0x2 << 1)
#define MEM_RSRC_FLAG_MEM_ATTR_CACHE_PREFETCH	(0x3 << 1)
#define MEM_RSRC_FLAG_MEM_READ_ONLY		0x0
#define MEM_RSRC_FLAG_MEM_READ_WRITE		0x1

/* macros for ACPI Table 6.50 I/O Resource Flag (Resource Type = 1) */
#define IO_RSRC_FLAG_ENTIRE_RANGE		0x3
#define IO_RSRC_FLAG_ISA_ONLY			0x2
#define IO_RSRC_FLAG_NON_ISA_ONLY		0x1
#define IO_RSRC_FLAG_TRASL_TYPE_STATIC		0x0
#define IO_RSRC_FLAG_TRNSL_TYPE_TRANSLATION	(0x1 << 4)
#define IO_RSRC_FLAG_SPRS_TRASL_DENSE		0x0
#define IO_RSRC_FLAG_SPRS_TRNSL_SPARSE		(0x1 << 5)

/* macro for ACPI Table 6.51 Bus Number Range Resource Flag (Resource Type = 2) */
#define BUS_NUM_RANGE_RESOURCE_FLAG		0x0 // reserved

/* General Flags for WORD Address Space Descriptor Definition  (ACPI Table 6.47, byte 4)*/
#define ADDR_SPACE_GENERAL_FLAG_MAX_FIXED	(0x1 << 3)
#define ADDR_SPACE_GENERAL_FLAG_MAX_NOT_FIXED	0x0
#define ADDR_SPACE_GENERAL_FLAG_MIN_FIXED	(0x1 << 2)
#define ADDR_SPACE_GENERAL_FLAG_MIN_NOT_FIXED	0x0
#define ADDR_SPACE_GENERAL_FLAG_DEC_SUB		(0x1 << 1)
#define ADDR_SPACE_GENERAL_FLAG_DEC_POS		0x0
#define ADDR_SPACE_GENERAL_FLAG_CONSUMER	0x10
#define ADDR_SPACE_GENERAL_FLAG_PRODUCER	0x0

struct opregion {
	const char *name;
	enum region_space regionspace;
	unsigned long regionoffset;
	unsigned long regionlen;
};

#define DSM_UUID(DSM_UUID, DSM_CALLBACKS, DSM_COUNT, DSM_ARG) \
	{ .uuid = DSM_UUID, \
	.callbacks = DSM_CALLBACKS, \
	.count = DSM_COUNT, \
	.arg = DSM_ARG, \
	}

struct dsm_uuid {
	const char *uuid;
	void (**callbacks)(void *);
	size_t count;
	void *arg;
};

#define CPPC_VERSION_1	1
#define CPPC_VERSION_2	2
#define CPPC_VERSION_3	3

#define CPPC_PACKAGE_NAME "GCPC"

/*version 1 has 15 fields, version 2 has 19, and version 3 has 21 */
enum cppc_fields {
	CPPC_HIGHEST_PERF, /* can be DWORD */
	CPPC_NOMINAL_PERF, /* can be DWORD */
	CPPC_LOWEST_NONL_PERF, /* can be DWORD */
	CPPC_LOWEST_PERF, /* can be DWORD */
	CPPC_GUARANTEED_PERF,
	CPPC_DESIRED_PERF,
	CPPC_MIN_PERF,
	CPPC_MAX_PERF,
	CPPC_PERF_REDUCE_TOLERANCE,
	CPPC_TIME_WINDOW,
	CPPC_COUNTER_WRAP, /* can be DWORD */
	CPPC_REF_PERF_COUNTER,
	CPPC_DELIVERED_PERF_COUNTER,
	CPPC_PERF_LIMITED,
	CPPC_ENABLE, /* can be System I/O */
	CPPC_MAX_FIELDS_VER_1,
	CPPC_AUTO_SELECT = /* can be DWORD */
		CPPC_MAX_FIELDS_VER_1,
	CPPC_AUTO_ACTIVITY_WINDOW,
	CPPC_PERF_PREF,
	CPPC_REF_PERF, /* can be DWORD */
	CPPC_MAX_FIELDS_VER_2,
	CPPC_LOWEST_FREQ = /* can be DWORD */
		CPPC_MAX_FIELDS_VER_2,
	CPPC_NOMINAL_FREQ, /* can be DWORD */
	CPPC_MAX_FIELDS_VER_3,
};

typedef struct cppc_entry {
	enum { CPPC_TYPE_REG, CPPC_TYPE_DWORD } type;
	union {
		acpi_addr_t reg;
		uint32_t dword;
	};
} cppc_entry_t;

#define CPPC_DWORD(_dword) \
	(cppc_entry_t){ \
		.type  = CPPC_TYPE_DWORD, \
		.dword = _dword, \
	}

#define CPPC_REG(_reg) \
	(cppc_entry_t){ \
		.type = CPPC_TYPE_REG, \
		.reg  = _reg, \
	}

#define CPPC_REG_MSR(address, offset, width) CPPC_REG(ACPI_REG_MSR(address, offset, width))
#define CPPC_UNSUPPORTED CPPC_REG(ACPI_REG_UNSUPPORTED)

struct cppc_config {
	u32 version; /* must be 1, 2, or 3 */
	/*
	 * The generic acpi_addr_t structure is being used, though
	 * anything besides PPC or FFIXED generally requires checking
	 * if the OS has advertised support for it (via _OSC).
	 */
	cppc_entry_t entries[CPPC_MAX_FIELDS_VER_3];
};

#define ACPI_MUTEX_NO_TIMEOUT		0xffff

void acpigen_write_return_integer(uint64_t arg);
void acpigen_write_return_namestr(const char *arg);
void acpigen_write_return_string(const char *arg);
void acpigen_write_len_f(void);
void acpigen_pop_len(void);
void acpigen_set_current(char *curr);
char *acpigen_get_current(void);
char *acpigen_write_package(int nr_el);
__always_inline void acpigen_write_package_end(void)
{
	acpigen_pop_len();
}
void acpigen_write_zero(void);
void acpigen_write_one(void);
void acpigen_write_ones(void);
void acpigen_write_byte(unsigned int data);
void acpigen_emit_byte(unsigned char data);
void acpigen_emit_ext_op(uint8_t op);
void acpigen_emit_word(unsigned int data);
void acpigen_emit_dword(unsigned int data);
void acpigen_emit_stream(const char *data, int size);
void acpigen_emit_string(const char *string);
void acpigen_emit_namestring(const char *namepath);
void acpigen_emit_eisaid(const char *eisaid);
void acpigen_write_word(unsigned int data);
void acpigen_write_dword(unsigned int data);
void acpigen_write_qword(uint64_t data);
void acpigen_write_integer(uint64_t data);
void acpigen_write_string(const char *string);
void acpigen_write_name_unicode(const char *name, const char *string);
void acpigen_write_name(const char *name);
void acpigen_write_name_string(const char *name, const char *string);
void acpigen_write_name_dword(const char *name, uint32_t val);
void acpigen_write_name_qword(const char *name, uint64_t val);
void acpigen_write_name_byte(const char *name, uint8_t val);
void acpigen_write_name_integer(const char *name, uint64_t val);
void acpigen_write_coreboot_hid(enum coreboot_acpi_ids id);
void acpigen_write_scope(const char *name);
__always_inline void acpigen_write_scope_end(void)
{
	acpigen_pop_len();
}
void acpigen_write_method(const char *name, int nargs);
void acpigen_write_method_serialized(const char *name, int nargs);
__always_inline void acpigen_write_method_end(void)
{
	acpigen_pop_len();
}
void acpigen_write_device(const char *name);
__always_inline void acpigen_write_device_end(void)
{
	acpigen_pop_len();
}
void acpigen_write_thermal_zone(const char *name);
__always_inline void acpigen_write_thermal_zone_end(void)
{
	acpigen_pop_len();
}
void acpigen_write_LPI_package(u64 level, const struct acpi_lpi_state *states, u16 nentries);
void acpigen_write_PPC(u8 nr);
void acpigen_write_PPC_NVS(void);
void acpigen_write_empty_PCT(void);
void acpigen_write_empty_PTC(void);
void acpigen_write_PRW(u32 wake, u32 level);
void acpigen_write_STA(uint8_t status);
void acpigen_write_STA_ext(const char *namestring);
void acpigen_write_TPC(const char *gnvs_tpc_limit);
void acpigen_write_PSS_package(u32 coreFreq, u32 power, u32 transLat,
			u32 busmLat, u32 control, u32 status);
void acpigen_write_pss_object(const struct acpi_sw_pstate *pstate_values, size_t nentries);
typedef enum { SW_ALL = 0xfc, SW_ANY = 0xfd, HW_ALL = 0xfe } PSD_coord;
void acpigen_write_PSD_package(u32 domain, u32 numprocs, PSD_coord coordtype);
void acpigen_write_CST_package_entry(const acpi_cstate_t *cstate);
void acpigen_write_CST_package(const acpi_cstate_t *entry, int nentries);
typedef enum { CSD_HW_ALL = 0xfe } CSD_coord;
void acpigen_write_CSD_package(u32 domain, u32 numprocs, CSD_coord coordtype,
				u32 index);
void acpigen_write_pct_package(const acpi_addr_t *perf_ctrl, const acpi_addr_t *perf_sts);
void acpigen_write_xpss_package(const struct acpi_xpss_sw_pstate *pstate_value);
void acpigen_write_xpss_object(const struct acpi_xpss_sw_pstate *pstate_values,
			       size_t nentries);
void acpigen_write_processor(u8 cpuindex, u32 pblock_addr, u8 pblock_len);
__always_inline void acpigen_write_processor_end(void)
{
	acpigen_pop_len();
}
void acpigen_write_processor_package(const char *name,
				     unsigned int first_core,
				     unsigned int core_count);
void acpigen_write_processor_cnot(const unsigned int number_of_cores);
void acpigen_write_TSS_package(int entries, acpi_tstate_t *tstate_list);
void acpigen_write_TSD_package(u32 domain, u32 numprocs, PSD_coord coordtype);
void acpigen_write_mem32fixed(int readwrite, u32 base, u32 size);
void acpigen_write_io16(u16 min, u16 max, u8 align, u8 len, u8 decode16);
void acpigen_write_register_resource(const acpi_addr_t *addr);
void acpigen_write_resourcetemplate_header(void);
void acpigen_write_resourcetemplate_footer(void);
void acpigen_write_mainboard_resource_template(void);
void acpigen_write_mainboard_resources(const char *scope, const char *name);
void acpigen_write_irq(u16 mask);
void acpigen_write_uuid(const char *uuid);
void acpigen_write_power_res(const char *name, uint8_t level, uint16_t order,
			     const char * const dev_states[], size_t dev_states_count);
__always_inline void acpigen_write_power_res_end(void)
{
	acpigen_pop_len();
}
void acpigen_write_sleep(uint64_t sleep_ms);
void acpigen_write_store(void);
void acpigen_write_store_int_to_namestr(uint64_t src, const char *dst);
void acpigen_write_store_int_to_op(uint64_t src, uint8_t dst);
void acpigen_write_store_ops(uint8_t src, uint8_t dst);
void acpigen_write_store_op_to_namestr(uint8_t src, const char *dst);
void acpigen_write_or(uint8_t arg1, uint8_t arg2, uint8_t res);
void acpigen_write_xor(uint8_t arg1, uint8_t arg2, uint8_t res);
void acpigen_write_and(uint8_t arg1, uint8_t arg2, uint8_t res);
void acpigen_write_not(uint8_t arg, uint8_t res);
void acpigen_concatenate_string_string(const char *str1, const char *str2, uint8_t res);
void acpigen_concatenate_string_int(const char *str, uint64_t val, uint8_t res);
void acpigen_concatenate_string_op(const char *str, uint8_t src_res, uint8_t dest_res);
void acpigen_write_debug_string(const char *str);
void acpigen_write_debug_namestr(const char *str);
void acpigen_write_debug_integer(uint64_t val);
void acpigen_write_debug_op(uint8_t op);
void acpigen_write_debug_concatenate_string_string(const char *str1, const char *str2,
						   uint8_t tmp_res);
void acpigen_write_debug_concatenate_string_int(const char *str1, uint64_t val,
						uint8_t tmp_res);
void acpigen_write_debug_concatenate_string_op(const char *str1, uint8_t res, uint8_t tmp_res);
void acpigen_write_if(void);
void acpigen_write_if_and(uint8_t arg1, uint8_t arg2);
void acpigen_write_if_lequal_op_op(uint8_t op, uint8_t val);
void acpigen_write_if_lequal_op_int(uint8_t op, uint64_t val);
void acpigen_write_if_lequal_namestr_int(const char *namestr, uint64_t val);
__always_inline void acpigen_write_if_end(void)
{
	acpigen_pop_len();
}
/* Emits If (CondRefOf(NAME)) */
void acpigen_write_if_cond_ref_of(const char *namestring);
void acpigen_write_else(void);
void acpigen_write_shiftleft_op_int(uint8_t src_result, uint64_t count);
void acpigen_write_to_buffer(uint8_t src, uint8_t dst);
void acpigen_write_to_integer(uint8_t src, uint8_t dst);
void acpigen_write_to_integer_from_namestring(const char *source, uint8_t dst_op);
void acpigen_write_byte_buffer(uint8_t *arr, size_t size);
void acpigen_write_return_byte_buffer(uint8_t *arr, size_t size);
void acpigen_write_return_singleton_buffer(uint8_t arg);
void acpigen_write_return_op(uint8_t arg);
void acpigen_write_return_byte(uint8_t arg);
void acpigen_write_upc(enum acpi_upc_type type);
void acpigen_write_pld(const struct acpi_pld *pld);
void acpigen_write_ADR(uint64_t adr);
struct soundwire_address;
void acpigen_write_ADR_soundwire_device(const struct soundwire_address *address);
void acpigen_write_create_byte_field(uint8_t op, size_t byte_offset, const char *name);
void acpigen_write_create_word_field(uint8_t op, size_t byte_offset, const char *name);
void acpigen_write_create_dword_field(uint8_t op, size_t byte_offset, const char *name);
void acpigen_write_create_qword_field(uint8_t op, size_t byte_offset, const char *name);
void acpigen_write_field_name(const char *name, uint32_t size);
/*
 * Generate ACPI AML code for _DSM method.
 * This function takes as input uuid for the device, set of callbacks and
 * argument to pass into the callbacks. Callbacks should ensure that Local0 and
 * Local1 are left untouched. Use of Local2-Local7 is permitted in callbacks.
 * If the first callback is NULL, then a default implementation of Function 0
 * will be autogenerated, returning a package of bits corresponding to the
 * function callbacks that are non-NULL.
 */
void acpigen_write_dsm(const char *uuid, void (**callbacks)(void *),
		       size_t count, void *arg);

void acpigen_write_dsm_uuid_arr(struct dsm_uuid *ids, size_t count);
/*
 * Generate ACPI AML code for _CPC (Continuous Performance Control).
 * Execute the package function once to create a global table, then
 * execute the method function within each processor object to
 * create a method that points to the global table.
 */
void acpigen_write_CPPC_package(const struct cppc_config *config);
void acpigen_write_CPPC_method(void);

/*
 * Generate ACPI AML code for _ROM method.
 * This function takes as input ROM data and ROM length.
 * The ROM length has to be multiple of 4096 and has to be less
 * than the current implementation limit of 0x40000.
 */
void acpigen_write_rom(void *bios, const size_t length);
/*
 * Generate ACPI AML code for OperationRegion
 * This function takes input region name, region space, region offset & region
 * length.
 */
void acpigen_write_opregion(const struct opregion *opreg);
/*
 * Generate ACPI AML code for Mutex
 * This function takes mutex name and initial value.
 */
void acpigen_write_mutex(const char *name, const uint8_t flags);
/*
 * Generate ACPI AML code for Acquire
 * This function takes mutex name and privilege value.
 */
void acpigen_write_acquire(const char *name, const uint16_t val);
/*
 * Generate ACPI AML code for Release
 * This function takes mutex name.
 */
void acpigen_write_release(const char *name);
/*
 * Generate ACPI AML code for Field
 * This function takes input region name, fieldlist, count & flags.
 */
void acpigen_write_field(const char *name, const struct fieldlist *l, size_t count,
			 uint8_t flags);
/*
 * Generate ACPI AML code for IndexField
 * This function takes input index name, data name, fieldlist, count & flags.
 */
void acpigen_write_indexfield(const char *idx, const char *data,
			      struct fieldlist *l, size_t count, uint8_t flags);

int get_cst_entries(const acpi_cstate_t **);

/*
 * Get element from package into specified destination op:
 *   <dest_op> = DeRefOf (<package_op>[<element>])
 *
 * Example:
 *  acpigen_get_package_op_element(ARG0_OP, 0, LOCAL0_OP)
 *  Local0 = DeRefOf (Arg0[0])
 */
void acpigen_get_package_op_element(uint8_t package_op, unsigned int element, uint8_t dest_op);

/* Set element of package op to specified op:  DeRefOf (<package>[<element>]) = <src> */
void acpigen_set_package_op_element_int(uint8_t package_op, unsigned int element, uint64_t src);

/* Get element from package to specified op:  <dest_op> = <package>[<element>] */
void acpigen_get_package_element(const char *package, unsigned int element, uint8_t dest_op);

/* Set element of package to specified op:  <package>[<element>] = <src> */
void acpigen_set_package_element_int(const char *package, unsigned int element, uint64_t src);

/* Set element of package to specified namestr:  <package>[<element>] = <src> */
void acpigen_set_package_element_namestr(const char *package, unsigned int element,
					 const char *src);

/*
 * Delay up to wait_ms milliseconds until the provided name matches the expected value.
 * If wait_ms is >= 32ms then it will wait in 16ms chunks.  This function uses LOCAL7_OP.
 */
void acpigen_write_delay_until_namestr_int(uint32_t wait_ms, const char *name, uint64_t value);

/*
 * Soc-implemented functions for generating ACPI AML code for GPIO handling. All
 * these functions are expected to use only Local5, Local6 and Local7
 * variables. If the functions call into another ACPI method, then there is no
 * restriction on the use of Local variables. In case of get/read functions,
 * return value is expected to be stored in Local0 variable.
 *
 * All functions return 0 on success and -1 on error.
 */

/* Generate ACPI AML code to return Rx value of GPIO in Local0. */
int acpigen_soc_read_rx_gpio(unsigned int gpio_num);

/* Generate ACPI AML code to return Tx value of GPIO in Local0. */
int acpigen_soc_get_tx_gpio(unsigned int gpio_num);

/* Generate ACPI AML code to set Tx value of GPIO to 1. */
int acpigen_soc_set_tx_gpio(unsigned int gpio_num);

/* Generate ACPI AML code to set Tx value of GPIO to 0. */
int acpigen_soc_clear_tx_gpio(unsigned int gpio_num);

/*
 * Helper functions for enabling/disabling Tx GPIOs based on the GPIO
 * polarity. These functions end up calling acpigen_soc_{set,clear}_tx_gpio to
 * make callbacks into SoC acpigen code.
 *
 * Returns 0 on success and -1 on error.
 */
int acpigen_enable_tx_gpio(const struct acpi_gpio *gpio);
int acpigen_disable_tx_gpio(const struct acpi_gpio *gpio);

/*
 *  Helper function for getting a RX GPIO value based on the GPIO polarity.
 *  The return value is stored in Local0 variable.
 *  This function ends up calling acpigen_soc_get_rx_gpio to make callbacks
 *  into SoC acpigen code
 */
void acpigen_get_rx_gpio(const struct acpi_gpio *gpio);

/*
 *  Helper function for getting a TX GPIO value based on the GPIO polarity.
 *  The return value is stored in Local0 variable.
 *  This function ends up calling acpigen_soc_get_tx_gpio to make callbacks
 *  into SoC acpigen code
 */
void acpigen_get_tx_gpio(const struct acpi_gpio *gpio);

/* refer to ACPI 6.4.3.5.3 Word Address Space Descriptor section for details */
void acpigen_resource_word(u16 res_type, u16 gen_flags, u16 type_flags, u16 gran,
	u16 range_min, u16 range_max, u16 translation, u16 length);
/* refer to ACPI 6.4.3.5.2 DWord Address Space Descriptor section for details */
void acpigen_resource_dword(u16 res_type, u16 gen_flags, u16 type_flags,
	u32 gran, u32 range_min, u32 range_max, u32 translation, u32 length);
/* refer to ACPI 6.4.3.5.1 QWord Address Space Descriptor section for details */
void acpigen_resource_qword(u16 res_type, u16 gen_flags, u16 type_flags,
	u64 gran, u64 range_min, u64 range_max, u64 translation, u64 length);

/* Emits Notify(namestr, value) */
void acpigen_notify(const char *namestr, int value);

#endif /* __ACPI_ACPIGEN_H__ */
