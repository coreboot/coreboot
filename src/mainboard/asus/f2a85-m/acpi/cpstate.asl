/* SPDX-License-Identifier: GPL-2.0-only */

/* This file defines the processor and performance state capability
 * for each core in the system.  It is included into the DSDT for each
 * core.  It assumes that each core of the system has the same performance
 * characteristics.
*/
	/* P-state support: The maximum number of P-states supported by the */
	/* CPUs we'll use is 6. */
	/* Get from AMI BIOS. */
	Name(_PSS, Package(){
		Package()
		{
			0x00000D48,
			0x00011170,
			0x00000004,
			0x00000004,
			0x00000000,
			0x00000000
		},

		Package()
		{
			0x00000AF0,
			0x0000C544,
			0x00000004,
			0x00000004,
			0x00000001,
			0x00000001
		},

		Package()
		{
		    0x000009C4,
		    0x0000B3B0,
		    0x00000004,
		    0x00000004,
		    0x00000002,
		    0x00000002
		},

		Package()
		{
		    0x00000898,
		    0x0000ABE0,
		    0x00000004,
		    0x00000004,
		    0x00000003,
		    0x00000003
		},

		Package()
		{
		    0x00000708,
		    0x0000A410,
		    0x00000004,
		    0x00000004,
		    0x00000004,
		    0x00000004
		},

		Package()
		{
		    0x00000578,
		    0x00006F54,
		    0x00000004,
		    0x00000004,
		    0x00000005,
		    0x00000005
		}
	})

	Name(_PCT, Package(){
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
	})

	Method(_PPC, 0){
		Return(0)
	}
