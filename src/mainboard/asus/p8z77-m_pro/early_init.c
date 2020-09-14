/* SPDX-License-Identifier: GPL-2.0-only */
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

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
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
	int usb3_mode = 1;
	get_option(&usb3_mode, "usb3_mode");
	usb3_mode &= 0x3; /* ensure it's 0/1/2/3 only */

	/* Load USB3 pre-OS xHCI driver */
	int usb3_drv = 1;
	get_option(&usb3_drv, "usb3_drv");
	usb3_drv &= 0x1; /* ensure it's 0/1 only */

	/* Use USB3 xHCI streams */
	int usb3_streams = 1;
	get_option(&usb3_streams, "usb3_streams");
	usb3_streams &= 0x1; /* ensure it's 0/1 only */

	struct pei_data pd = {
		.pei_version = PEI_VERSION,
		.mchbar = DEFAULT_MCHBAR,
		.dmibar = DEFAULT_DMIBAR,
		.epbar = DEFAULT_EPBAR,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = CONFIG_FIXED_SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBA,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 1, /* 0=Mobile, 1=Desktop/Server */
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xa0, 0xa2, 0xa4, 0xa6 }, /* SMBus mul 2 */
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 0, /* Asus 2203 BIOS shows XUECA016, but no EC */
		.gbe_enable = 0, /* Board uses no Intel GbE but a RTL8111F */
		.dimm_channel0_disabled = 0, /* Both DIMM enabled */
		.dimm_channel1_disabled = 0, /* Both DIMM enabled */
		.max_ddr3_freq = 1600, /* 1333=Sandy; 1600=Ivy */
		.usb_port_config = {
			/* {enabled, oc_pin, cable len 0x0080=<8inches/20cm} */
			{ 1, 0, 0x0080 }, /* USB3 front internal header */
			{ 1, 0, 0x0080 }, /* USB3 front internal header */
			{ 1, 1, 0x0080 }, /* USB3 ETH top connector */
			{ 1, 1, 0x0080 }, /* USB3 ETH botton connector */
			{ 1, 2, 0x0080 }, /* USB2 PS2 top connector */
			{ 1, 2, 0x0080 }, /* USB2 PS2 botton connector */
			{ 1, 3, 0x0080 }, /* USB2 internal header (USB78) */
			{ 1, 3, 0x0080 }, /* USB2 internal header (USB78) */
			{ 1, 4, 0x0080 }, /* USB2 internal header (USB910) */
			{ 1, 4, 0x0080 }, /* USB2 internal header (USB910) */
			{ 1, 6, 0x0080 }, /* USB2 internal header (USB1112) */
			{ 1, 5, 0x0080 }, /* USB2 internal header (USB1112) */
			{ 0, 5, 0x0080 }, /* Unused. Asus DEBUG_PORT ??? */
			{ 0, 6, 0x0080 }  /* Unused. Asus DEBUG_PORT ??? */
		},
		.usb3 = {
			/* 0=Disable; 1=Enable (start at USB3 speed)
			 * 2=Auto (start as USB2 speed until OS loads)
			 * 3=Smart Auto (like Auto but keep speed on reboot)
			 */
			usb3_mode,
			/* 4 bit switch mask. 0=not switchable, 1=switchable
			 * Means once it's loaded the OS, it can swap ports
			 * from/to EHCI/xHCI. Z77 has four USB3 ports, so 0xf
			 */
			0xf,
			usb3_drv, /* 1=Load xHCI pre-OS drv */
			/* 0=Don't use xHCI streams for better compatibility
			 * 1=use xHCI streams for better speed
			 */
			usb3_streams
		},
		/* ASUS P8Z77-M PRO manual says 1.35v DIMMs are supported */
		.ddr3lv_support = 1,
		/* PCIe 3.0 support. As we use Ivy Bridge, let's enable it,
		 * but might cause some system instability !
		 */
		.pcie_init = 1,
		/* Command Rate. 0=Auto; 1=1N; 2=2N.
		 * Leave it always at Auto for compatibility & stability
		 */
		.nmode = 0,
		/* DDR refresh rate. 0=Auto based on DRAM's temperature;
		 * 1=Normal rate for speed; 2=Double rate for stability
		 */
		.ddr_refresh_rate_config = 0
	};

	/* copy the data to output PEI */
	*pei_data = pd;
}
