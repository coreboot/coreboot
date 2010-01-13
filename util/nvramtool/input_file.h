/*****************************************************************************\
 * input_file.h
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

#ifndef INPUT_FILE_H
#define INPUT_FILE_H

#include "common.h"
#include "layout.h"

typedef struct cmos_write_t cmos_write_t;

/* This represents a pending CMOS write operation.  When changing
 * multiple CMOS parameter values, we first represent the changes as a
 * list of pending write operations.  This allows us to sanity check all
 * write operations before any of them are performed.
 */
struct cmos_write_t {
	unsigned bit;
	unsigned length;
	cmos_entry_config_t config;
	unsigned long long value;
	cmos_write_t *next;
};

cmos_write_t *process_input_file(FILE * f);
void do_cmos_writes(cmos_write_t * list);

extern const char assignment_regex[];

#endif				/* INPUT_FILE_H */
