/*****************************************************************************\
 * cmos_lowlevel.h
 * $Id$
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
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
\*****************************************************************************/

#ifndef NVRAMTOOL_CMOS_LOWLEVEL_H
#define NVRAMTOOL_CMOS_LOWLEVEL_H

#include "common.h"

#define CMOS_AREA_OUT_OF_RANGE (CMOS_RESULT_START + 0)
#define CMOS_AREA_OVERLAPS_RTC (CMOS_RESULT_START + 1)
#define CMOS_AREA_TOO_WIDE (CMOS_RESULT_START + 2)

unsigned long long cmos_read (unsigned bit, unsigned length);
void cmos_write (unsigned bit, unsigned length, unsigned long long value);
unsigned char cmos_read_byte (unsigned index);
void cmos_write_byte (unsigned index, unsigned char value);
void cmos_read_all (unsigned char data[]);
void cmos_write_all (unsigned char data[]);
void set_iopl (int level);
int verify_cmos_op (unsigned bit, unsigned length);

#define CMOS_SIZE 256  /* size of CMOS memory in bytes */
#define CMOS_RTC_AREA_SIZE 14  /* first 14 bytes control real time clock */

/****************************************************************************
 * verify_cmos_byte_index
 *
 * Return 1 if 'index' does NOT specify a valid CMOS memory location.  Else
 * return 0.
 ****************************************************************************/
static inline int verify_cmos_byte_index (unsigned index)
 { return (index < CMOS_RTC_AREA_SIZE) || (index >= CMOS_SIZE); }

#endif  /* NVRAMTOOL_CMOS_LOWLEVEL_H */
