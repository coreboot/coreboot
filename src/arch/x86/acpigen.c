/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 */

/* How much nesting do we support? */
#define ACPIGEN_LENSTACK_SIZE 10

/*
 * If you need to change this, change acpigen_write_len_f and
 * acpigen_pop_len
 */

#define ACPIGEN_MAXLEN 0xfffff

#include <lib.h>
#include <string.h>
#include <arch/acpigen.h>
#include <console/console.h>
#include <device/device.h>

static char *gencurrent;

char *len_stack[ACPIGEN_LENSTACK_SIZE];
int ltop = 0;

void acpigen_write_len_f(void)
{
	ASSERT(ltop < (ACPIGEN_LENSTACK_SIZE - 1))
	len_stack[ltop++] = gencurrent;
	acpigen_emit_byte(0);
	acpigen_emit_byte(0);
	acpigen_emit_byte(0);
}

void acpigen_pop_len(void)
{
	int len;
	ASSERT(ltop > 0)
	char *p = len_stack[--ltop];
	len = gencurrent - p;
	ASSERT(len <= ACPIGEN_MAXLEN)
	/* generate store length for 0xfffff max */
	p[0] = (0x80 | (len & 0xf));
	p[1] = (len >> 4 & 0xff);
	p[2] = (len >> 12 & 0xff);

}

void acpigen_set_current(char *curr)
{
	gencurrent = curr;
}

char *acpigen_get_current(void)
{
	return gencurrent;
}

void acpigen_emit_byte(unsigned char b)
{
	(*gencurrent++) = b;
}

void acpigen_emit_ext_op(uint8_t op)
{
	acpigen_emit_byte(EXT_OP_PREFIX);
	acpigen_emit_byte(op);
}

void acpigen_emit_word(unsigned int data)
{
	acpigen_emit_byte(data & 0xff);
	acpigen_emit_byte((data >> 8) & 0xff);
}

void acpigen_emit_dword(unsigned int data)
{
	acpigen_emit_byte(data & 0xff);
	acpigen_emit_byte((data >> 8) & 0xff);
	acpigen_emit_byte((data >> 16) & 0xff);
	acpigen_emit_byte((data >> 24) & 0xff);
}

char *acpigen_write_package(int nr_el)
{
	char *p;
	acpigen_emit_byte(PACKAGE_OP);
	acpigen_write_len_f();
	p = acpigen_get_current();
	acpigen_emit_byte(nr_el);
	return p;
}

void acpigen_write_byte(unsigned int data)
{
	acpigen_emit_byte(BYTE_PREFIX);
	acpigen_emit_byte(data & 0xff);
}

void acpigen_write_word(unsigned int data)
{
	acpigen_emit_byte(WORD_PREFIX);
	acpigen_emit_word(data);
}

void acpigen_write_dword(unsigned int data)
{
	acpigen_emit_byte(DWORD_PREFIX);
	acpigen_emit_dword(data);
}

void acpigen_write_qword(uint64_t data)
{
	acpigen_emit_byte(QWORD_PREFIX);
	acpigen_emit_dword(data & 0xffffffff);
	acpigen_emit_dword((data >> 32) & 0xffffffff);
}

void acpigen_write_zero(void)
{
	acpigen_emit_byte(ZERO_OP);
}

void acpigen_write_one(void)
{
	acpigen_emit_byte(ONE_OP);
}

void acpigen_write_ones(void)
{
	acpigen_emit_byte(ONES_OP);
}

void acpigen_write_integer(uint64_t data)
{
	if (data == 0)
		acpigen_write_zero();
	else if (data == 1)
		acpigen_write_one();
	else if (data <= 0xff)
		acpigen_write_byte((unsigned char)data);
	else if (data <= 0xffff)
		acpigen_write_word((unsigned int)data);
	else if (data <= 0xffffffff)
		acpigen_write_dword((unsigned int)data);
	else
		acpigen_write_qword(data);
}

void acpigen_write_name_zero(const char *name)
{
	acpigen_write_name(name);
	acpigen_write_one();
}

