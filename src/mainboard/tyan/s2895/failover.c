#define ASSEMBLY 1
#define __PRE_RAM__

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"

#include "southbridge/nvidia/ck804/ck804_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"

#define post_code(x) outb(x, 0x80)

#include "superio/smsc/lpc47b397/lpc47b397_early_serial.c"
#include "superio/smsc/lpc47b397/lpc47b397_early_gpio.c"
#define SUPERIO_GPIO_DEV PNP_DEV(0x2e, LPC47B397_RT)
#define SUPERIO_GPIO_IO_BASE 0x400

static void sio_setup(void)
{

	unsigned value;
	uint32_t dword;
	uint8_t byte;

	pci_write_config32(PCI_DEV(0, CK804_DEVN_BASE+1, 0), 0xac, 0x047f0400);

	byte = pci_read_config8(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0x7b);
	byte |= 0x20;
	pci_write_config8(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0x7b, byte);

	dword = pci_read_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa0);
	dword |= (1<<29)|(1<<0);
	pci_write_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa0, dword);

	dword = pci_read_config32(PCI_DEV(0, CK804_DEVN_BASE+1, 0), 0xa4);
	dword |= (1<<16);
	pci_write_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa4, dword);

	lpc47b397_enable_serial(SUPERIO_GPIO_DEV, SUPERIO_GPIO_IO_BASE);
	value = lpc47b397_gpio_offset_in(SUPERIO_GPIO_IO_BASE, 0x77);
	value &= 0xbf;
	lpc47b397_gpio_offset_out(SUPERIO_GPIO_IO_BASE, 0x77, value);

}

void mainboard_bsp_init()
{
	/* Nothing special needs to be done to find bus 0 */
	/* Allow the HT devices to be found */

	enumerate_ht_chain();

	sio_setup();

	/* Setup the ck804 */
	ck804_enable_rom();
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	unsigned last_boot_normal_x = last_boot_normal();

	/* Is this a cpu only reset? or Is this a secondary cpu? */
	if ((cpu_init_detectedx) || (!boot_cpu())) {
		if (last_boot_normal_x) {
			goto normal_image;
		} else {
			goto fallback_image;
		}
	}

	mainboard_bsp_init();

	/* Is this a deliberate reset by the bios */
	post_code(0x22);
	if (bios_reset_detected() && last_boot_normal_x) {
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
	post_code(0x23);
	__asm__ volatile ("jmp __normal_image"
	: /* outputs */
	: "a" (bist), "b"(cpu_init_detectedx) /* inputs */
	);

 fallback_image:
	post_code(0x25);
	__asm__ volatile ("jmp __fallback_image"
	: /* outputs */
	: "a" (bist), "b" (cpu_init_detectedx) /* inputs */
	);
}
