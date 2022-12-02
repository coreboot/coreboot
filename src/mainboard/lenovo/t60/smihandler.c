/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
#include <southbridge/intel/common/pmutil.h>
#include <ec/acpi/ec.h>
#include "dock.h"
#include "smi.h"

#define GPE_EC_SCI	12

#define LVTMA_BL_MOD_LEVEL 0x7af9 /* ATI Radeon backlight level */

static void mainboard_smi_brightness_down(void)
{
	uint32_t reg32 = pci_read_config32(PCI_DEV(1, 0, 0), PCI_BASE_ADDRESS_2) & ~0xf;
	u8 *bar = (void *)(uintptr_t)reg32;

	/* Validate pointer before using it */
	if (!bar || smm_points_to_smram(bar, LVTMA_BL_MOD_LEVEL + sizeof(uint8_t)))
		return;

	printk(BIOS_DEBUG, "bar: %p, level %02X\n", bar, *(bar+LVTMA_BL_MOD_LEVEL));
	*(bar+LVTMA_BL_MOD_LEVEL) &= 0xf0;
	if (*(bar+LVTMA_BL_MOD_LEVEL) > 0x10)
		*(bar+LVTMA_BL_MOD_LEVEL) -= 0x10;
}

static void mainboard_smi_brightness_up(void)
{
	uint32_t reg32 = pci_read_config32(PCI_DEV(1, 0, 0), PCI_BASE_ADDRESS_2) & ~0xf;
	u8 *bar = (void *)(uintptr_t)reg32;

	/* Validate pointer before using it */
	if (!bar || smm_points_to_smram(bar, LVTMA_BL_MOD_LEVEL + sizeof(uint8_t)))
		return;

	printk(BIOS_DEBUG, "bar: %p, level %02X\n", bar, *(bar+LVTMA_BL_MOD_LEVEL));
	*(bar+LVTMA_BL_MOD_LEVEL) |= 0x0f;
	if (*(bar+LVTMA_BL_MOD_LEVEL) < 0xf0)
		*(bar+LVTMA_BL_MOD_LEVEL) += 0x10;
}

static void mainboard_smi_dock_connect(void)
{
	/* If there's an legacy I/O module present, we're not
	 * allowed to connect the Docking LPC Bus, as both Super I/O
	 * chips are using 0x2e as base address.
	 */
	if (legacy_io_present())
		return;

	if (!dock_connect()) {
		/* set dock LED to indicate status */
		ec_write(0x0c, 0x08);
		ec_write(0x0c, 0x89);
	} else {
		/* blink dock LED to indicate failure */
		ec_write(0x0c, 0xc8);
		ec_write(0x0c, 0x09);
	}
}

static void mainboard_smi_dock_disconnect(void)
{
	dock_disconnect();
	ec_write(0x0c, 0x09);
	ec_write(0x0c, 0x08);
}

int mainboard_io_trap_handler(int smif)
{
	switch (smif) {
	case SMI_DOCK_CONNECT:
		mainboard_smi_dock_connect();
		break;

	case SMI_DOCK_DISCONNECT:
		mainboard_smi_dock_disconnect();
		break;

	case SMI_BRIGHTNESS_UP:
		mainboard_smi_brightness_up();
		break;

	case SMI_BRIGHTNESS_DOWN:
		mainboard_smi_brightness_down();
		break;

	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 1
	 * On failure, the IO Trap Handler returns a value != 1 */
	return 1;
}

static void mainboard_smi_handle_ec_sci(void)
{
	u8 status = inb(EC_SC);
	u8 event;

	if (!(status & EC_SCI_EVT))
		return;

	event = ec_query();
	printk(BIOS_DEBUG, "EC event %#02x\n", event);

	switch (event) {
		/* brightness up */
		case 0x14:
			mainboard_smi_brightness_up();
			break;
		/* brightness down */
		case 0x15:
			mainboard_smi_brightness_down();
			break;
		/* Fn-F9 Key */
		case 0x18:
		/* power loss */
		case 0x27:
		/* undock event */
		case 0x50:
			mainboard_smi_dock_disconnect();
			break;
		/* dock event */
		case 0x37:
			mainboard_smi_dock_connect();
			break;
		default:
			break;
	}
}

void mainboard_smi_gpi(u32 gpi)
{
	if (gpi & (1 << GPE_EC_SCI))
		mainboard_smi_handle_ec_sci();
}

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
		case APM_CNT_ACPI_ENABLE:
			/* use 0x1600/0x1604 to prevent races with userspace */
			ec_set_ports(0x1604, 0x1600);
			/* route H8SCI to SCI */
			gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
			break;
		case APM_CNT_ACPI_DISABLE:
			/* we have to use port 0x62/0x66, as 0x1600/0x1604 doesn't
			   provide a EC query function */
			ec_set_ports(0x66, 0x62);
			/* route H8SCI# to SMI */
			gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
			break;
		default:
			break;
	}
	return 0;
}