void acpigen_write_name_one(const char *name)
{
	acpigen_write_name(name);
	acpigen_write_zero();
}

void acpigen_write_name_byte(const char *name, uint8_t val)
{
	acpigen_write_name(name);
	acpigen_write_byte(val);
}

void acpigen_write_name_dword(const char *name, uint32_t val)
{
	acpigen_write_name(name);
	acpigen_write_dword(val);
}

void acpigen_write_name_qword(const char *name, uint64_t val)
{
	acpigen_write_name(name);
	acpigen_write_qword(val);
}

void acpigen_write_name_integer(const char *name, uint64_t val)
{
	acpigen_write_name(name);
	acpigen_write_integer(val);
}

void acpigen_write_name_string(const char *name, const char *string)
{
	acpigen_write_name(name);
	acpigen_write_string(string);
}

void acpigen_emit_stream(const char *data, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		acpigen_emit_byte(data[i]);
	}
}

void acpigen_emit_string(const char *string)
{
	acpigen_emit_stream(string, string ? strlen(string) : 0);
	acpigen_emit_byte('\0'); /* NUL */
}

void acpigen_write_string(const char *string)
{
	acpigen_emit_byte(STRING_PREFIX);
	acpigen_emit_string(string);
}

void acpigen_write_coreboot_hid(enum coreboot_acpi_ids id)
{
	char hid[9]; /* CORExxxx */

	snprintf(hid, sizeof(hid), "%.4s%04X", COREBOOT_ACPI_ID, id);
	acpigen_write_name_string("_HID", hid);
}

/*
 * The naming conventions for ACPI namespace names are a bit tricky as
 * each element has to be 4 chars wide ("All names are a fixed 32 bits.")
 * and "By convention, when an ASL compiler pads a name shorter than 4
 * characters, it is done so with trailing underscores ('_')".
 *
 * Check sections 5.3, 18.2.2 and 18.4 of ACPI spec 3.0 for details.
 */

static void acpigen_emit_simple_namestring(const char *name) {
	int i;
	char ud[] = "____";
	for (i = 0; i < 4; i++) {
		if ((name[i] == '\0') || (name[i] == '.')) {
			acpigen_emit_stream(ud, 4 - i);
			break;
		} else {
			acpigen_emit_byte(name[i]);
		}
	}
}

static void acpigen_emit_double_namestring(const char *name, int dotpos) {
	acpigen_emit_byte(DUAL_NAME_PREFIX);
	acpigen_emit_simple_namestring(name);
	acpigen_emit_simple_namestring(&name[dotpos + 1]);
}

static void acpigen_emit_multi_namestring(const char *name) {
	int count = 0;
	unsigned char *pathlen;
	acpigen_emit_byte(MULTI_NAME_PREFIX);
	acpigen_emit_byte(ZERO_OP);
	pathlen = ((unsigned char *) acpigen_get_current()) - 1;

	while (name[0] != '\0') {
		acpigen_emit_simple_namestring(name);
		/* find end or next entity */
		while ((name[0] != '.') && (name[0] != '\0'))
			name++;
		/* forward to next */
		if (name[0] == '.')
			name++;
		count++;
	}

	pathlen[0] = count;
}


void acpigen_emit_namestring(const char *namepath) {
	int dotcount = 0, i;
	int dotpos = 0;

	/* We can start with a '\'. */
	if (namepath[0] == '\\') {
		acpigen_emit_byte('\\');
		namepath++;
	}

	/* And there can be any number of '^' */
	while (namepath[0] == '^') {
		acpigen_emit_byte('^');
		namepath++;
	}

	/* If we have only \\ or only ^...^. Then we need to put a null
	   name (0x00). */
	if (namepath[0] == '\0') {
		acpigen_emit_byte(ZERO_OP);
		return;
	}

	i = 0;
	while (namepath[i] != '\0') {
		if (namepath[i] == '.') {
			dotcount++;
			dotpos = i;
		}
		i++;
	}

	if (dotcount == 0) {
		acpigen_emit_simple_namestring(namepath);
	} else if (dotcount == 1) {
		acpigen_emit_double_namestring(namepath, dotpos);
	} else {
		acpigen_emit_multi_namestring(namepath);
	}
}

