/*****************************************************************************\
 * opts.h
 * $Id: opts.h,v 1.2 2005/12/16 22:45:49 dsp_llnl Exp $
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

#ifndef LXBIOS_OPTS_H
#define LXBIOS_OPTS_H

#include "common.h"

typedef enum
 { LXBIOS_OP_SHOW_VERSION = 0,
   LXBIOS_OP_SHOW_USAGE,
   LXBIOS_OP_LBTABLE_SHOW_INFO,
   LXBIOS_OP_LBTABLE_DUMP,
   LXBIOS_OP_SHOW_PARAM_VALUES,
   LXBIOS_OP_CMOS_SHOW_ONE_PARAM,
   LXBIOS_OP_CMOS_SHOW_ALL_PARAMS,
   LXBIOS_OP_CMOS_SET_ONE_PARAM,
   LXBIOS_OP_CMOS_SET_PARAMS_STDIN,
   LXBIOS_OP_CMOS_SET_PARAMS_FILE,
   LXBIOS_OP_CMOS_CHECKSUM,
   LXBIOS_OP_SHOW_LAYOUT,
   LXBIOS_OP_WRITE_CMOS_DUMP,
   LXBIOS_OP_READ_CMOS_DUMP,
   LXBIOS_OP_SHOW_CMOS_HEX_DUMP,
   LXBIOS_OP_SHOW_CMOS_DUMPFILE
 }
lxbios_op_t;

typedef struct
 { lxbios_op_t op;
   char *param;
 }
lxbios_op_info_t;

typedef enum
 { LXBIOS_MOD_SHOW_VALUE_ONLY = 0,
   LXBIOS_MOD_USE_CMOS_LAYOUT_FILE,
   LXBIOS_MOD_USE_CMOS_OPT_TABLE,
   LXBIOS_NUM_OP_MODIFIERS  /* must always be last */
 }
lxbios_op_modifier_t;

typedef struct
 { int found;
   int found_seq;
   char *param;
 }
lxbios_op_modifier_info_t;

extern lxbios_op_info_t lxbios_op;

extern lxbios_op_modifier_info_t lxbios_op_modifiers[];

void parse_lxbios_args (int argc, char *argv[]);

#endif  /* LXBIOS_OPTS_H */
