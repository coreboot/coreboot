/*****************************************************************************\
 * layout_file.h
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
\*****************************************************************************/

#ifndef LAYOUT_FILE_H
#define LAYOUT_FILE_H

#include "common.h"
#include "coreboot_tables.h"

void set_layout_filename(const char filename[]);
void get_layout_from_file(void);
void write_cmos_layout(FILE * f);
void write_cmos_output_bin(const char *binary_filename);
void write_cmos_layout_header(const char *header_filename);
extern int is_ident(char *str);

#endif				/* LAYOUT_FILE_H */