void acpigen_write_name(const char *name)
{
	acpigen_emit_byte(NAME_OP);
	acpigen_emit_namestring(name);
}

void acpigen_write_scope(const char *name)
{
	acpigen_emit_byte(SCOPE_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(name);
}

void acpigen_write_processor(u8 cpuindex, u32 pblock_addr, u8 pblock_len)
{
/*
	Processor (\_PR.CPUcpuindex, cpuindex, pblock_addr, pblock_len)
	{
*/
	char pscope[16];
	acpigen_emit_ext_op(PROCESSOR_OP);
	acpigen_write_len_f();

	snprintf(pscope, sizeof(pscope),
		 "\\_PR.CP%02d", (unsigned int) cpuindex);
	acpigen_emit_namestring(pscope);
	acpigen_emit_byte(cpuindex);
	acpigen_emit_dword(pblock_addr);
	acpigen_emit_byte(pblock_len);
}

void acpigen_write_empty_PCT(void)
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
	acpigen_emit_stream(stream, ARRAY_SIZE(stream));
}

void acpigen_write_empty_PTC(void)
{
/*
	Name (_PTC, Package (0x02)
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
	acpi_addr_t addr = {
		.space_id   = ACPI_ADDRESS_SPACE_FIXED,
		.bit_width  = 0,
		.bit_offset = 0,
		{
			.resv       = 0
		},
		.addrl      = 0,
		.addrh      = 0,
	};

	acpigen_write_name("_PTC");
	acpigen_write_package(2);

	/* ControlRegister */
	acpigen_write_resourcetemplate_header();
	acpigen_write_register(&addr);
	acpigen_write_resourcetemplate_footer();

	/* StatusRegister */
	acpigen_write_resourcetemplate_header();
	acpigen_write_register(&addr);
	acpigen_write_resourcetemplate_footer();

	acpigen_pop_len();
}

