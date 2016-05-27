/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
DefinitionBlock ("DSDT.AML","DSDT",0x01,"XXXXXX","XXXXXXXX",0x00010001
		)
	{
		#include "routing.asl"
	}
*/

/* Routing is in System Bus scope */
Name(PR0, Package(){
	/* NB devices */
	/* Bus 0, Dev 0 - F16 Host Controller */

	/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics(IGP) */
	/* Bus 0, Dev 1, Func 1 - HDMI Audio Controller */
	Package(){0x0001FFFF, 0, INTB, 0 },
	Package(){0x0001FFFF, 1, INTC, 0 },


	/* Bus 0, Dev 2 Func 0,1,2,3,4,5 - PCIe Bridges */
	Package(){0x0002FFFF, 0, INTC, 0 },
	Package(){0x0002FFFF, 1, INTD, 0 },
	Package(){0x0002FFFF, 2, INTA, 0 },
	Package(){0x0002FFFF, 3, INTB, 0 },

	/* FCH devices */
	/* Bus 0, Dev 20 - F0:SMBus/ACPI,F2:HDAudio;F3:LPC;F7:SD */
	Package(){0x0014FFFF, 0, INTA, 0 },
	Package(){0x0014FFFF, 1, INTB, 0 },
	Package(){0x0014FFFF, 2, INTC, 0 },
	Package(){0x0014FFFF, 3, INTD, 0 },

	/* Bus 0, Dev 18, 19, 22 Func 0 - USB: OHCI */
	/* Bus 0, Dev 18, 19, 22 Func 1 - USB: EHCI */
	Package(){0x0012FFFF, 0, INTC, 0 },
	Package(){0x0012FFFF, 1, INTB, 0 },

	Package(){0x0013FFFF, 0, INTC, 0 },
	Package(){0x0013FFFF, 1, INTB, 0 },

	Package(){0x0016FFFF, 0, INTC, 0 },
	Package(){0x0016FFFF, 1, INTB, 0 },

	/* Bus 0, Dev 10 - USB: XHCI func 0, 1 */
	Package(){0x0010FFFF, 0, INTC, 0 },
	Package(){0x0010FFFF, 1, INTB, 0 },

	/* Bus 0, Dev 17 - SATA controller */
	Package(){0x0011FFFF, 0, INTD, 0 },

})

Name(APR0, Package(){
	/* NB devices in APIC mode */
	/* Bus 0, Dev 0 - F15 Host Controller */

	/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics(IGP) */
	Package(){0x0001FFFF, 0, 0, 44 },
	Package(){0x0001FFFF, 1, 0, 45 },

	/* Bus 0, Dev 2 - PCIe Bridges  */
	Package(){0x0002FFFF, 0, 0, 24 },
	Package(){0x0002FFFF, 1, 0, 25 },
	Package(){0x0002FFFF, 2, 0, 26 },
	Package(){0x0002FFFF, 3, 0, 27 },


	/* SB devices in APIC mode */
	/* Bus 0, Dev 20 - F0:SMBus/ACPI,F2:HDAudio;F3:LPC;F7:SD */
	Package(){0x0014FFFF, 0, 0, 16 },
	Package(){0x0014FFFF, 1, 0, 17 },
	Package(){0x0014FFFF, 2, 0, 18 },
	Package(){0x0014FFFF, 3, 0, 19 },

	/* Bus 0, Dev 18, 19, 22 Func 0 - USB: OHCI */
	/* Bus 0, Dev 18, 19, 22 Func 1 - USB: EHCI */
	Package(){0x0012FFFF, 0, 0, 18 },
	Package(){0x0012FFFF, 1, 0, 17 },

	Package(){0x0013FFFF, 0, 0, 18 },
	Package(){0x0013FFFF, 1, 0, 17 },

	Package(){0x0016FFFF, 0, 0, 18 },
	Package(){0x0016FFFF, 1, 0, 17 },

	/* Bus 0, Dev 10 - USB: XHCI func 0, 1 */
	Package(){0x0010FFFF, 0, 0, 0x12},
	Package(){0x0010FFFF, 1, 0, 0x11},

	/* Bus 0, Dev 17 - SATA controller */
	Package(){0x0011FFFF, 0, 0, 19 },

})

