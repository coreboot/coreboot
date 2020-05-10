/* SPDX-License-Identifier: GPL-2.0-only */

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
