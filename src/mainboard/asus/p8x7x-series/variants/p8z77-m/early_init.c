/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/pei_data.h>
#include <southbridge/intel/bd82x6x/pch.h>

#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#include <option.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT6779D_SP1)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* {enable, current, oc_pin} */
	{1, 8, 0}, /* Port 0: USB3 front internal header, top */
	{1, 8, 0}, /* Port 1: USB3 front internal header, bottom */
	{1, 2, 1}, /* Port 2: USB3 rear, top */
	{1, 2, 1}, /* Port 3: USB3 rear, bottom */
	{1, 2, 2}, /* Port 4: USB2 rear, PS2 top */
	{1, 2, 2}, /* Port 5: USB2 rear, PS2 bottom */
	{1, 2, 3}, /* Port 6: USB2 rear, ETH, top */
	{1, 2, 3}, /* Port 7: USB2 rear, ETH, bottom */
	{1, 9, 4}, /* Port 8: USB2 internal header USB910, top */
	{1, 9, 4}, /* Port 9: USB2 internal header USB910, bottom */
	{1, 2, 6}, /* Port 10: USB2 internal header USB1112, top */
	{1, 2, 5}, /* Port 11: USB2 internal header USB1112, bottom */
	{1, 2, 5}, /* Port 12: USB2 internal header USB1314, top */
	{1, 2, 6}  /* Port 13: USB2 internal header USB1314, bottom */
};

void bootblock_mainboard_early_init(void)
{
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

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
