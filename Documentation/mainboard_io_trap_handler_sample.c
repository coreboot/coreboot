/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright 2013 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/pm.h>
#include <soc/smm.h>
#include <elog.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/smm.h>
#include "ec.h"
#include "gpio.h"

int mainboard_io_trap_handler(int smif)
{
       switch (smif) {
       case 0x99:
               printk(BIOS_DEBUG, "Sample\n");
               smm_get_gnvs()->smif = 0;
               break;
       default:
               return 0;
       }

       /* On success, the IO Trap Handler returns 0
        * On failure, the IO Trap Handler returns a value != 0
        *
        * For now, we force the return value to 0 and log all traps to
        * see what's going on.
        */
       //smm_get_gnvs()->smif = 0;
       return 1;
}
