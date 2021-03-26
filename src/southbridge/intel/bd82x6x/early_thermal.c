/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include "pch.h"
#include "cpu/intel/model_206ax/model_206ax.h"
#include <cpu/x86/msr.h>

/* Temporary address for the thermal BAR */
#define TBARB_TEMP 0x40000000

/* Early thermal init, must be done prior to giving ME its memory
   which is done at the end of raminit */
void early_thermal_init(void)
{
	const pci_devfn_t dev = PCH_THERMAL_DEV;

	/* Program address for temporary BAR */
	pci_write_config32(dev, 0x40, TBARB_TEMP);
	pci_write_config32(dev, 0x44, 0x0);

	/* Activate temporary BAR */
	pci_or_config32(dev, 0x40, 5);

	write16p(TBARB_TEMP + 0x04, 0x3a2b);

	write8p(TBARB_TEMP + 0x0c, 0xff);
	write8p(TBARB_TEMP + 0x0d, 0x00);
	write8p(TBARB_TEMP + 0x0e, 0x40);
	write8p(TBARB_TEMP + 0x82, 0x00);
	write8p(TBARB_TEMP + 0x01, 0xba);

	/* Perform init */
	/* Configure TJmax */
	const msr_t msr = rdmsr(MSR_TEMPERATURE_TARGET);
	write16p(TBARB_TEMP + 0x12, ((msr.lo >> 16) & 0xff) << 6);
	/* Northbridge temperature slope and offset */
	write16p(TBARB_TEMP + 0x16, 0x808c);

	write16p(TBARB_TEMP + 0x14, 0xde87);

	/* Enable thermal data reporting, processor, PCH and northbridge */
	write16p(TBARB_TEMP + 0x1a, (read16p(TBARB_TEMP + 0x1a) & ~0xf) | 0x10f0);

	/* Disable temporary BAR */
	pci_and_config32(dev, 0x40, ~1);

	pci_write_config32(dev, 0x40, 0);

	RCBA32_AND_OR(0x38b0, 0xffff8003, 0x403c);
}
