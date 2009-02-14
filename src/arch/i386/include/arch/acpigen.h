/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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
int acpigen_emit_stream(char *data, int size);
int acpigen_write_dword(unsigned int data);
int acpigen_write_name(char *name);
int acpigen_write_name_dword(char *name, uint32_t val);
int acpigen_write_name_byte(char *name, uint8_t val);
int acpigen_write_scope(char *name);
int acpigen_write_PPC(u8 nr);
int acpigen_write_empty_PCT(void);
int acpigen_write_PSS_package(u32 coreFreq, u32 power, u32 transLat, u32 busmLat,
			u32 control, u32 status);
int acpigen_write_processor(u8 cpuindex, u32 pblock_addr, u8 pblock_len);
#endif
