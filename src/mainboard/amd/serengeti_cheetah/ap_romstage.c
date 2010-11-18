#define ASSEMBLY 1
#define __PRE_RAM__

#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include "pc80/serial.c"
#include "./arch/i386/lib/printk_init.c"

#include "console/console.c"
#include "lib/uart8250.c"
#include "console/vtxprintf.c"

#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"

#include "lib/delay.c"

#include "northbridge/amd/amdk8/reset_test.c"

#include "northbridge/amd/amdk8/debug.c"

#include "southbridge/amd/amd8111/amd8111_early_ctrl.c"

#include "northbridge/amd/amdk8/amdk8_f.h"

#include "cpu/x86/mtrr.h"
#include "cpu/amd/mtrr.h"
#include "cpu/x86/tsc.h"

#include "northbridge/amd/amdk8/amdk8_f_pci.c"
#include "northbridge/amd/amdk8/raminit_f_dqs.c"

static inline unsigned get_nodes(void)
{
	return ((pci_read_config32(PCI_DEV(0, 0x18, 0), 0x60)>>4) & 7) + 1;
}

#include "cpu/amd/dualcore/dualcore.c"

void hardwaremain(int ret_addr)
{
	struct sys_info *sysinfo = (CONFIG_DCACHE_RAM_BASE +
			CONFIG_DCACHE_RAM_SIZE -
			CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in CACHE
	struct sys_info *sysinfox = ((CONFIG_RAMTOP) -
			CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in RAM

	struct node_core_id id;

	id = get_node_core_id_x();

        printk(BIOS_DEBUG, "CODE IN CACHE ON NODE: %02x\n", id.nodeid);

	train_ram(id.nodeid, sysinfo, sysinfox);

	/*
	 * go back, but can not use stack any more, because we 
	 * only keep ret_addr and can not restore esp, and ebp.
	 */

        __asm__ volatile (
                "movl  %0, %%edi\n\t"
                "jmp     *%%edi\n\t"
                :: "a"(ret_addr)
        );
}

#include <arch/registers.h>

void x86_exception(struct eregs *info)
{
        do {
                hlt();
        } while(1);
}

