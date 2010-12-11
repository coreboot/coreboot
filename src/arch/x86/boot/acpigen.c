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

/* how many nesting we support */
#define ACPIGEN_LENSTACK_SIZE 10

/* if you need to change this, change the acpigen_write_f and
   acpigen_patch_len */

#define ACPIGEN_MAXLEN 0xfff

#include <string.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <device/device.h>

static char *gencurrent;

char *len_stack[ACPIGEN_LENSTACK_SIZE];
int ltop = 0;

static int acpigen_write_len_f(void)
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

void acpigen_set_current(char *curr)
{
	gencurrent = curr;
}

char *acpigen_get_current(void)
{
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

int acpigen_write_qword(uint64_t data)
{
	/* qword op */
	acpigen_emit_byte(0xe);
	acpigen_emit_byte(data & 0xff);
	acpigen_emit_byte((data >> 8) & 0xff);
	acpigen_emit_byte((data >> 16) & 0xff);
	acpigen_emit_byte((data >> 24) & 0xff);
	acpigen_emit_byte((data >> 32) & 0xff);
	acpigen_emit_byte((data >> 40) & 0xff);
	acpigen_emit_byte((data >> 48) & 0xff);
	acpigen_emit_byte((data >> 56) & 0xff);
	return 9;
}

int acpigen_write_name_byte(const char *name, uint8_t val)
{
	int len;
	len = acpigen_write_name(name);
	len += acpigen_write_byte(val);
	return len;
}

int acpigen_write_name_dword(const char *name, uint32_t val)
{
	int len;
	len = acpigen_write_name(name);
	len += acpigen_write_dword(val);
	return len;
}

int acpigen_write_name_qword(const char *name, uint64_t val)
{
	int len;
	len = acpigen_write_name(name);
	len += acpigen_write_qword(val);
	return len;
}

int acpigen_emit_stream(const char *data, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		acpigen_emit_byte(data[i]);
	}
	return size;
}

/* The NameString are bit tricky, each element can be 4 chars, if
   less its padded with underscore. Check 18.2.2 and 18.4
   and 5.3 of ACPI specs 3.0 for details
*/

static int acpigen_emit_simple_namestring(const char *name) {
	int i, len = 0;
	char ud[] = "____";
	for (i = 0; i < 4; i++) {
		if ((name[i] == '\0') || (name[i] == '.')) {
			len += acpigen_emit_stream(ud, 4 - i);
			break;
		} else {
			len += acpigen_emit_byte(name[i]);
		}
	}
	return len;
}

static int acpigen_emit_double_namestring(const char *name, int dotpos) {
	int len = 0;
	/* mark dual name prefix */
	len += acpigen_emit_byte(0x2e);
	len += acpigen_emit_simple_namestring(name);
	len += acpigen_emit_simple_namestring(&name[dotpos + 1]);
	return len;
}

static int acpigen_emit_multi_namestring(const char *name) {
	int len = 0, count = 0;
	unsigned char *pathlen;
	/* mark multi name prefix */
	len += acpigen_emit_byte(0x2f);
	len += acpigen_emit_byte(0x0);
	pathlen = ((unsigned char *) acpigen_get_current()) - 1;

	while (name[0] != '\0') {
		len += acpigen_emit_simple_namestring(name);
		/* find end or next entity */
		while ((name[0] != '.') && (name[0] != '\0'))
			name++;
		/* forward to next */
		if (name[0] == '.')
			name++;
		count++;
	}

	pathlen[0] = count;
	return len;
}


int acpigen_emit_namestring(const char *namepath) {
	int dotcount = 0, i;
	int dotpos = 0;
	int len = 0;

	/* we can start with a \ */
	if (namepath[0] == '\\') {
		len += acpigen_emit_byte('\\');
		namepath++;
	}

	/* and there can be any number of ^ */
	while (namepath[0] == '^') {
		len += acpigen_emit_byte('^');
		namepath++;
	}

	ASSERT(namepath[0] != '\0');

	i = 0;
	while (namepath[i] != '\0') {
		if (namepath[i] == '.') {
			dotcount++;
			dotpos = i;
		}
		i++;
	}

	if (dotcount == 0) {
		len += acpigen_emit_simple_namestring(namepath);
	} else if (dotcount == 1) {
		len += acpigen_emit_double_namestring(namepath, dotpos);
	} else {
		len += acpigen_emit_multi_namestring(namepath);
	}
	return len;
}

int acpigen_write_name(const char *name)
{
	int len;
	/* name op */
	len = acpigen_emit_byte(0x8);
	return len + acpigen_emit_namestring(name);
}

