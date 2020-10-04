/* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/pci_devs.h>

/* Calculate _ADR for Intel Integrated Sensor Hub Controller */
#define ISH_ACPI_DEVICE (PCH_DEV_SLOT_ISH << 16 | 0x0000)

Device (ISHB)
{
	Name (_ADR, ISH_ACPI_DEVICE)
	Name (_DDN, "Integrated Sensor Hub Controller")
}