Name(PS2, Package(){
	Package(){0x0000FFFF, 0, INTC, 0 },
	Package(){0x0000FFFF, 1, INTD, 0 },
	Package(){0x0000FFFF, 2, INTA, 0 },
	Package(){0x0000FFFF, 3, INTB, 0 },
})
Name(APS2, Package(){
	Package(){0x0000FFFF, 0, 0, 18 },
	Package(){0x0000FFFF, 1, 0, 19 },
	Package(){0x0000FFFF, 2, 0, 16 },
	Package(){0x0000FFFF, 3, 0, 17 },
})

/* GFX */
Name(PS4, Package(){
	Package(){0x0000FFFF, 0, INTA, 0 },
	Package(){0x0000FFFF, 1, INTB, 0 },
	Package(){0x0000FFFF, 2, INTC, 0 },
	Package(){0x0000FFFF, 3, INTD, 0 },
})
Name(APS4, Package(){
	/* PCIe slot - Hooked to PCIe slot 4 */
	Package(){0x0000FFFF, 0, 0, 24 },
	Package(){0x0000FFFF, 1, 0, 25 },
	Package(){0x0000FFFF, 2, 0, 26 },
	Package(){0x0000FFFF, 3, 0, 27 },
})

/* GPP 0 */
Name(PS5, Package(){
	Package(){0x0000FFFF, 0, INTB, 0 },
	Package(){0x0000FFFF, 1, INTC, 0 },
	Package(){0x0000FFFF, 2, INTD, 0 },
	Package(){0x0000FFFF, 3, INTA, 0 },
})
Name(APS5, Package(){
	Package(){0x0000FFFF, 0, 0, 28 },
	Package(){0x0000FFFF, 1, 0, 29 },
	Package(){0x0000FFFF, 2, 0, 30 },
	Package(){0x0000FFFF, 3, 0, 31 },
})

/* GPP 1 */
Name(PS6, Package(){
	Package(){0x0000FFFF, 0, INTC, 0 },
	Package(){0x0000FFFF, 1, INTD, 0 },
	Package(){0x0000FFFF, 2, INTA, 0 },
	Package(){0x0000FFFF, 3, INTB, 0 },
})
Name(APS6, Package(){
	Package(){0x0000FFFF, 0, 0, 32 },
	Package(){0x0000FFFF, 1, 0, 33 },
	Package(){0x0000FFFF, 2, 0, 34 },
	Package(){0x0000FFFF, 3, 0, 35 },
})

/* GPP 2 */
Name(PS7, Package(){
	Package(){0x0000FFFF, 0, INTD, 0 },
	Package(){0x0000FFFF, 1, INTA, 0 },
	Package(){0x0000FFFF, 2, INTB, 0 },
	Package(){0x0000FFFF, 3, INTC, 0 },
})
Name(APS7, Package(){
	Package(){0x0000FFFF, 0, 0, 36 },
	Package(){0x0000FFFF, 1, 0, 37 },
	Package(){0x0000FFFF, 2, 0, 38 },
	Package(){0x0000FFFF, 3, 0, 39 },
})

/* GPP 3 */
Name(PS8, Package(){
	Package(){0x0000FFFF, 0, INTA, 0 },
	Package(){0x0000FFFF, 1, INTB, 0 },
	Package(){0x0000FFFF, 2, INTC, 0 },
	Package(){0x0000FFFF, 3, INTD, 0 },
})
Name(APS8, Package(){
	Package(){0x0000FFFF, 0, 0, 40 },
	Package(){0x0000FFFF, 1, 0, 41 },
	Package(){0x0000FFFF, 2, 0, 42 },
	Package(){0x0000FFFF, 3, 0, 43 },
})
