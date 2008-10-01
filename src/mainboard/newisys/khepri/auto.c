#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include <stdlib.h>
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
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/amd/mtrr/amd_earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

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

static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {
		/* Set the memreset low */
		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 28);
		/* Ensure the BIOS has control of the memory lines */
		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 29);
	}
	else {
		/* Ensure the CPU has controll of the memory lines */
		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 29);
	}
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);
		/* Set memreset_high */
		outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 28);
		udelay(90);
	}
}

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
	/* nothing to do */
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/raminit.c"

#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/incoherent_ht.c"

#include "sdram/generic_sdram.c"

/* newisys khepri does not want the default */
#include "resourcemap.c"
#include "cpu/amd/dualcore/dualcore.c"

#define NODE_RAM(x)                     \
	.node_id = 0+x,                 \
	.f0 = PCI_DEV(0, 0x18+x, 0),    \
	.f1 = PCI_DEV(0, 0x18+x, 1),    \
	.f2 = PCI_DEV(0, 0x18+x, 2),    \
	.f3 = PCI_DEV(0, 0x18+x, 3)

static void main(unsigned long bist)
{
	static const struct mem_controller cpu[] = {
		{
			NODE_RAM(0),
			.channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
			.channel1 = { (0xa<<3)|1, (0xa<<3)|3, 0, 0 },
		},
		{
			NODE_RAM(1),
			.channel0 = { (0xa<<3)|4, (0xa<<3)|6, 0, 0 },
			.channel1 = { (0xa<<3)|5, (0xa<<3)|7, 0, 0 },
		},
	};

	int needs_reset;
        unsigned nodeid;

	if (bist == 0) {
	    	k8_init_and_stop_secondaries();
	}
	/* Setup the console */
        w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	setup_khepri_resource_map();
	needs_reset = setup_coherent_ht_domain();
	needs_reset=ht_setup_chains_x();

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
	sdram_initialize(ARRAY_SIZE(cpu), cpu);

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
