/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>,
 * Raptor Engineering
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

#ifndef LIBACPI_H
#define LIBACPI_H

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <arch/acpi.h>
#include <arch/acpi_device.h>
#include <arch/acpi_pld.h>

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

/* ACPI Op/Prefix Codes */
enum {
	ZERO_OP		= 0x00,
	ONE_OP			= 0x01,
	NAME_OP		= 0x08,
	SCOPE_OP		= 0x10,
	BUFFER_OP		= 0x11,
	BYTE_PREFIX		= 0x0A,
	WORD_PREFIX		= 0x0B,
	DWORD_PREFIX		= 0x0C,
	STRING_PREFIX		= 0x0D,
	QWORD_PREFIX		= 0x0E,
	PACKAGE_OP		= 0x12,
	METHOD_OP		= 0x14,
	DUAL_NAME_PREFIX	= 0x2E,
	MULTI_NAME_PREFIX	= 0x2F,
	EXT_OP_PREFIX		= 0x5B,
	 CREATEFIELD_OP		= 0x13,
	 SLEEP_OP		= 0x22,
	 DEBUG_OP		= 0x31,
	 OPREGION_OP		= 0x80,
	 FIELD_OP		= 0x81,
	 DEVICE_OP		= 0x82,
	 PROCESSOR_OP		= 0x83,
	 POWER_RES_OP		= 0x84,
	LOCAL0_OP		= 0x60,
	LOCAL1_OP		= 0x61,
	LOCAL2_OP		= 0x62,
	LOCAL3_OP		= 0x63,
	LOCAL4_OP		= 0x64,
	LOCAL5_OP		= 0x65,
	LOCAL6_OP		= 0x66,
	LOCAL7_OP		= 0x67,
	ARG0_OP		= 0x68,
	ARG1_OP		= 0x69,
	ARG2_OP		= 0x6A,
	ARG3_OP		= 0x6B,
	ARG4_OP		= 0x6C,
	ARG5_OP		= 0x6D,
	ARG6_OP		= 0x6E,
	STORE_OP		= 0x70,
	SUBTRACT_OP		= 0x74,
	MULTIPLY_OP		= 0x77,
	AND_OP			= 0x7B,
	OR_OP			= 0x7D,
	NOT_OP			= 0x80,
	NOTIFY_OP		= 0x86,
	LEQUAL_OP		= 0x93,
	LGREATER_OP		= 0x94,
	LLESS_OP		= 0x95,
	TO_BUFFER_OP		= 0x96,
	TO_INTEGER_OP		= 0x99,
	IF_OP			= 0xA0,
	ELSE_OP		= 0xA1,
	RETURN_OP		= 0xA4,
	ONES_OP		= 0xFF,
};

#define FIELDLIST_OFFSET(X)		{ .type = OFFSET, \
					  .name = "",\
					  .bits = X * 8, \
					}
#define FIELDLIST_NAMESTR(X, Y)		{ .type = NAME_STRING, \
					  .name = X, \
					  .bits = Y, \
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
	REGION_SPACE_MAX,
};

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

void acpigen_write_return_integer(uint64_t arg);
void acpigen_write_return_string(const char *arg);
void acpigen_write_len_f(void);
void acpigen_pop_len(void);
void acpigen_set_current(char *curr);
char *acpigen_get_current(void);
char *acpigen_write_package(int nr_el);
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
void acpigen_write_name(const char *name);
void acpigen_write_name_zero(const char *name);
void acpigen_write_name_one(const char *name);
void acpigen_write_name_string(const char *name, const char *string);
void acpigen_write_name_dword(const char *name, uint32_t val);
void acpigen_write_name_qword(const char *name, uint64_t val);
void acpigen_write_name_byte(const char *name, uint8_t val);
void acpigen_write_name_integer(const char *name, uint64_t val);
void acpigen_write_coreboot_hid(enum coreboot_acpi_ids id);
void acpigen_write_scope(const char *name);
void acpigen_write_method(const char *name, int nargs);
void acpigen_write_method_serialized(const char *name, int nargs);
void acpigen_write_device(const char *name);
void acpigen_write_PPC(u8 nr);
void acpigen_write_PPC_NVS(void);
void acpigen_write_empty_PCT(void);
void acpigen_write_empty_PTC(void);
void acpigen_write_PRW(u32 wake, u32 level);
void acpigen_write_STA(uint8_t status);
void acpigen_write_TPC(const char *gnvs_tpc_limit);
void acpigen_write_PSS_package(u32 coreFreq, u32 power, u32 transLat,
			u32 busmLat, u32 control, u32 status);
