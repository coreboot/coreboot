#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <stdlib.h>
#include <console/console.h>
#include "southbridge/intel/i82801ex/early_smbus.c"
#include "northbridge/intel/e7520/raminit.h"
#include "superio/winbond/w83627hf/w83627hf.h"
#include "cpu/x86/lapic/boot_cpu.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "debug.c"
#include "watchdog.c"
#include "southbridge/intel/i82801ex/reset.c"
#include "superio/winbond/w83627hf/early_serial.c"
#include "northbridge/intel/e7520/memory_initialized.c"
#include "cpu/x86/bist.h"
#include <spd.h>

#define CONSOLE_SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)
#define HIDDEN_SERIAL_DEV  PNP_DEV(0x2e, W83627HF_SP2)
#define DUMMY_DEV PNP_DEV(0x2e, 0)

#define DEVPRES_CONFIG  ( \
	DEVPRES_D0F0 | \
	DEVPRES_D1F0 | \
	DEVPRES_D2F0 | \
	DEVPRES_D3F0 | \
	DEVPRES_D4F0 | \
	DEVPRES_D6F0 | \
	0 )
#define DEVPRES1_CONFIG (DEVPRES1_D0F1 | DEVPRES1_D8F0)

static void mch_reset(void) {}
static void mainboard_set_e7520_pll(unsigned bits) {}
static void mainboard_set_e7520_leds(void) {}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/e7520/raminit.c"
#include "lib/generic_sdram.c"
#include "arch/x86/lib/stages.c"

static void main(unsigned long bist)
{
	static const struct mem_controller mch[] = {
		{
			.node_id = 0,
			.channel0 = {DIMM3, DIMM2, DIMM1, DIMM0, },
			.channel1 = {DIMM7, DIMM6, DIMM5, DIMM4, },
		}
	};

	if (bist == 0) {
		/* Skip this if there was a built in self test failure */
		early_mtrr_init();
		if (memory_initialized())
			skip_romstage();
	}

	w83627hf_set_clksel_48(DUMMY_DEV);
	w83627hf_enable_serial(CONSOLE_SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
//	report_bist_failure(bist);

	/* MOVE ME TO A BETTER LOCATION !!! */
	/* config LPC decode for flash memory access */
        device_t dev;
        dev = pci_locate_device(PCI_ID(0x8086, 0x24d0), 0);
        if (dev == PCI_DEV_INVALID)
                die("Missing ich5?");
        pci_write_config32(dev, 0xe8, 0x00000000);
        pci_write_config8(dev, 0xf0, 0x00);

#if 0
	display_cpuid_update_microcode();
	print_pci_devices();
#endif
#if 1
	enable_smbus();
#endif
#if 0
//	dump_spd_registers(&cpu[0]);
	int i;
	for(i = 0; i < 1; i++)
		dump_spd_registers();
#endif
	disable_watchdogs();
//	dump_ipmi_registers();
	mainboard_set_e7520_leds();
	sdram_initialize(ARRAY_SIZE(mch), mch);
#if 1
	dump_pci_devices();
#endif
#if 0
	dump_pci_device(PCI_DEV(0, 0x00, 0));
	dump_bar14(PCI_DEV(0, 0x00, 0));
#endif
}
