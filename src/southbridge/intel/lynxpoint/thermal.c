/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

#define TBARB_TEMP 0x40000000

#define THERMAL_DEV PCI_DEV(0, 0x1f, 6)

/* Early thermal init, it may need to be done prior to giving ME its memory */
void early_thermal_init(void)
{
	/* Program address for temporary BAR */
	pci_write_config32(THERMAL_DEV, 0x40, TBARB_TEMP);
	pci_write_config32(THERMAL_DEV, 0x44, 0);

	/* Activate temporary BAR */
	pci_or_config32(THERMAL_DEV, 0x40, 1);

	/*
	 * BWG section 17.3.1 says:
	 *
	 * ### Initializing Lynx Point Thermal Sensors ###
	 *
	 * The System BIOS must perform the following steps to initialize the Lynx
	 * Point thermal subsystem device, D31:F6. The System BIOS is required to
	 * repeat this process on a resume from Sx. BIOS may enable any or all of
	 * the registers below based on OEM's platform configuration. Intel does
	 * not recommend a value on some of the registers, since each platform has
	 * different temperature trip points and one may enable a trip to cause an
	 * SMI while another platform would cause an interrupt instead.
	 *
	 * The recommended flow for enabling thermal sensor is by setting up various
	 * temperature trip points first, followed by enabling the desired trip
	 * alert method and then enable the actual sensors from TSEL registers.
	 * If this flow is not followed, software will need to take special care
	 * to handle false events during setting up those registers.
	 */

	/* Step 1: Program CTT */
	write16p(TBARB_TEMP + 0x10, 0x0154);

	/* Step 2: Clear trip status from TSS and TAS */
	write8p(TBARB_TEMP + 0x06, 0xff);
	write8p(TBARB_TEMP + 0x80, 0xff);

	/* Step 3: Program TSGPEN and TSPIEN to zero */
	write8p(TBARB_TEMP + 0x84, 0x00);
	write8p(TBARB_TEMP + 0x82, 0x00);

	/*
	 * Step 4: If thermal reporting to an EC over SMBus is supported,
	 *         then write 0x01 to TSREL, else leave at default.
	 */
	write8p(TBARB_TEMP + 0x0a, 0x01);

	/* Disable temporary BAR */
	pci_and_config32(THERMAL_DEV, 0x40, ~1);

	/* Clear temporary BAR address */
	pci_write_config32(THERMAL_DEV, 0x40, 0);
}
