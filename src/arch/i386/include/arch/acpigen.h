/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef LIBACPI_H
#define LIBACPI_H

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void acpigen_patch_len(int len);
void acpigen_set_current(char *curr);
char *acpigen_get_current(void);
int acpigen_write_package(int nr_el);
int acpigen_write_byte(unsigned int data);
int acpigen_emit_byte(unsigned char data);
int acpigen_emit_stream(const char *data, int size);
int acpigen_emit_namestring(const char *namepath);
int acpigen_write_dword(unsigned int data);
int acpigen_write_qword(uint64_t data);
int acpigen_write_name(const char *name);
int acpigen_write_name_dword(const char *name, uint32_t val);
int acpigen_write_name_qword(const char *name, uint64_t val);
int acpigen_write_name_byte(const char *name, uint8_t val);
int acpigen_write_scope(const char *name);
int acpigen_write_PPC(u8 nr);
int acpigen_write_empty_PCT(void);
int acpigen_write_PSS_package(u32 coreFreq, u32 power, u32 transLat, u32 busmLat,
			u32 control, u32 status);
typedef enum { SW_ALL=0xfc, SW_ANY=0xfd, HW_ALL=0xfe } PSD_coord;
int acpigen_write_PSD_package(u32 domain, u32 numprocs, PSD_coord coordtype);
int acpigen_write_processor(u8 cpuindex, u32 pblock_addr, u8 pblock_len);
int acpigen_write_mem32fixed(int readwrite, u32 base, u32 size);
int acpigen_write_io16(u16 min, u16 max, u8 align, u8 len, u8 decode16);
int acpigen_write_resourcetemplate_header(void);
int acpigen_write_resourcetemplate_footer(int len);
int acpigen_write_mainboard_resource_template(void);
int acpigen_write_mainboard_resources(const char *scope, const char *name);

#endif
