#define ASSEMBLY 1
#define MAXIMUM_CONSOLE_LOGLEVEL 9
#define DEFAULT_CONSOLE_LOGLEVEL 9

#include <stdint.h>
#include <device/pci_def.h>
#include <cpu/p6/apic.h>
#include <arch/io.h>
#include <device/pnp.h>
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

#define SIO_BASE 0x2e

static void memreset_setup(void)
{
	/* Set the memreset low */
	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 28);
	/* Ensure the BIOS has control of the memory lines */
	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 29);
	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 30);
	outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 31);
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	udelay(800);
	/* Set memreset_high */
	outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 28);
	udelay(90);
}

/*
 * generate_row is specific to board implementation
 *
 */

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
		{ 0x00030101, 0x00010202 },
		{ 0x00010202, 0x00030101 }
	};

	static const unsigned int rows_4p[4][4] = {
		{ 0x00070101, 0x00010202, 0x00030404, 0x00010204 },
		{ 0x00010202, 0x000b0101, 0x00010208, 0x00030808 },
		{ 0x00030808, 0x00010208, 0x000b0101, 0x00010202 },
		{ 0x00010204, 0x00030404, 0x00010202, 0x00070101 }
	};

	if (!(node>=maxnodes || row>=maxnodes)) {
		if (maxnodes==2)
			ret=rows_2p[node][row];
		if (maxnodes==4)
			ret=rows_4p[node][row];
	}

	return ret;
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
#define SMBUS_HUB 0x30
	unsigned hub = device >> 8;
	
	device &= 0xff;
	// smbus_write_byte(SMBUS_HUB, hub);
	return smbus_read_byte(device, address);
	
}

/* no specific code here. this should go away completely */
static void coherent_ht_mainboard(unsigned cpus)
{
}

#include "northbridge/amd/amdk8/cpu_ldtstop.c"
#include "southbridge/amd/amd8111/amd8111_ldtstop.c"

#include "northbridge/amd/amdk8/raminit.c"

#define CONNECTION_0_1 UP
#define CONNECTION_0_2 ACROSS
#define CONNECTION_1_3 DOWN

#include "northbridge/amd/amdk8/coherent_ht.c"
#include "sdram/generic_sdram.c"

#include "resourcemap.c" /* quartet does not want the default */

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

#define PC87360_FDC  0x00
#define PC87360_PP   0x01
#define PC87360_SP2  0x02
#define PC87360_SP1  0x03
#define PC87360_SWC  0x04
#define PC87360_KBCM 0x05
#define PC87360_KBCK 0x06
#define PC87360_GPIO 0x07
#define PC87360_ACB  0x08
#define PC87360_FSCM 0x09
#define PC87360_WDT  0x0A

static void pc87360_enable_serial(void)
{
	pnp_set_logical_device(SIO_BASE, PC87360_SP1);
	pnp_set_enable(SIO_BASE, 1);
	pnp_set_iobase0(SIO_BASE, 0x3f8);
}

#define RC0 (0<<8)
#define RC1 (1<<8)
#define RC2 (2<<8)
#define RC3 (3<<8)

#define DIMM0 0xa0
#define DIMM1 0xa2
#define DIMM2 0xa4
#define DIMM3 0xa8

static void main(void)
{
	static const struct mem_controller cpu[] = {
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x18, 0),
			.f1 = PCI_DEV(0, 0x18, 1),
			.f2 = PCI_DEV(0, 0x18, 2),
			.f3 = PCI_DEV(0, 0x18, 3),
			.channel0 = { RC0|DIMM0, RC0|DIMM2, 0, 0 },
			.channel1 = { RC0|DIMM1, RC0|DIMM3, 0, 0 },
		},
		{
			.node_id = 1,
			.f0 = PCI_DEV(0, 0x19, 0),
			.f1 = PCI_DEV(0, 0x19, 1),
			.f2 = PCI_DEV(0, 0x19, 2),
			.f3 = PCI_DEV(0, 0x19, 3),
			.channel0 = { RC1|DIMM0, RC1|DIMM2, 0, 0 },
			.channel1 = { RC1|DIMM1, RC1|DIMM3, 0, 0 },
		},
		{
			.node_id = 2,
			.f0 = PCI_DEV(0, 0x1a, 0),
			.f1 = PCI_DEV(0, 0x1a, 1),
			.f2 = PCI_DEV(0, 0x1a, 2),
			.f3 = PCI_DEV(0, 0x1a, 3),
			.channel0 = { RC2|DIMM0, RC2|DIMM2, 0, 0 },
			.channel1 = { RC2|DIMM1, RC2|DIMM3, 0, 0 },
		},
		{
			.node_id = 3,
			.f0 = PCI_DEV(0, 0x1b, 0),
			.f1 = PCI_DEV(0, 0x1b, 1),
			.f2 = PCI_DEV(0, 0x1b, 2),
			.f3 = PCI_DEV(0, 0x1b, 3),
			.channel0 = { RC3|DIMM0, RC3|DIMM2, 0, 0 },
			.channel1 = { RC3|DIMM1, RC3|DIMM3, 0, 0 },
		}
	};
	if (cpu_init_detected()) {
		asm("jmp __cpu_reset");
	}
	enable_lapic();
	init_timer();
	if (!boot_cpu()) {
//		notify_bsp_ap_is_stopped();
		stop_this_cpu();
	}
	pc87360_enable_serial();
	uart_init();
	console_init();
	setup_quartet_resource_map();
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
	dump_pci_device(PCI_DEV(0, 0x18, 2));
#endif

	/* Check all of memory */
#if 0
	msr_t msr;
	msr = rdmsr(TOP_MEM);
	print_debug("TOP_MEM: ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\r\n");
#endif
#if 0
	ram_check(0x00000000, msr.lo);
#else
	/* Check 16MB of memory */
	ram_check(0x00000000, 0x01000000);
#endif
}
