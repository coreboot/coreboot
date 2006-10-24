#define ASSEMBLY 1
//#define MAXIMUM_CONSOLE_LOGLEVEL 9
//#define DEFAULT_CONSOLE_LOGLEVEL 9

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "southbridge/amd/amd8111/amd8111_early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include <cpu/amd/model_fxx_rev.h>
#include "superio/nsc/pc87360/pc87360_early_serial.c"
#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, PC87360_SP1)

static void hard_reset(void)
{
	set_bios_reset();
	/* enable cf9 */
	pci_write_config8(PCI_DEV(0, 0x04, 3), 0x41, 0xf1);
	/* reset */
	outb(0x0e, 0x0cf9);
}

static void soft_reset(void)
{
	set_bios_reset();
	pci_write_config8(PCI_DEV(0, 0x04, 0), 0x47, 1);
}

/*
 * GPIO28 of 8111 will control H0_MEMRESET_L
 * GPIO29 of 8111 will control H1_MEMRESET_L
 */
static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {
		/* Set the memreset low */
		outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) |
		     (0 << 0), SMBUS_IO_BASE + 0xc0 + 28);
		/* Ensure the BIOS has control of the memory lines */
		outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) |
		     (0 << 0), SMBUS_IO_BASE + 0xc0 + 29);
	} else {
		/* Ensure the CPU has controll of the memory lines */
		outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) |
		     (1 << 0), SMBUS_IO_BASE + 0xc0 + 29);
	}
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);
		/* Set memreset_high */
		outb((0 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (1 << 2) |
		     (1 << 0), SMBUS_IO_BASE + 0xc0 + 28);
		udelay(90);
	}
}

/*
 * generate_row is specific to board implementation
 *
 */

static unsigned int generate_row(uint8_t node, uint8_t row,
				 uint8_t maxnodes)
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

	uint32_t ret = 0x00010101;	/* default row entry */
/*
	static const unsigned int rows_2p[2][2] = {
		{0x00030101, 0x00010202},
		{0x00010202, 0x00030101}
	};
*/

	static const unsigned int rows_4p[4][4] = {
		{0x00070101, 0x00010202, 0x00030404, 0x00010204},
		{0x00010202, 0x000b0101, 0x00010208, 0x00030808},
		{0x00030808, 0x00010208, 0x000b0101, 0x00010202},
		{0x00010204, 0x00030404, 0x00010202, 0x00070101}
	};

	if (!(node >= maxnodes || row >= maxnodes)) {
/*
		if (maxnodes == 2)
			ret = rows_2p[node][row];
		if (maxnodes == 4)
*/
			ret = rows_4p[node][row];
	}

	return ret;
}


#if ( FAKE_SPDROM != 1 )
static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
#define SMBUS_HUB 0x18
	unsigned device = (ctrl->channel0[0]) >> 8;
	smbus_write_byte(SMBUS_HUB, 0x01, device);
	smbus_write_byte(SMBUS_HUB, 0x03, 0);
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device & 0xff, address);
}
#else
#include "fakespd.c"
#endif

// #include "northbridge/amd/amdk8/setup_resource_map.c"
#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "cpu/amd/dualcore/dualcore.c"

#include "sdram/generic_sdram.c"
/* quartet does not want the default */
#include "resourcemap.c"

#define RC0 ((1<<1)<<8)
#define RC1 ((1<<2)<<8)
#define RC2 ((1<<3)<<8)
#define RC3 ((1<<4)<<8)

#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53

static void main(unsigned long bist)
{
	static const struct mem_controller cpu[] = {
		{
		 .node_id = 0,
		 .f0 = PCI_DEV(0, 0x18, 0),
		 .f1 = PCI_DEV(0, 0x18, 1),
		 .f2 = PCI_DEV(0, 0x18, 2),
		 .f3 = PCI_DEV(0, 0x18, 3),
		 .channel0 = {RC0 | DIMM0, RC0 | DIMM2, 0, 0},
		 .channel1 = {RC0 | DIMM1, RC0 | DIMM3, 0, 0},
		 },
		{
		 .node_id = 1,
		 .f0 = PCI_DEV(0, 0x19, 0),
		 .f1 = PCI_DEV(0, 0x19, 1),
		 .f2 = PCI_DEV(0, 0x19, 2),
		 .f3 = PCI_DEV(0, 0x19, 3),
		 .channel0 = {RC1 | DIMM0, RC1 | DIMM2, 0, 0},
		 .channel1 = {RC1 | DIMM1, RC1 | DIMM3, 0, 0},
		 },
		{
		 .node_id = 2,
		 .f0 = PCI_DEV(0, 0x1a, 0),
		 .f1 = PCI_DEV(0, 0x1a, 1),
		 .f2 = PCI_DEV(0, 0x1a, 2),
		 .f3 = PCI_DEV(0, 0x1a, 3),
		 .channel0 = {RC2 | DIMM0, RC2 | DIMM2, 0, 0},
		 .channel1 = {RC2 | DIMM1, RC2 | DIMM3, 0, 0},
		 },
		{
		 .node_id = 3,
		 .f0 = PCI_DEV(0, 0x1b, 0),
		 .f1 = PCI_DEV(0, 0x1b, 1),
		 .f2 = PCI_DEV(0, 0x1b, 2),
		 .f3 = PCI_DEV(0, 0x1b, 3),
		 .channel0 = {RC3 | DIMM0, RC3 | DIMM2, 0, 0},
		 .channel1 = {RC3 | DIMM1, RC3 | DIMM3, 0, 0},
		 }
	};

	int needs_reset;
	unsigned nodeid;

	if (bist == 0) {
	    	k8_init_and_stop_secondaries();
	}
	/* Setup the console */
	pc87360_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	setup_quartet_resource_map();
	needs_reset = setup_coherent_ht_domain();
#if 0
        needs_reset |= ht_setup_chains(2);
#else
	needs_reset |= ht_setup_chains_x();
#endif
	if (needs_reset) {
		print_info("ht reset -\r\n");
		soft_reset();
	}
#if 0
	print_pci_devices();
#endif
	enable_smbus();
#if 0
	dump_spd_registers(&cpu[0]);
#endif
	memreset_setup();
	sdram_initialize(sizeof(cpu) / sizeof(cpu[0]), cpu);

#if 0
	dump_pci_devices();
#endif
#if 0
	dump_pci_device(PCI_DEV(0, 0x18, 2));
#endif
#if 0
	/* Check the first 1M */
	ram_check(0x00000000, 0x000100000);
#endif
}
