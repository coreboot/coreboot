/* the standard linuxbios include file has constant definitions, types and so on */
#include <linuxbios.h>

/* support library code. */
src/LinuxBIOSv2/src/cpu/x86/lapic/boot_cpu.c -- which cpu is the boot cpu
src/LinuxBIOSv2/src/northbridge/amd/amdk8/reset_test.c  -- determine if we had a reset -- hard or soft
src/LinuxBIOSv2/src/cpu/amd/mtrr/amd_earlymtrr.c -- early mtrr setup

/* this is currently used but may be replaced by properties for the CPUs */
northbridge/amd/amdk8/setup_resource_map.c -- map of 18.1 device for routing

#define CK804_NUM 1
#include "southbridge/nvidia/ck804/ck804_early_setup_ss.h"
//set GPIO to input mode
#define CK804_MB_SETUP \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+15, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* M8,GPIO16, PCIXA_PRSNT2_L*/ \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+44, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* P5,GPIO45, PCIXA_PRSNT1_L*/ \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+16, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* K4,GPIO17, PCIXB_PRSNT1_L*/ \
                RES_PORT_IO_8, SYSCTRL_IO_BASE + 0xc0+45, ~(0xff), ((0<<4)|(0<<2)|(0<<0)),/* P7,GPIO46, PCIXB_PRSNT2_L*/ \


/* there is a global struct used by main, that is the dtb tree */
/* it is built when the linuxbios image is built */
/* it is linked in, as it is generated as a C struct */
extern struct dtb *dtb;

/* assumptions: when we get here, we have a small region of cache-as-ram usable as a stack. 
  * we have the DTB in flash. bist (built-in-self-test) and cpu_init_detectedx are set by CAR code. 
  */
/* what we have to do:
  * enable console(s)
  * make the processors sane
  * figure out what memory is there and turn it on
  * do initial hardware enable
  */
void main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct property *spd;
	int spdsize;
	struct property *uart;
	u16 *spd_addr;
	struct property *image;
	struct property *uart;

	int needs_reset;
	unsigned bsp_apicid = 0;

	struct mem_controller ctrl[8];
	unsigned nodes;
	unsigned last_boot_normal_x = last_boot_normal(); /* from CMOS */

	/* fill in the SPD entries from the properties. 
	  * the SPD properties are an array of shorts
	  */
	spd = get_property(dtb, "spd");
	if (! spd)		/* now what? */
		fatal("no SPD properties");

	spdsize = spd->val.len / sizeof(uint16);

	spd_addr = malloc(spdsize * sizeof(*spd_addr));

	for(I = 0; i < spdsize; i++)
		spd_addr[i] = be16_to_cpu(*((u32 *)(d.val+i))));

	/* we now have the spd addresses from the DTB */


	/* There are several ways we could be here. We could be power-on reset, 
	  * in which case we have to init a lot of things. We could be cpu-only reset, 
	  * in which case we just have to clean up the cpu. We could be the 
	  * Attached Processor (AP), in which case it is a lot like a cpu-only reset, 
	  * since a lot of the setup has been done by the Boot Strap Processor (BSP or BP)
	  */

	if (bist == 0) {
		init_cpus(cpu_init_detectedx);
	}

	uart = get_property(dtb, "uart");
	w83627hf_enable_serial(uart);
	uart_init(uart);
	console_init(uart);

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	setup_s2892_resource_map(dtb);
#if 0
	dump_pci_device(PCI_DEV(0, 0x18, 0));
	dump_pci_device(PCI_DEV(0, 0x19, 0));
#endif

	needs_reset = setup_coherent_ht_domain(dtb);

	wait_all_core0_started(dtb);
	/* this should be determined from dtb. */
	numcpus = get_value(dtb, "#cpus");
#if CONFIG_LOGICAL_CPUS==1
	// It is said that we should start core1 after all core0 launched
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif

	needs_reset |= ht_setup_chains_x(dtb);

	needs_reset |= ck804_early_setup_x(dtb);

	if (needs_reset) {
	print_info("ht reset -\r\n");
	soft_reset();
	}

	allow_all_aps_stop(bsp_apicid);

	nodes = get_nodes(dtb);
	//It's the time to set ctrl now;
	fill_mem_ctrl(nodes, ctrl, spd_addr);

	enable_smbus(dtb);
#if 0
	dump_spd_registers(&cpu[0]);
#endif
#if 0
	dump_smbus_registers();
#endif

	memreset_setup(dtb);
	sdram_initialize(dtb, nodes, ctrl);

#if 0
	print_pci_devices();
#endif

#if 0
	dump_pci_devices();
#endif

	post_cache_as_ram();
}
