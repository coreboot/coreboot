/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/pei_data.h>

#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#include <option.h>

#define SERIAL_DEV PNP_DEV(CONFIG_SUPERIO_PNP_BASE, NCT6779D_SP1)
#define GPIO0_DEV  PNP_DEV(CONFIG_SUPERIO_PNP_BASE, NCT6779D_WDT1_GPIO01_V)

void bootblock_mainboard_early_init(void)
{
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	nuvoton_pnp_enter_conf_state(GPIO0_DEV);

	/* Turn on DRAM_LED. If raminit dies, this would remain on and we know
	 * we have a problem. We turn it off in ramstage. */
	pnp_set_logical_device(GPIO0_DEV);
	pnp_write_config(GPIO0_DEV, 0x30, 0x02);
	pnp_write_config(GPIO0_DEV, 0xe0, 0x7f);
	pnp_write_config(GPIO0_DEV, 0xe1, 0x00);

	nuvoton_pnp_exit_conf_state(GPIO0_DEV);

	/*
	 * TODO: Put PCIe root port 7 (00:1c.6) into subtractive decode and have it accept I/O
	 * cycles. This should allow a POST card in the PCI slot, connected via an ASM1083
	 * bridge to this port, to receive POST codes.
	 */
}

void mainboard_fill_pei_data(struct pei_data *pei)
{
	/*
	 * USB 3 mode settings.
	 * These are obtained from option table then bit masked to keep within range.
	 */
	/*
	 * 0 = Disable: work always as USB 2.0(ehci)
	 * 1 = Enable: work always as USB 3.0(xhci)
	 * 2 = Auto: work as USB2.0(ehci) until OS loads USB3 xhci driver
	 * 3 = Smart Auto : same than Auto, but if OS loads USB3 driver
	 *     and reboots, it will keep the USB3.0 speed
	 */
	pei->usb3.mode = get_uint_option("usb3_mode", 1) & 0x3;
	/* 1=Load xHCI pre-OS drv */
	pei->usb3.preboot_support = get_uint_option("usb3_drv", 1) & 0x1;
	/*
	 * 0=Don't use xHCI streams for better compatibility
	 * 1=use xHCI streams for better speed
	 */
	pei->usb3.xhci_streams = get_uint_option("usb3_streams", 1) & 0x1;
}
