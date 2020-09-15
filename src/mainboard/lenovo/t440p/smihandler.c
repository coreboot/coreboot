/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include <southbridge/intel/lynxpoint/pch.h>

#define GPE_EC_SCI	1
#define GPE_EC_WAKE	13

static void mainboard_smi_handle_ec_sci(void)
{
	u8 status = inb(EC_SC);
	u8 event;

	if (!(status & EC_SCI_EVT))
		return;

	event = ec_query();
	printk(BIOS_DEBUG, "EC event %#02x\n", event);
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << GPE_EC_SCI))
		mainboard_smi_handle_ec_sci();
}

/* lynxpoint doesn't have gpi_route_interrupt, so add it */
#define GPI_DISABLE 0x00
#define GPI_IS_SMI 0x01
#define GPI_IS_SCI 0x02
#define GPI_IS_NMI 0x03

static void gpi_route_interrupt(u8 gpi, u8 mode)
{
	u32 gpi_rout;

	gpi_rout = pci_read_config32(PCI_DEV(0, 0x1f, 0), GPIO_ROUT);
	gpi_rout &= ~(3 << (2 * gpi));
	gpi_rout |= ((mode & 3) << (2 * gpi));
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GPIO_ROUT, gpi_rout);
}

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		/* use 0x1600/0x1604 to prevent races with userspace */
		ec_set_ports(0x1604, 0x1600);
		/* route EC_SCI to SCI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
		/* discard all events, and enable attention */
		ec_write(0x80, 0x01);
		break;
	case APM_CNT_ACPI_DISABLE:
		/* we have to use port 0x62/0x66, as 0x1600/0x1604 doesn't
		   provide a EC query function */
		ec_set_ports(0x66, 0x62);
		/* route EC_SCI to SMI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
		/* discard all events, and enable attention */
		ec_write(0x80, 0x01);
		break;
	default:
		break;
	}
	return 0;
}

void mainboard_smi_sleep(u8 slp_typ)
{
	if (slp_typ == 3) {
		u8 ec_wake = ec_read(0x32);
		/* If EC wake events are enabled,
		 * enable wake on EC WAKE GPE. */
		if (ec_wake & 0x14) {
			/* Redirect EC WAKE GPE to SCI. */
			gpi_route_interrupt(GPE_EC_WAKE, GPI_IS_SCI);
		}
	}
}
