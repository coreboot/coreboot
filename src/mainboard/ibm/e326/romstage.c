#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <stdlib.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/amd/amd8111/early_smbus.c"
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/nsc/pc87366/early_serial.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include "southbridge/amd/amd8111/early_ctrl.c"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, PC87366_SP1)

static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {
		/* Set the memreset low. */
		outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 16);
		/* Ensure the BIOS has control of the memory lines. */
		outb((1<<2)|(0<<0), SMBUS_IO_BASE + 0xc0 + 17);
	} else {
		/* Ensure the CPU has control of the memory lines. */
		outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 17);
	}
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);
		/* Set memreset high. */
		outb((1<<2)|(1<<0), SMBUS_IO_BASE + 0xc0 + 16);
		udelay(90);
	}
}

static void activate_spd_rom(const struct mem_controller *ctrl) { }

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/raminit.c"
#include "resourcemap.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/dualcore/dualcore.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const struct mem_controller cpu[] = {
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x18, 0),
			.f1 = PCI_DEV(0, 0x18, 1),
			.f2 = PCI_DEV(0, 0x18, 2),
			.f3 = PCI_DEV(0, 0x18, 3),
			.channel0 = { DIMM0, DIMM2, 0, 0 },
			.channel1 = { DIMM1, DIMM3, 0, 0 },
		},
#if CONFIG_MAX_PHYSICAL_CPUS > 1
		{
			.node_id = 1,
			.f0 = PCI_DEV(0, 0x19, 0),
			.f1 = PCI_DEV(0, 0x19, 1),
			.f2 = PCI_DEV(0, 0x19, 2),
			.f3 = PCI_DEV(0, 0x19, 3),
			.channel0 = { DIMM4, DIMM6, 0, 0 },
			.channel1 = { DIMM5, DIMM7, 0, 0 },
		},
#endif
	};

        int needs_reset;

        if (bist == 0)
		init_cpus(cpu_init_detectedx);

	pc87366_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

        setup_ibm_e326_resource_map();

	needs_reset = setup_coherent_ht_domain();

#if CONFIG_LOGICAL_CPUS
        // It is said that we should start core1 after all core0 launched
        start_other_cores();
#endif
        // automatically set that for you, but you might meet tight space
        needs_reset |= ht_setup_chains_x();

       	if (needs_reset) {
               	print_info("ht reset -\n");
               	soft_reset();
       	}

	enable_smbus();

	memreset_setup();
	sdram_initialize(ARRAY_SIZE(cpu), cpu);

	post_cache_as_ram();
}
