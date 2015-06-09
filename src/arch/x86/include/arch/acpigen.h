/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

void acpigen_write_len_f(void);
void acpigen_pop_len(void);
void acpigen_set_current(char *curr);
char *acpigen_get_current(void);
void acpigen_write_package(int nr_el);
void acpigen_write_byte(unsigned int data);
void acpigen_emit_byte(unsigned char data);
void acpigen_emit_stream(const char *data, int size);
void acpigen_emit_namestring(const char *namepath);
void acpigen_emit_eisaid(const char *eisaid);
void acpigen_write_dword(unsigned int data);
void acpigen_write_qword(uint64_t data);
void acpigen_write_name(const char *name);
void acpigen_write_name_dword(const char *name, uint32_t val);
void acpigen_write_name_qword(const char *name, uint64_t val);
void acpigen_write_name_byte(const char *name, uint8_t val);
void acpigen_write_scope(const char *name);
void acpigen_write_method(const char *name, int nargs);
void acpigen_write_device(const char *name);
void acpigen_write_PPC(u8 nr);
void acpigen_write_PPC_NVS(void);
void acpigen_write_empty_PCT(void);
void acpigen_write_empty_PTC(void);
void acpigen_write_TPC(const char *gnvs_tpc_limit);
void acpigen_write_PSS_package(u32 coreFreq, u32 power, u32 transLat, u32 busmLat,
			u32 control, u32 status);
typedef enum { SW_ALL=0xfc, SW_ANY=0xfd, HW_ALL=0xfe } PSD_coord;
void acpigen_write_PSD_package(u32 domain, u32 numprocs, PSD_coord coordtype);
void acpigen_write_CST_package_entry(acpi_cstate_t *cstate);
void acpigen_write_CST_package(acpi_cstate_t *entry, int nentries);
typedef enum { CSD_HW_ALL=0xfe } CSD_coord;
void acpigen_write_CSD_package(u32 domain, u32 numprocs, CSD_coord coordtype, u32 index);
void acpigen_write_processor(u8 cpuindex, u32 pblock_addr, u8 pblock_len);
void acpigen_write_TSS_package(int entries, acpi_tstate_t *tstate_list);
void acpigen_write_TSD_package(u32 domain, u32 numprocs, PSD_coord coordtype);
void acpigen_write_mem32fixed(int readwrite, u32 base, u32 size);
void acpigen_write_io16(u16 min, u16 max, u8 align, u8 len, u8 decode16);
void acpigen_write_register(acpi_addr_t *addr);
void acpigen_write_resourcetemplate_header(void);
void acpigen_write_resourcetemplate_footer(void);
void acpigen_write_mainboard_resource_template(void);
void acpigen_write_mainboard_resources(const char *scope, const char *name);
void acpigen_write_irq(u16 mask);

int get_cst_entries(acpi_cstate_t **);

#endif