static void __acpigen_write_method(const char *name, uint8_t flags)
{
	acpigen_emit_byte(METHOD_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(name);
	acpigen_emit_byte(flags);
}

/* Method (name, nargs, NotSerialized) */
void acpigen_write_method(const char *name, int nargs)
{
	__acpigen_write_method(name, (nargs & 7));
}

/* Method (name, nargs, Serialized) */
void acpigen_write_method_serialized(const char *name, int nargs)
{
	__acpigen_write_method(name, (nargs & 7) | (1 << 3));
}

void acpigen_write_device(const char *name)
{
	acpigen_emit_ext_op(DEVICE_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(name);
}

void acpigen_write_STA(uint8_t status)
{
	/*
	 * Method (_STA, 0, NotSerialized) { Return (status) }
	 */
	acpigen_write_method("_STA", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte(status);
	acpigen_pop_len();
}

/*
 * Generates a func with max supported P-states.
 */
void acpigen_write_PPC(u8 nr)
{
/*
	Method (_PPC, 0, NotSerialized)
	{
		Return (nr)
	}
*/
	acpigen_write_method("_PPC", 0);
	acpigen_emit_byte(RETURN_OP);
	/* arg */
	acpigen_write_byte(nr);
	acpigen_pop_len();
}

/*
 * Generates a func with max supported P-states saved
 * in the variable PPCM.
 */
void acpigen_write_PPC_NVS(void)
{
/*
	Method (_PPC, 0, NotSerialized)
	{
		Return (PPCM)
	}
*/
	acpigen_write_method("_PPC", 0);
	acpigen_emit_byte(RETURN_OP);
	/* arg */
	acpigen_emit_namestring("PPCM");
	acpigen_pop_len();
}

void acpigen_write_TPC(const char *gnvs_tpc_limit)
{
/*
	// Sample _TPC method
	Method (_TPC, 0, NotSerialized)
	{
		Return (\TLVL)
	}
*/
	acpigen_write_method("_TPC", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(gnvs_tpc_limit);
	acpigen_pop_len();
}

void acpigen_write_PRW(u32 wake, u32 level)
{
	/*
	 * Name (_PRW, Package () { wake, level }
	 */
	acpigen_write_name("_PRW");
	acpigen_write_package(2);
	acpigen_write_integer(wake);
	acpigen_write_integer(level);
	acpigen_pop_len();
}

void acpigen_write_PSS_package(u32 coreFreq, u32 power, u32 transLat,
			      u32 busmLat, u32 control, u32 status)
{
	acpigen_write_package(6);
	acpigen_write_dword(coreFreq);
	acpigen_write_dword(power);
	acpigen_write_dword(transLat);
	acpigen_write_dword(busmLat);
	acpigen_write_dword(control);
	acpigen_write_dword(status);
	acpigen_pop_len();

	printk(BIOS_DEBUG, "PSS: %uMHz power %u control 0x%x status 0x%x\n",
	       coreFreq, power, control, status);
}

void acpigen_write_PSD_package(u32 domain, u32 numprocs, PSD_coord coordtype)
{
	acpigen_write_name("_PSD");
	acpigen_write_package(1);
	acpigen_write_package(5);
	acpigen_write_byte(5);	// 5 values
	acpigen_write_byte(0);	// revision 0
	acpigen_write_dword(domain);
	acpigen_write_dword(coordtype);
	acpigen_write_dword(numprocs);
	acpigen_pop_len();
	acpigen_pop_len();
}

void acpigen_write_CST_package_entry(acpi_cstate_t *cstate)
{
	acpigen_write_package(4);
	acpigen_write_resourcetemplate_header();
	acpigen_write_register(&cstate->resource);
	acpigen_write_resourcetemplate_footer();
	acpigen_write_dword(cstate->ctype);
	acpigen_write_dword(cstate->latency);
	acpigen_write_dword(cstate->power);
	acpigen_pop_len();
}

void acpigen_write_CST_package(acpi_cstate_t *cstate, int nentries)
{
	int i;
	acpigen_write_name("_CST");
	acpigen_write_package(nentries+1);
	acpigen_write_dword(nentries);

	for (i = 0; i < nentries; i++)
		acpigen_write_CST_package_entry(cstate + i);

	acpigen_pop_len();
}

void acpigen_write_CSD_package(u32 domain, u32 numprocs, CSD_coord coordtype, u32 index)
{
	acpigen_write_name("_CSD");
	acpigen_write_package(1);
	acpigen_write_package(6);
	acpigen_write_byte(6);	// 6 values
	acpigen_write_byte(0);	// revision 0
	acpigen_write_dword(domain);
	acpigen_write_dword(coordtype);
	acpigen_write_dword(numprocs);
	acpigen_write_dword(index);
	acpigen_pop_len();
	acpigen_pop_len();
}

void acpigen_write_TSS_package(int entries, acpi_tstate_t *tstate_list)
{
/*
	Sample _TSS package with 100% and 50% duty cycles
	Name (_TSS, Package (0x02)
	{
		Package(){100, 1000, 0, 0x00, 0)
		Package(){50, 520, 0, 0x18, 0)
	})
*/
	int i;
	acpi_tstate_t *tstate = tstate_list;

	acpigen_write_name("_TSS");
	acpigen_write_package(entries);

	for (i = 0; i < entries; i++) {
		acpigen_write_package(5);
		acpigen_write_dword(tstate->percent);
		acpigen_write_dword(tstate->power);
		acpigen_write_dword(tstate->latency);
		acpigen_write_dword(tstate->control);
		acpigen_write_dword(tstate->status);
		acpigen_pop_len();
		tstate++;
	}

	acpigen_pop_len();
}

void acpigen_write_TSD_package(u32 domain, u32 numprocs, PSD_coord coordtype)
{
	acpigen_write_name("_TSD");
	acpigen_write_package(1);
	acpigen_write_package(5);
	acpigen_write_byte(5);	// 5 values
	acpigen_write_byte(0);	// revision 0
	acpigen_write_dword(domain);
	acpigen_write_dword(coordtype);
	acpigen_write_dword(numprocs);
	acpigen_pop_len();
	acpigen_pop_len();
}



void acpigen_write_mem32fixed(int readwrite, u32 base, u32 size)
{
	/*
	 * ACPI 4.0 section 6.4.3.4: 32-Bit Fixed Memory Range Descriptor
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
	acpigen_emit_dword(base);
	acpigen_emit_dword(size);
}

void acpigen_write_register(acpi_addr_t *addr)
{
	acpigen_emit_byte(0x82);		/* Register Descriptor */
	acpigen_emit_byte(0x0c);		/* Register Length 7:0 */
	acpigen_emit_byte(0x00);		/* Register Length 15:8 */
	acpigen_emit_byte(addr->space_id);	/* Address Space ID */
	acpigen_emit_byte(addr->bit_width);	/* Register Bit Width */
	acpigen_emit_byte(addr->bit_offset);	/* Register Bit Offset */
	acpigen_emit_byte(addr->resv);		/* Register Access Size */
	acpigen_emit_dword(addr->addrl);	/* Register Address Low */
	acpigen_emit_dword(addr->addrh);	/* Register Address High */
}

void acpigen_write_irq(u16 mask)
{
	/*
	 * ACPI 3.0b section 6.4.2.1: IRQ Descriptor
	 * Byte 0:
	 *   Bit7  : 0 => small item
	 *   Bit6-3: 0100 (0x4) => IRQ port descriptor
	 *   Bit2-0: 010 (0x2) => 2 Bytes long
	 */
	acpigen_emit_byte(0x22);
	acpigen_emit_byte(mask & 0xff);
	acpigen_emit_byte((mask >> 8) & 0xff);
}

void acpigen_write_io16(u16 min, u16 max, u8 align, u8 len, u8 decode16)
{
	/*
	 * ACPI 4.0 section 6.4.2.6: I/O Port Descriptor
	 * Byte 0:
	 *   Bit7  : 0 => small item
	 *   Bit6-3: 1000 (0x8) => I/O port descriptor
	 *   Bit2-0: 111 (0x7) => 7 Bytes long
	 */
	acpigen_emit_byte(0x47);
	/* Does the device decode all 16 or just 10 bits? */
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
}

void acpigen_write_resourcetemplate_header(void)
{
	/*
	 * A ResourceTemplate() is a Buffer() with a
	 * (Byte|Word|DWord) containing the length, followed by one or more
	 * resource items, terminated by the end tag.
	 * (small item 0xf, len 1)
	 */
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(WORD_PREFIX);
	len_stack[ltop++] = acpigen_get_current();
	acpigen_emit_byte(0x00);
	acpigen_emit_byte(0x00);
}

void acpigen_write_resourcetemplate_footer(void)
{
	char *p = len_stack[--ltop];
	int len;
	/*
	 * end tag (acpi 4.0 Section 6.4.2.8)
	 * 0x79 <checksum>
	 * 0x00 is treated as a good checksum according to the spec
	 * and is what iasl generates.
	 */
	acpigen_emit_byte(0x79);
	acpigen_emit_byte(0x00);

	len = gencurrent - p;

	/* patch len word */
	p[0] = len & 0xff;
	p[1] = (len >> 8) & 0xff;
	/* patch len field */
	acpigen_pop_len();
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

void acpigen_write_mainboard_resource_template(void)
{
	acpigen_write_resourcetemplate_header();

	/* Add reserved memory ranges. */
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_RESERVE,
		 IORESOURCE_MEM | IORESOURCE_RESERVE,
		acpigen_add_mainboard_rsvd_mem32, 0);

	/* Add reserved io ranges. */
	search_global_resources(
		IORESOURCE_IO | IORESOURCE_RESERVE,
		 IORESOURCE_IO | IORESOURCE_RESERVE,
		acpigen_add_mainboard_rsvd_io, 0);

	acpigen_write_resourcetemplate_footer();
}

void acpigen_write_mainboard_resources(const char *scope, const char *name)
{
	acpigen_write_scope(scope);
	acpigen_write_name(name);
	acpigen_write_mainboard_resource_template();
	acpigen_pop_len();
}

static int hex2bin(const char c)
{
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return c - '0';
}

void acpigen_emit_eisaid(const char *eisaid)
{
	u32 compact = 0;

	/* Clamping individual values would be better but
	   there is a disagreement over what is a valid
	   EISA id, so accept anything and don't clamp,
	   parent code should create a valid EISAid.
	 */
	compact |= (eisaid[0] - 'A' + 1) << 26;
	compact |= (eisaid[1] - 'A' + 1) << 21;
	compact |= (eisaid[2] - 'A' + 1) << 16;
	compact |= hex2bin(eisaid[3]) << 12;
	compact |= hex2bin(eisaid[4]) << 8;
	compact |= hex2bin(eisaid[5]) << 4;
	compact |= hex2bin(eisaid[6]);

	acpigen_emit_byte(0xc);
	acpigen_emit_byte((compact >> 24) & 0xff);
	acpigen_emit_byte((compact >> 16) & 0xff);
	acpigen_emit_byte((compact >> 8) & 0xff);
	acpigen_emit_byte(compact & 0xff);
}

/*
 * ToUUID(uuid)
 *
 * ACPI 6.1 Section 19.6.136 table 19-385 defines a special output
 * order for the bytes that make up a UUID Buffer object.
 * UUID byte order for input:
 *   aabbccdd-eeff-gghh-iijj-kkllmmnnoopp
 * UUID byte order for output:
 *   ddccbbaa-ffee-hhgg-iijj-kkllmmnnoopp
 */
#define UUID_LEN 16
void acpigen_write_uuid(const char *uuid)
{
	uint8_t buf[UUID_LEN];
	size_t i, order[UUID_LEN] = { 3, 2, 1, 0, 5, 4, 7, 6,
				      8, 9, 10, 11, 12, 13, 14, 15 };

	/* Parse UUID string into bytes */
	if (hexstrtobin(uuid, buf, UUID_LEN) < UUID_LEN)
		return;

	/* BufferOp */
	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();

	/* Buffer length in bytes */
	acpigen_write_word(UUID_LEN);

	/* Output UUID in expected order */
	for (i = 0; i < UUID_LEN; i++)
		acpigen_emit_byte(buf[order[i]]);

	acpigen_pop_len();
}

/*
 * Name (_PRx, Package(One) { name })
 * ...
 * PowerResource (name, level, order)
 */
void acpigen_write_power_res(const char *name, uint8_t level, uint16_t order,
			     const char *dev_states[], size_t dev_states_count)
{
	int i;
	for (i = 0; i < dev_states_count; i++) {
		acpigen_write_name(dev_states[i]);
		acpigen_write_package(1);
		acpigen_emit_simple_namestring(name);
		acpigen_pop_len();		/* Package */
	}

	acpigen_emit_ext_op(POWER_RES_OP);

	acpigen_write_len_f();

	acpigen_emit_simple_namestring(name);
	acpigen_emit_byte(level);
	acpigen_emit_word(order);
}

/* Sleep (ms) */
void acpigen_write_sleep(uint64_t sleep_ms)
{
	acpigen_emit_ext_op(SLEEP_OP);
	acpigen_write_integer(sleep_ms);
}

void acpigen_write_store(void)
{
	acpigen_emit_byte(STORE_OP);
}

/* Store (src, dst) */
void acpigen_write_store_ops(uint8_t src, uint8_t dst)
{
	acpigen_write_store();
	acpigen_emit_byte(src);
	acpigen_emit_byte(dst);
}

/* Or (arg1, arg2, res) */
void acpigen_write_or(uint8_t arg1, uint8_t arg2, uint8_t res)
{
	acpigen_emit_byte(OR_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
	acpigen_emit_byte(res);
}

/* And (arg1, arg2, res) */
void acpigen_write_and(uint8_t arg1, uint8_t arg2, uint8_t res)
{
	acpigen_emit_byte(AND_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
	acpigen_emit_byte(res);
}

/* Not (arg, res) */
void acpigen_write_not(uint8_t arg, uint8_t res)
{
	acpigen_emit_byte(NOT_OP);
	acpigen_emit_byte(arg);
	acpigen_emit_byte(res);
}

/* Store (str, DEBUG) */
void acpigen_write_debug_string(const char *str)
{
	acpigen_write_store();
	acpigen_write_string(str);
	acpigen_emit_ext_op(DEBUG_OP);
}

/* Store (val, DEBUG) */
void acpigen_write_debug_integer(uint64_t val)
{
	acpigen_write_store();
	acpigen_write_integer(val);
	acpigen_emit_ext_op(DEBUG_OP);
}

/* Store (op, DEBUG) */
void acpigen_write_debug_op(uint8_t op)
{
	acpigen_write_store();
	acpigen_emit_byte(op);
	acpigen_emit_ext_op(DEBUG_OP);
}

void acpigen_write_if(void)
{
	acpigen_emit_byte(IF_OP);
	acpigen_write_len_f();
}

/* If (And (arg1, arg2)) */
void acpigen_write_if_and(uint8_t arg1, uint8_t arg2)
{
	acpigen_write_if();
	acpigen_emit_byte(AND_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
}

void acpigen_write_if_lequal(uint8_t arg1, uint8_t arg2)
{
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(arg1);
	acpigen_emit_byte(arg2);
}

void acpigen_write_else(void)
{
	acpigen_emit_byte(ELSE_OP);
	acpigen_write_len_f();
}

void acpigen_write_to_buffer(uint8_t src, uint8_t dst)
{
	acpigen_emit_byte(TO_BUFFER_OP);
	acpigen_emit_byte(src);
	acpigen_emit_byte(dst);
}

void acpigen_write_to_integer(uint8_t src, uint8_t dst)
{
	acpigen_emit_byte(TO_INTEGER_OP);
	acpigen_emit_byte(src);
	acpigen_emit_byte(dst);
}

void acpigen_write_byte_buffer(uint8_t *arr, uint8_t size)
{
	uint8_t i;

	acpigen_emit_byte(BUFFER_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(size);

	for (i = 0; i < size; i++)
		acpigen_emit_byte(arr[i]);

	acpigen_pop_len();
}

void acpigen_write_return_byte_buffer(uint8_t *arr, uint8_t size)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte_buffer(arr, size);
}

void acpigen_write_return_singleton_buffer(uint8_t arg)
{
	acpigen_write_return_byte_buffer(&arg, 1);
}

void acpigen_write_return_byte(uint8_t arg)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_byte(arg);
}

/* Soc-implemented functions -- weak definitions. */
int __attribute__((weak)) acpigen_soc_read_rx_gpio(unsigned int gpio_num)
{
	printk(BIOS_ERR, "ERROR: %s not implemented\n", __func__);
	acpigen_write_debug_string("read_rx_gpio not available");
	return -1;
}

int __attribute__((weak)) acpigen_soc_get_tx_gpio(unsigned int gpio_num)
{
	printk(BIOS_ERR, "ERROR: %s not implemented\n", __func__);
	acpigen_write_debug_string("get_tx_gpio not available");
	return -1;
}

int __attribute__((weak)) acpigen_soc_set_tx_gpio(unsigned int gpio_num)
{
	printk(BIOS_ERR, "ERROR: %s not implemented\n", __func__);
	acpigen_write_debug_string("set_tx_gpio not available");
	return -1;
}

int __attribute__((weak)) acpigen_soc_clear_tx_gpio(unsigned int gpio_num)
{
	printk(BIOS_ERR, "ERROR: %s not implemented\n", __func__);
	acpigen_write_debug_string("clear_tx_gpio not available");
	return -1;
}
