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

static void memreset_setup(void)
{
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
}

static unsigned int generate_row(uint8_t node, uint8_t row, uint8_t maxnodes)
{
	/* since the AMD Solo is a UP only machine, we can 
	 * always return the default row entry value
	 */
	return 0x00010101; /* default row entry */
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

/* no specific code here. this should go away completely */
static void coherent_ht_mainboard(unsigned cpus)
{
}

#include "northbridge/amd/amdk8/cpu_ldtstop.c"
#include "southbridge/amd/amd8111/amd8111_ldtstop.c"

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

static void main(void)
{
	/*
	 * GPIO28 of 8111 will control H0_MEMRESET_L
	 */
	static const struct mem_controller cpu[] = {
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x18, 0),
			.f1 = PCI_DEV(0, 0x18, 1),
			.f2 = PCI_DEV(0, 0x18, 2),
			.f3 = PCI_DEV(0, 0x18, 3),
			.channel0 = { (0xa<<3), (0xa<<3)|1, 0, 0 },
			.channel1 = { 0, 0, 0, 0 },
		}
	};

	if (cpu_init_detected()) {
		asm("jmp __cpu_reset");
	}

	enable_lapic();
	init_timer();

	if (!boot_cpu()) {
		notify_bsp_ap_is_stopped();
		stop_this_cpu();
	}

	uart_init();
	console_init();
	setup_default_resource_map();
	setup_coherent_ht_domain();
	enumerate_ht_chain(0);
	distinguish_cpu_resets(0);
	
	enable_smbus();

	memreset_setup();
	sdram_initialize(1, cpu);

	msr_t msr;
	msr = rdmsr(TOP_MEM);
	print_debug("TOP_MEM: ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\r\n");

	ram_check(0x00000000, msr.lo);
	
}
