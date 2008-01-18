/*****************************************************************************\
 * layout_file.h
 * $Id: layout_file.h,v 1.2 2005/12/16 22:45:49 dsp_llnl Exp $
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of lxbios, a utility for reading/writing coreboot
 *  parameters and displaying information from the coreboot table.
 *  For details, see <http://www.llnl.gov/linux/lxbios/>.
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

#ifndef LXBIOS_LAYOUT_FILE_H
#define LXBIOS_LAYOUT_FILE_H

#include "common.h"
#include "coreboot_tables.h"

void set_layout_filename (const char filename[]);
void get_layout_from_file (void);
void write_cmos_layout (FILE *f);

#endif  /* LXBIOS_LAYOUT_FILE_H */
