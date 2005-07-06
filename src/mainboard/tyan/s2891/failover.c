#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "pc80/mc146818rtc_early.c"

#include "southbridge/nvidia/ck804/ck804_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"

static void sio_setup(void)
{
        
        unsigned value;
	uint32_t dword;
	uint8_t byte;

        byte = pci_read_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0x7b);
        byte |= 0x20; 
        pci_write_config8(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0x7b, byte);
	
	dword = pci_read_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa0);
	dword |= (1<<0) | (1<<1);  
	pci_write_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa0, dword);

#if 1
        dword = pci_read_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa4);
        dword |= (1<<16);  
        pci_write_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa4, dword);

#endif

}


static unsigned long main(unsigned long bist)
{
        unsigned nodeid;
        /* Make cerain my local apic is useable */
        enable_lapic();

        nodeid = lapicid();
        /* Is this a cpu only reset? */
        if (cpu_init_detected(nodeid)) {
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

	sio_setup();

	/* Setup the ck804 */
	ck804_enable_rom();

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
	//CPU reset will reset memtroller ???
	asm volatile ("jmp __cpu_reset" 
		: /* outputs */ 
		: "a"(bist) /* inputs */
		: /* clobbers */
		);
#endif

 fallback_image:
	return bist;
}
