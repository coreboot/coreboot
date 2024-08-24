/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_type.h>

#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#include <option.h>

#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/pei_data.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT6779D_SP2)

void bootblock_mainboard_early_init(void)
{
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/*
	 * USB3 mode:
	 * 0 = Disable: work always as USB 2.0(ehci)
	 * 1 = Enable: work always as USB 3.0(xhci)
	 * 2 = Auto: work as USB2.0(ehci) until OS loads USB3 xhci driver
	 * 3 = Smart Auto : same than Auto, but if OS loads USB3 driver
	 *     and reboots, it will keep the USB3.0 speed
	 */
	pei_data->usb3.mode = get_uint_option("usb3_mode", 1) & 0x3;
	/* Load USB3 pre-OS xHCI driver */
	pei_data->usb3.preboot_support = get_uint_option("usb3_drv", 1) & 0x1;
	/* Use USB3 xHCI streams */
	pei_data->usb3.xhci_streams = get_uint_option("usb3_streams", 1) & 0x1;
}
