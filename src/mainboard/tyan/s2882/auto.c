#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <cpu/p6/apic.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/amd/amdk8/early_ht.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/k8/apic_timer.c"
#include "lib/delay.c"
#include "cpu/p6/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "debug.c"
#include "northbridge/amd/amdk8/cpu_rev.c"
 

static void memreset_setup(void)
{
	/* Set the memreset low */
//	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 28);
	/* Ensure the BIOS has control of the memory lines */
//	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 29);
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	udelay(800);
	/* Set memreset_high */
//	outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 28);
	udelay(90);
}

static unsigned int generate_row(uint8_t node, uint8_t row, uint8_t maxnodes)
{
	/* Routing Table Node i 
	 *
	 * F0: 0x40, 0x44, 0x48, 0x4c, 0x50, 0x54, 0x58, 0x5c 
	 *  i:    0,    1,    2,    3,    4,    5,    6,    7
	 *
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [11: 8] Response Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [19:16] Broadcast route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */

	uint32_t ret=0x00010101; /* default row entry */

	static const unsigned int rows_2p[2][2] = {
		{ 0x00050101, 0x00010404 },
		{ 0x00010404, 0x00050101 }
	};

	if(maxnodes>2) {
		print_debug("this mainboard is only designed for 2 cpus\r\n");
		maxnodes=2;
	}


	if (!(node>=maxnodes || row>=maxnodes)) {
		ret=rows_2p[node][row];
	}

	return ret;
}

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
	/* nothing to do */
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

/* include mainboard specific ht code */
#include "hypertransport.c"

//#include "northbridge/amd/amdk8/cpu_ldtstop.c"
//#include "southbridge/amd/amd8111/amd8111_ldtstop.c"

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

static void enable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(0x1b);
	msr.hi &= 0xffffff00;
	msr.lo &= 0x000007ff;
	msr.lo |= APIC_DEFAULT_BASE | (1 << 11);
	wrmsr(0x1b, msr);
}

static void stop_this_cpu(void)
{
	unsigned apicid;
	apicid = apic_read(APIC_ID) >> 24;

	/* Send an APIC INIT to myself */
	apic_write(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write(APIC_ICR, APIC_INT_LEVELTRIG | APIC_INT_ASSERT | APIC_DM_INIT);
	/* Wait for the ipi send to finish */
	apic_wait_icr_idle();

	/* Deassert the APIC INIT */
	apic_write(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write(APIC_ICR,  APIC_INT_LEVELTRIG | APIC_DM_INIT);
	/* Wait for the ipi send to finish */
	apic_wait_icr_idle();

	/* If I haven't halted spin forever */
	for(;;) {
		hlt();
	}
}
#define FIRST_CPU  1
#define SECOND_CPU 1
#define TOTAL_CPUS (FIRST_CPU + SECOND_CPU)
static void main(void)
{
	/*
	 * GPIO28 of 8111 will control H0_MEMRESET_L
	 * GPIO29 of 8111 will control H1_MEMRESET_L
	 */
	static const struct mem_controller cpu[] = {
#if FIRST_CPU
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x18, 0),
			.f1 = PCI_DEV(0, 0x18, 1),
			.f2 = PCI_DEV(0, 0x18, 2),
			.f3 = PCI_DEV(0, 0x18, 3),
			.channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
			.channel1 = { (0xa<<3)|1, (0xa<<3)|3, 0, 0 },
		},
#endif
#if SECOND_CPU
		{
			.node_id = 1,
			.f0 = PCI_DEV(0, 0x19, 0),
			.f1 = PCI_DEV(0, 0x19, 1),
			.f2 = PCI_DEV(0, 0x19, 2),
			.f3 = PCI_DEV(0, 0x19, 3),
			.channel0 = { (0xa<<3)|4, (0xa<<3)|6, 0, 0 },
			.channel1 = { (0xa<<3)|5, (0xa<<3)|7, 0, 0 },
		},
#endif
	};
	if (cpu_init_detected()) {
		asm("jmp __cpu_reset");
	}
	enable_lapic();
	init_timer();
	if (!boot_cpu() ) {
		notify_bsp_ap_is_stopped();
		stop_this_cpu();
	}
	uart_init();
	console_init();
	setup_default_resource_map();
	setup_coherent_ht_domain();
	enumerate_ht_chain(0);
	distinguish_cpu_resets(0);
	
#if 0
	print_pci_devices();
#endif
	enable_smbus();
#if 0
	dump_spd_registers(&cpu[0]);
#endif
	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

#if 0
	dump_pci_devices();
#endif
#if 0
	dump_pci_device(PCI_DEV(0, 0x18, 1));
#endif

	/* Check all of memory */
#if 0
	msr_t msr;
	msr = rdmsr(TOP_MEM2);
	print_debug("TOP_MEM2: ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\r\n");
#endif
/*
#if  0
	ram_check(0x00000000, msr.lo+(msr.hi<<32));
#else
#if TOTAL_CPUS < 2
	// Check 16MB of memory @ 0
	ram_check(0x00000000, 0x01000000);
#else
	// Check 16MB of memory @ 2GB 
	ram_check(0x80000000, 0x81000000);
#endif
#endif
*/
}
