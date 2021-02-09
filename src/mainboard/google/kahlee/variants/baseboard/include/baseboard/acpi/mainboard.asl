/* SPDX-License-Identifier: GPL-2.0-only */

/* Memory related values */
Name (LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
Name (PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
Name (PBLN, 0x0)	/* Length of BIOS area */

/* Base address of PCIe config space */
Name (PCBA, CONFIG_MMCONF_BASE_ADDRESS)

/* Length of PCIe config space, 1MB each bus */
Name (PCLN, Multiply(0x100000, CONFIG_MMCONF_BUS_NUMBER))

/* Base address of HPET table */
Name (HPBA, 0xFED00000)
