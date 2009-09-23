/*****************************************************************************\
 * common.h
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of nvramtool, a utility for reading/writing coreboot
 *  parameters and displaying information from the coreboot table.
 *  For details, see http://coreboot.org/nvramtool.
 *
 *  Please also read the file DISCLAIMER which is included in this software
 *  distribution.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the terms and
 *  conditions of the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
\*****************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
#include <DirectIO/darwinio.h>
#endif
#define OUTB outb
#define OUTW outw
#define OUTL outl
#define INB  inb
#define INW  inw
#define INL  inl
#endif

#define FALSE 0
#define TRUE 1

#define BUG() assert(0)

#define COMMON_RESULT_START 0x10000
#define LAYOUT_RESULT_START 0x20000
#define CMOS_RESULT_START 0x30000
#define CMOS_OP_RESULT_START 0x40000

#define OK 0  /* 0 is used universally to indicate success. */

#define LINE_EOF (COMMON_RESULT_START + 0)
#define LINE_TOO_LONG (COMMON_RESULT_START + 1)

/* basename of this program, as reported by argv[0] */
extern const char prog_name[];

/* version of this program */
extern const char prog_version[];

int get_line_from_file (FILE *f, char line[], int line_buf_size);
void out_of_memory (void);
void usage (FILE *outfile);

#endif  /* COMMON_H */
