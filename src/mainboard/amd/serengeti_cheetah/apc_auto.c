#define ASSEMBLY 1
#define __ROMCC__

#define RAMINIT_SYSINFO 1
#define CACHE_AS_RAM_ADDRESS_DEBUG 0

#define SET_NB_CFG_54 1 

//used by raminit
#define QRANK_DIMM_SUPPORT 1

#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "./arch/i386/lib/printk_init.c"

#if CONFIG_USE_INIT == 0
	#include "lib/memcpy.c"
#endif

#include "arch/i386/lib/console.c"
#include "lib/uart8250.c"
#include "console/vtxprintf.c"

#if 0 
static void post_code(uint8_t value) {
#if 1
        int i;
        for(i=0;i<0x80000;i++) {
                outb(value, 0x80);
        }
#endif
}
#endif

#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"

#include "lib/delay.c"


//#include "cpu/x86/lapic/boot_cpu.c"
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
	struct sys_info *sysinfo = (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in CACHE
        struct sys_info *sysinfox = ((CONFIG_LB_MEM_TOPK<<10) - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in RAM

	struct node_core_id id;

	id = get_node_core_id_x();

#if CONFIG_USE_PRINTK_IN_CAR
        printk_debug("CODE IN CACHE ON NODE: %02x\n");
#else
        print_debug("CODE IN CACHE ON NODE:"); print_debug_hex8(id.nodeid); print_debug("\r\n");
#endif

	train_ram(id.nodeid, sysinfo, sysinfox);

	/*
		go back, but can not use stack any more, because we only keep ret_addr and can not restore esp, and ebp
	*/

        __asm__ volatile (
                "movl  %0, %%edi\n\t"
                "jmp     *%%edi\n\t"
                :: "a"(ret_addr)
        );



}
struct eregs {
        uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
        uint32_t vector;
        uint32_t error_code;
        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
};

void x86_exception(struct eregs *info)
{
        do {
                hlt();
        } while(1);
}