int acpigen_write_scope(const char *name)
{
	int len;
	/* scope op */
	len = acpigen_emit_byte(0x10);
	len += acpigen_write_len_f();
	return len + acpigen_emit_namestring(name);
}

int acpigen_write_processor(u8 cpuindex, u32 pblock_addr, u8 pblock_len)
{
/*
        Processor (\_PR.CPUcpuindex, cpuindex, pblock_addr, pblock_len)
        {
*/
	char pscope[16];
	int len;
	/* processor op */
	acpigen_emit_byte(0x5b);
	acpigen_emit_byte(0x83);
	len = acpigen_write_len_f();

	sprintf(pscope, "\\_PR.CPU%x", (unsigned int) cpuindex);
	len += acpigen_emit_namestring(pscope);
	acpigen_emit_byte(cpuindex);
	acpigen_emit_byte(pblock_addr & 0xff);
	acpigen_emit_byte((pblock_addr >> 8) & 0xff);
	acpigen_emit_byte((pblock_addr >> 16) & 0xff);
	acpigen_emit_byte((pblock_addr >> 24) & 0xff);
	acpigen_emit_byte(pblock_len);
	return 6 + 2 + len;
}

int acpigen_write_empty_PCT(void)
{
/*
    Name (_PCT, Package (0x02)
    {
        ResourceTemplate ()
        {
            Register (FFixedHW,
                0x00,               // Bit Width
                0x00,               // Bit Offset
                0x0000000000000000, // Address
                ,)
        },

        ResourceTemplate ()
        {
            Register (FFixedHW,
                0x00,               // Bit Width
                0x00,               // Bit Offset
                0x0000000000000000, // Address
                ,)
        }
    })
*/
	static char stream[] = {
		0x08, 0x5F, 0x50, 0x43, 0x54, 0x12, 0x2C,	/* 00000030    "0._PCT.," */
		0x02, 0x11, 0x14, 0x0A, 0x11, 0x82, 0x0C, 0x00,	/* 00000038    "........" */
		0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/* 00000040    "........" */
		0x00, 0x00, 0x00, 0x00, 0x79, 0x00, 0x11, 0x14,	/* 00000048    "....y..." */
		0x0A, 0x11, 0x82, 0x0C, 0x00, 0x7F, 0x00, 0x00,	/* 00000050    "........" */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/* 00000058    "........" */
		0x00, 0x79, 0x00
	};
	return acpigen_emit_stream(stream, ARRAY_SIZE(stream));
}

/* generates a func with max supported P states */
int acpigen_write_PPC(u8 nr)
{
/*
    Method (_PPC, 0, NotSerialized)
    {
        Return (nr)
    }
*/
	int len;
	/* method op */
	acpigen_emit_byte(0x14);
	len = acpigen_write_len_f();
	len += acpigen_emit_namestring("_PPC");
	/* no fnarg */
	acpigen_emit_byte(0x00);
	/* return */
	acpigen_emit_byte(0xa4);
	/* arg */
	len += acpigen_write_byte(nr);
	/* add all single bytes */
	len += 3;
	acpigen_patch_len(len - 1);
	return len;
}

int acpigen_write_PSS_package(u32 coreFreq, u32 power, u32 transLat,
			      u32 busmLat, u32 control, u32 status)
{
	int len;
	len = acpigen_write_package(6);
	len += acpigen_write_dword(coreFreq);
	len += acpigen_write_dword(power);
	len += acpigen_write_dword(transLat);
	len += acpigen_write_dword(busmLat);
	len += acpigen_write_dword(control);
	len += acpigen_write_dword(status);
	//pkglen without the len opcode
	acpigen_patch_len(len - 1);
	return len;
}

int acpigen_write_PSD_package(u32 domain, u32 numprocs, PSD_coord coordtype)
{
	int len, lenh, lenp;
	lenh = acpigen_write_name("_PSD");
	lenp = acpigen_write_package(1);
	len = acpigen_write_package(5);
	len += acpigen_write_byte(5);	// 5 values
	len += acpigen_write_byte(0);	// revision 0
	len += acpigen_write_dword(domain);
	len += acpigen_write_dword(coordtype);
	len += acpigen_write_dword(numprocs);
	acpigen_patch_len(len - 1);
	len += lenp;
	acpigen_patch_len(len - 1);
	return len + lenh;
}

