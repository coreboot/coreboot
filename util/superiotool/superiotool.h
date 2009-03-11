/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2007 Carl-Daniel Hailfinger
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2008 Robinson P. Tryon <bishop.robinson@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef SUPERIOTOOL_H
#define SUPERIOTOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#if defined(__GLIBC__)
#include <sys/io.h>
#endif

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <machine/cpufunc.h>
#define OUTB(x, y) do { u_int tmp = (y); outb(tmp, (x)); } while (0)
#define OUTW(x, y) do { u_int tmp = (y); outw(tmp, (x)); } while (0)
#define OUTL(x, y) do { u_int tmp = (y); outl(tmp, (x)); } while (0)
#define INB(x) __extension__ ({ u_int tmp = (x); inb(tmp); })
#define INW(x) __extension__ ({ u_int tmp = (x); inw(tmp); })
#define INL(x) __extension__ ({ u_int tmp = (x); inl(tmp); })
#else
#define OUTB outb
#define OUTW outw
#define OUTL outl
#define INB  inb
#define INW  inw
#define INL  inl
#endif

#define USAGE "Usage: superiotool [-d] [-e] [-l] [-V] [-v] [-h]\n\n\
  -d | --dump            Dump Super I/O register contents\n\
  -e | --extra-dump      Dump secondary registers too (e.g. EC registers)\n\
  -l | --list-supported  Show the list of supported Super I/O chips\n\
  -V | --verbose         Verbose mode\n\
  -v | --version         Show the superiotool version\n\
  -h | --help            Show a short help text\n\n"

#define USAGE_INFO "\
Per default (no options) superiotool will just probe for a Super I/O\n\
and print its vendor, name, ID, revision, and config port.\n"

#define NOTFOUND "  Failed. Returned data: "

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define EOT		-1		/* End Of Table */
#define NOLDN		-2		/* NO LDN needed */
#define NANA		-3		/* Not Available */
#define RSVD		-4		/* Reserved */
#define MISC		-5		/* Needs special comment in output */
#define MAXLDN		0x14		/* Biggest LDN */
#define LDNSIZE		(MAXLDN + 3)	/* Biggest LDN + 0 + NOLDN + EOT */
#define MAXNUMIDX	170		/* Maximum number of indices */
#define IDXSIZE 	(MAXNUMIDX + 1)
#define MAXNUMPORTS	(6 + 1)		/* Maximum number of Super I/O ports */

/* Select registers for various components. */
#define LDN_SEL		0x07		/* LDN select register */
#define WINBOND_HWM_SEL	0x4e		/* Hardware monitor bank select */

/* Command line parameters. */
extern int dump, verbose, extra_dump;

extern int chip_found;

struct superio_registers {
	int32_t superio_id;		/* Signed, as we need EOT. */
	const char *name;		/* Super I/O name */
	struct {
		int8_t ldn;
		const char *name;	/* LDN name */
		int16_t idx[IDXSIZE];
		int16_t def[IDXSIZE];
	} ldn[LDNSIZE];
};

/* superiotool.c */
uint8_t regval(uint16_t port, uint8_t reg);
void regwrite(uint16_t port, uint8_t reg, uint8_t val);
void enter_conf_mode_winbond_fintek_ite_8787(uint16_t port);
void exit_conf_mode_winbond_fintek_ite_8787(uint16_t port);
int superio_unknown(const struct superio_registers reg_table[], uint16_t id);
const char *get_superio_name(const struct superio_registers reg_table[],
			     uint16_t id);
void dump_superio(const char *name, const struct superio_registers reg_table[],
		  uint16_t port, uint16_t id, uint8_t ldn_sel);
void dump_io(uint16_t iobase, uint16_t length);
void probing_for(const char *vendor, const char *info, uint16_t port);
void print_vendor_chips(const char *vendor,
			const struct superio_registers reg_table[]);

/* ali.c */
void probe_idregs_ali(uint16_t port);
void print_ali_chips(void);

/* fintek.c */
void probe_idregs_fintek(uint16_t port);
void print_fintek_chips(void);

/* ite.c */
void probe_idregs_ite(uint16_t port);
void print_ite_chips(void);

/* nsc.c */
void probe_idregs_nsc(uint16_t port);
void print_nsc_chips(void);

/* smsc.c */
void probe_idregs_smsc(uint16_t port);
void print_smsc_chips(void);

/* winbond.c */
void probe_idregs_winbond(uint16_t port);
void print_winbond_chips(void);

/** Table of which config ports to probe for each Super I/O family. */
static const struct {
	void (*probe_idregs) (uint16_t port);
	int ports[MAXNUMPORTS]; /* Signed, as we need EOT. */
} superio_ports_table[] = {
	{probe_idregs_ali,	{0x3f0, 0x370, EOT}},
	{probe_idregs_fintek,	{0x2e, 0x4e, EOT}},
	/* Only use 0x370 for ITE, but 0x3f0 or 0x3bd would also be valid. */
	{probe_idregs_ite,	{0x2e, 0x4e, 0x370, EOT}},
	{probe_idregs_nsc,	{0x2e, 0x4e, 0x15c, EOT}},
	{probe_idregs_smsc,	{0x2e, 0x4e, 0x162e, 0x164e, 0x3f0, 0x370, EOT}},
	{probe_idregs_winbond,	{0x2e, 0x4e, 0x3f0, 0x370, 0x250, EOT}},
};

/** Table of functions to print out supported Super I/O chips. */
static const struct {
	void (*print_list) (void);
} vendor_print_functions[] = {
	{print_ali_chips},
	{print_fintek_chips},
	{print_ite_chips},
	{print_nsc_chips},
	{print_smsc_chips},
	{print_winbond_chips},
};

#endif
