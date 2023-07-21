/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>

#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#include <option.h>

#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/pei_data.h>

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6779D_SP2)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* {enable, current, oc_pin} */
	{ 1, 2, 0 }, /* Port 0: USB3 front internal header, top */
	{ 1, 2, 0 }, /* Port 1: USB3 front internal header, bottom */
	{ 1, 2, 1 }, /* Port 2: USB3 rear, ETH top */
	{ 1, 2, 1 }, /* Port 3: USB3 rear, ETH bottom */
	{ 1, 2, 2 }, /* Port 4: USB2 rear, PS2 top */
	{ 1, 2, 2 }, /* Port 5: USB2 rear, PS2 bottom */
	{ 1, 2, 3 }, /* Port 6: USB2 internal header USB78, top */
	{ 1, 2, 3 }, /* Port 7: USB2 internal header USB78, bottom */
	{ 1, 2, 4 }, /* Port 8: USB2 internal header USB910, top */
	{ 1, 2, 4 }, /* Port 9: USB2 internal header USB910, bottom */
	{ 1, 2, 6 }, /* Port 10: USB2 internal header USB1112, top */
	{ 1, 2, 5 }, /* Port 11: USB2 internal header USB1112, bottom */
	{ 0, 2, 5 }, /* Port 12: Unused. Asus proprietary DEBUG_PORT ??? */
	{ 0, 2, 6 }  /* Port 13: Unused. Asus proprietary DEBUG_PORT ??? */
};

void bootblock_mainboard_early_init(void)
{
	/* Setup COM/UART */
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* TODO / FIXME: Setup Multifuncion/SIO pins for COM */

	pnp_set_logical_device(SERIAL_DEV);
	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	const uint8_t spdaddr[] = {0xa0, 0xa2, 0xa4, 0xa6}; /* SMBus mul 2 */

	memcpy(pei_data->spd_addresses, &spdaddr, sizeof(pei_data->spd_addresses));

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