int acpigen_write_mem32fixed(int readwrite, u32 base, u32 size)
{
	/*
	 * acpi 4.0 section 6.4.3.4: 32-Bit Fixed Memory Range Descriptor
	 * Byte 0:
	 *   Bit7  : 1 => big item
	 *   Bit6-0: 0000110 (0x6) => 32-bit fixed memory
	 */
	acpigen_emit_byte(0x86);
	/* Byte 1+2: length (0x0009) */
	acpigen_emit_byte(0x09);
	acpigen_emit_byte(0x00);
	/* bit1-7 are ignored */
	acpigen_emit_byte(readwrite ? 0x01 : 0x00);
	acpigen_emit_byte(base & 0xff);
	acpigen_emit_byte((base >> 8) & 0xff);
	acpigen_emit_byte((base >> 16) & 0xff);
	acpigen_emit_byte((base >> 24) & 0xff);
	acpigen_emit_byte(size & 0xff);
	acpigen_emit_byte((size >> 8) & 0xff);
	acpigen_emit_byte((size >> 16) & 0xff);
	acpigen_emit_byte((size >> 24) & 0xff);
	return 12;
}

int acpigen_write_io16(u16 min, u16 max, u8 align, u8 len, u8 decode16)
{
	/*
	 * acpi 4.0 section 6.4.2.6: I/O Port Descriptor
	 * Byte 0:
	 *   Bit7  : 0 => small item
	 *   Bit6-3: 1000 (0x8) => I/O port descriptor
	 *   Bit2-0: 111 (0x7) => 7 Bytes long
	 */
	acpigen_emit_byte(0x47);
	/* does the device decode all 16 or just 10 bits? */
	/* bit1-7 are ignored */
	acpigen_emit_byte(decode16 ? 0x01 : 0x00);
	/* minimum base address the device may be configured for */
	acpigen_emit_byte(min & 0xff);
	acpigen_emit_byte((min >> 8) & 0xff);
	/* maximum base address the device may be configured for */
	acpigen_emit_byte(max & 0xff);
	acpigen_emit_byte((max >> 8) & 0xff);
	/* alignment for min base */
	acpigen_emit_byte(align & 0xff);
	acpigen_emit_byte(len & 0xff);
	return 8;
}

int acpigen_write_resourcetemplate_header(void)
{
	int len;
	/*
	 * A ResourceTemplate() is a Buffer() with a
	 * (Byte|Word|DWord) containing the length, followed by one or more
	 * resource items, terminated by the end tag
	 * (small item 0xf, len 1)
	 */
	len = acpigen_emit_byte(0x11); /* Buffer opcode */
	len += acpigen_write_len_f();
	len += acpigen_emit_byte(0x0b); /* Word opcode */
	len_stack[ltop++] = acpigen_get_current();
	len += acpigen_emit_byte(0x00);
	len += acpigen_emit_byte(0x00);
	return len;
}

int acpigen_write_resourcetemplate_footer(int len)
{
	char *p = len_stack[--ltop];
	/*
	 * end tag (acpi 4.0 Section 6.4.2.8)
	 * 0x79 <checksum>
	 * 0x00 is treated as a good checksum according to the spec
	 * and is what iasl generates.
	 */
	len += acpigen_emit_byte(0x79);
	len += acpigen_emit_byte(0x00);
	/* patch len word */
	p[0] = (len-6) & 0xff;
	p[1] = ((len-6) >> 8) & 0xff;
	/* patch len field */
	acpigen_patch_len(len-1);
	return 2;
}

static void acpigen_add_mainboard_rsvd_mem32(void *gp, struct device *dev,
						struct resource *res)
{
	acpigen_write_mem32fixed(0, res->base, res->size);
}

static void acpigen_add_mainboard_rsvd_io(void *gp, struct device *dev,
						struct resource *res)
{
	resource_t base = res->base;
	resource_t size = res->size;
	while (size > 0) {
		resource_t sz = size > 255 ? 255 : size;
		acpigen_write_io16(base, base, 0, sz, 1);
		size -= sz;
		base += sz;
	}
}

int acpigen_write_mainboard_resource_template(void)
{
	int len;
	char *start;
	char *end;
	len = acpigen_write_resourcetemplate_header();
	start = acpigen_get_current();

	/* Add reserved memory ranges */
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_RESERVE,
		 IORESOURCE_MEM | IORESOURCE_RESERVE,
		acpigen_add_mainboard_rsvd_mem32, 0);

	/* Add reserved io ranges */
	search_global_resources(
		IORESOURCE_IO | IORESOURCE_RESERVE,
		 IORESOURCE_IO | IORESOURCE_RESERVE,
		acpigen_add_mainboard_rsvd_io, 0);

	end = acpigen_get_current();
	len += end-start;
	len += acpigen_write_resourcetemplate_footer(len);
	return len;
}

int acpigen_write_mainboard_resources(const char *scope, const char *name)
{
	int len;
	len = acpigen_write_scope(scope);
	len += acpigen_write_name(name);
	len += acpigen_write_mainboard_resource_template();
	acpigen_patch_len(len - 1);
	return len;
}
