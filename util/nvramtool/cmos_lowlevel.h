/*****************************************************************************\
 * cmos_lowlevel.h
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by David S. Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
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

#ifndef NVRAMTOOL_CMOS_LOWLEVEL_H
#define NVRAMTOOL_CMOS_LOWLEVEL_H

#include "common.h"
#include "layout.h"

typedef struct {
	void (*init)(void* data);
	unsigned char (*read)(unsigned addr);
	void (*write)(unsigned addr, unsigned char value);
	void (*set_iopl)(int level);
} cmos_access_t;

typedef enum { HAL_CMOS, HAL_MEMORY } hal_t;
void select_hal(hal_t hal, void *data);

#define CMOS_AREA_OUT_OF_RANGE (CMOS_RESULT_START + 0)
#define CMOS_AREA_OVERLAPS_RTC (CMOS_RESULT_START + 1)
#define CMOS_AREA_TOO_WIDE (CMOS_RESULT_START + 2)

unsigned long long cmos_read(const cmos_entry_t * e);
void cmos_write(const cmos_entry_t * e, unsigned long long value);
unsigned char cmos_read_byte(unsigned index);
void cmos_write_byte(unsigned index, unsigned char value);
void cmos_read_all(unsigned char data[]);
void cmos_write_all(unsigned char data[]);
void set_iopl(int level);
int verify_cmos_op(unsigned bit, unsigned length, cmos_entry_config_t config);

#define CMOS_SIZE 256		/* size of CMOS memory in bytes */
#define CMOS_RTC_AREA_SIZE 14	/* first 14 bytes control real time clock */

/****************************************************************************
 * verify_cmos_byte_index
 *
 * Return 1 if 'index' does NOT specify a valid CMOS memory location.  Else
 * return 0.
 ****************************************************************************/
static inline int verify_cmos_byte_index(unsigned index)
{
	return (index < CMOS_RTC_AREA_SIZE) || (index >= CMOS_SIZE);
}

#endif				/* NVRAMTOOL_CMOS_LOWLEVEL_H */
