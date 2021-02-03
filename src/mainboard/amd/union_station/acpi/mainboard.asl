/* SPDX-License-Identifier: GPL-2.0-only */

/* Data to be patched by the BIOS during POST */
/* FIXME the patching is not done yet! */
/* Memory related values */
Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
Name(PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
Name(PBLN, 0x0)	/* Length of BIOS area */

Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)	/* Base address of PCIe config space */
Name(HPBA, 0xFED00000)	/* Base address of HPET table */

Scope(\_SI) {
	Method(_SST, 1) {
		/* DBGO("\\_SI\\_SST\n") */
		/* DBGO("   New Indicator state: ") */
		/* DBGO(Arg0) */
		/* DBGO("\n") */
	}
} /* End Scope SI */
