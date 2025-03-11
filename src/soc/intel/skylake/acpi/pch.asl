/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/gpe.h>
#include <soc/pcr_ids.h>

/* PCI IRQ assignment */
#include "pci_irqs.asl"

/* GPIO Controller */
#include "gpio.asl"

/* Interrupt Routing */
#include "irqlinks.asl"

/* LPC 0:1f.0 */
#include "lpc.asl"

/* PCH HDA */
#include "pch_hda.asl"

/* PCIE Ports */
#include "pcie.asl"

/* PCR Access */
#include <soc/intel/common/acpi/pch_pcr.asl>

/* PMC 0:1f.2 */
#include "pmc.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.3 */
#include <soc/intel/common/block/acpi/acpi/smbus.asl>

/* Storage Controllers */
#include "scs.asl"

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* CSE/HECI #1 0:16.0 */
#include <soc/intel/common/block/acpi/acpi/heci.asl>

Method (_OSC, 4)
{
	/* Check for proper GUID */
	If (Arg0 == ToUUID ("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
	{
		/* Let OS control everything */
		Return (Arg3)
	}
	Else
	{
		/* Unrecognized UUID */
		CreateDWordField (Arg3, 0, CDW1)
		CDW1 |= 4
		Return (Arg3)
	}
}

/* SGX */
#include <soc/intel/common/acpi/sgx.asl>
