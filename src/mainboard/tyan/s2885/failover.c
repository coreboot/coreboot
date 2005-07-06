#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "pc80/mc146818rtc_early.c"
#include "southbridge/amd/amd8111/amd8111_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"

#if CONFIG_LOGICAL_CPUS==1
#include "cpu/amd/dualcore/dualcore_id.c"
#else
#include "cpu/amd/model_fxx/node_id.c"
#endif

static unsigned long main(unsigned long bist)
{
#if CONFIG_LOGICAL_CPUS==1
        struct node_core_id id;
#else
	unsigned nodeid;
#endif	
	/* Make cerain my local apic is useable */
//	enable_lapic();

#if CONFIG_LOGICAL_CPUS==1
        id = get_node_core_id_x();
	/* Is this a cpu only reset? */
        if (cpu_init_detected(id.nodeid)) {
#else
//        nodeid = lapicid() & 0xf;
	nodeid = get_node_id();
	/* Is this a cpu only reset? */
        if (cpu_init_detected(nodeid)) {
#endif
	/* Is this a cpu only reset? */
		if (last_boot_normal()) {
			goto normal_image;
		} else {
			goto cpu_reset;
		}
	}
	/* Is this a secondary cpu? */
	if (!boot_cpu()) {
		if (last_boot_normal()) {
			goto normal_image;
		} else {
			goto fallback_image;
		}
	}
	
	/* Nothing special needs to be done to find bus 0 */
	/* Allow the HT devices to be found */
	enumerate_ht_chain();
	
	/* Setup the 8111 */
	amd8111_enable_rom();

	/* Is this a deliberate reset by the bios */
	if (bios_reset_detected() && last_boot_normal()) {
		goto normal_image;
	}
	/* This is the primary cpu how should I boot? */
	else if (do_normal_boot()) {
		goto normal_image;
	}
	else {
		goto fallback_image;
	}
 normal_image:
	asm volatile ("jmp __normal_image" 
		: /* outputs */ 
		: "a" (bist) /* inputs */
		: /* clobbers */
		);
 cpu_reset:
#if 0
	asm volatile ("jmp __cpu_reset"
		: /* outputs */ 
		: "a"(bist) /* inputs */
		: /* clobbers */
		);
#endif
 fallback_image:
	return bist;
}