typedef enum { SW_ALL = 0xfc, SW_ANY = 0xfd, HW_ALL = 0xfe } PSD_coord;
void acpigen_write_PSD_package(u32 domain, u32 numprocs, PSD_coord coordtype);
void acpigen_write_CST_package_entry(acpi_cstate_t *cstate);
void acpigen_write_CST_package(acpi_cstate_t *entry, int nentries);
typedef enum { CSD_HW_ALL = 0xfe } CSD_coord;
void acpigen_write_CSD_package(u32 domain, u32 numprocs, CSD_coord coordtype,
				u32 index);
void acpigen_write_processor(u8 cpuindex, u32 pblock_addr, u8 pblock_len);
void acpigen_write_processor_package(const char *name,
				     unsigned int first_core,
				     unsigned int core_count);
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
			     const char *dev_states[], size_t dev_states_count);
void acpigen_write_sleep(uint64_t sleep_ms);
void acpigen_write_store(void);
void acpigen_write_store_ops(uint8_t src, uint8_t dst);
void acpigen_write_or(uint8_t arg1, uint8_t arg2, uint8_t res);
void acpigen_write_and(uint8_t arg1, uint8_t arg2, uint8_t res);
void acpigen_write_not(uint8_t arg, uint8_t res);
void acpigen_write_debug_string(const char *str);
void acpigen_write_debug_integer(uint64_t val);
void acpigen_write_debug_op(uint8_t op);
void acpigen_write_if(void);
void acpigen_write_if_and(uint8_t arg1, uint8_t arg2);
void acpigen_write_if_lequal_op_int(uint8_t op, uint64_t val);
void acpigen_write_else(void);
void acpigen_write_to_buffer(uint8_t src, uint8_t dst);
void acpigen_write_to_integer(uint8_t src, uint8_t dst);
void acpigen_write_byte_buffer(uint8_t *arr, size_t size);
void acpigen_write_return_byte_buffer(uint8_t *arr, size_t size);
void acpigen_write_return_singleton_buffer(uint8_t arg);
void acpigen_write_return_byte(uint8_t arg);
void acpigen_write_upc(enum acpi_upc_type type);
void acpigen_write_pld(const struct acpi_pld *pld);
/*
 * Generate ACPI AML code for _DSM method.
 * This function takes as input uuid for the device, set of callbacks and
 * argument to pass into the callbacks. Callbacks should ensure that Local0 and
 * Local1 are left untouched. Use of Local2-Local7 is permitted in callbacks.
 */
void acpigen_write_dsm(const char *uuid, void (**callbacks)(void *),
		       size_t count, void *arg);
void acpigen_write_dsm_uuid_arr(struct dsm_uuid *ids, size_t count);

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
void acpigen_write_opregion(struct opregion *opreg);
/*
 * Generate ACPI AML code for Field
 * This function takes input region name, fieldlist, count & flags.
 */
void acpigen_write_field(const char *name, struct fieldlist *l, size_t count,
			 uint8_t flags);

int get_cst_entries(acpi_cstate_t **);

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
int acpigen_enable_tx_gpio(struct acpi_gpio *gpio);
int acpigen_disable_tx_gpio(struct acpi_gpio *gpio);

#endif
