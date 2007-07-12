/*****************************************************************************\
 * reg_expr.c
 * $Id: reg_expr.c,v 1.2 2005/12/16 22:45:49 dsp_llnl Exp $
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of lxbios, a utility for reading/writing LinuxBIOS
 *  parameters and displaying information from the LinuxBIOS table.
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

#include <stdarg.h>
#include "common.h"
#include "reg_expr.h"

/****************************************************************************
 * compile_reg_exprs
 *
 * Compile a bunch of regular expressions.
 ****************************************************************************/
void compile_reg_exprs (int cflags, int num_exprs,
                        /* const char *expr1, regex_t *reg1, */ ...)
 { static const size_t ERROR_BUF_SIZE = 256;
   char error_msg[ERROR_BUF_SIZE];
   va_list ap;
   regex_t *reg;
   const char *expr;
   int i, result;

   va_start(ap, num_exprs);

   for (i = 0; i < num_exprs; i++)
    { expr = va_arg(ap, const char *);
      reg  = va_arg(ap, regex_t *);

      if ((result = regcomp(reg, expr, cflags)) != 0)
       { regerror(result, reg, error_msg, ERROR_BUF_SIZE);
         fprintf(stderr, "%s: %s\n", prog_name, error_msg);
         exit(1);
       }
    }

   va_end(ap);
 }

/****************************************************************************
 * free_reg_exprs
 *
 * Destroy a bunch of previously compiled regular expressions.
 ****************************************************************************/
void free_reg_exprs (int num_exprs, /* regex_t *reg1, */ ...)
 { va_list ap;
   int i;

   va_start(ap, num_exprs);

   for (i = 0; i < num_exprs; i++)
      regfree(va_arg(ap, regex_t *));

   va_end(ap);
 }
