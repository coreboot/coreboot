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

/* how many nesting we support */
#define ACPIGEN_LENSTACK_SIZE 10

/* if you need to change this, change the acpigen_write_f and
   acpigen_patch_len */

#define ACPIGEN_MAXLEN 0xfff

#include <string.h>
#include <arch/acpigen.h>
#include <console/console.h>

static char *gencurrent;

char *len_stack[ACPIGEN_LENSTACK_SIZE];
int ltop = 0;

static int acpigen_write_len_f()
{
	ASSERT(ltop < (ACPIGEN_LENSTACK_SIZE - 1))
	len_stack[ltop++] = gencurrent;
	acpigen_emit_byte(0);
	acpigen_emit_byte(0);
	return 2;
}

void acpigen_patch_len(int len)
{
	ASSERT(len <= ACPIGEN_MAXLEN)
	ASSERT(ltop > 0)
	char *p = len_stack[--ltop];
	/* generate store length for 0xfff max */
	p[0] = (0x40 | (len & 0xf));
	p[1] = (len >> 4 & 0xff);

}

void acpigen_set_current(char *curr) {
    gencurrent = curr;
}

char *acpigen_get_current(void) {
    return gencurrent;
}

int acpigen_emit_byte(unsigned char b)
{
	(*gencurrent++) = b;
	return 1;
}

int acpigen_write_package(int nr_el)
{
	int len;
	/* package op */
	acpigen_emit_byte(0x12);
	len = acpigen_write_len_f();
	acpigen_emit_byte(nr_el);
	return len + 2;
}

int acpigen_write_byte(unsigned int data)
{
	/* byte op */
	acpigen_emit_byte(0xa);
	acpigen_emit_byte(data & 0xff);
	return 2;
}

int acpigen_write_dword(unsigned int data)
{
	/* dword op */
	acpigen_emit_byte(0xc);
	acpigen_emit_byte(data & 0xff);
	acpigen_emit_byte((data >> 8) & 0xff);
	acpigen_emit_byte((data >> 16) & 0xff);
	acpigen_emit_byte((data >> 24) & 0xff);
	return 5;
}

int acpigen_write_name_byte(char *name, uint8_t val) {
	int len;
	len = acpigen_write_name(name);
	len += acpigen_write_byte(val);
	return len;
}

int acpigen_write_name_dword(char *name, uint32_t val) {
	int len;
	len = acpigen_write_name(name);
	len += acpigen_write_dword(val);
	return len;
}

int acpigen_emit_stream(char *data, int size) {
	int i;
	for (i = 0; i < size; i++) {
		acpigen_emit_byte(data[i]);
	}
	return size;
}

int acpigen_write_name(char *name)
{
	int len = strlen(name);
	/* name op */
	acpigen_emit_byte(0x8);
	acpigen_emit_stream(name, len);
	return len + 1;
}

int acpigen_write_scope(char *name)
{
	int len;
	/* scope op */
	acpigen_emit_byte(0x10);
	len = acpigen_write_len_f();
	return len + acpigen_emit_stream(name, strlen(name)) + 1;
}
