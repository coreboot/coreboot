/* SPDX-License-Identifier: GPL-2.0-only */

	/* Base address of PCIe config space */
	Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)
	/* Length of PCIe config space, 1MB each bus */
	Name(PCLN, Multiply(0x100000, CONFIG_MMCONF_BUS_NUMBER))
	/* Base address of HPET table */
