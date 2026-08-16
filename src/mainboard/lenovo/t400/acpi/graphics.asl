/* SPDX-License-Identifier: GPL-2.0-only */

/* WARNING
 * Switchable graphics not yet tested!
 */

Method(SHYB, 1) {
	/* Switch hybrid graphics */
	if (Arg0 == 1)
	{
		/* Discrete graphics requested */
		\_SB.PCI0.STXS(17)
		\_SB.PCI0.STXS(19)	/* Backlight control */
		\_SB.PCI0.STXS(22)	/* Mux */
		\_SB.PCI0.STXS(49)	/* GFX_PWR_EN_D */
	}
	else
	{
		/* Integrated graphics requested */
		\_SB.PCI0.CTXS(17)
		\_SB.PCI0.CTXS(19)	/* Backlight control */
		\_SB.PCI0.CTXS(22)	/* Mux */
		\_SB.PCI0.CTXS(49)	/* GFX_PWR_EN_D */
	}
}

Method (ATPX, 2, Serialized) {
	/* Create local variables */
	Name (ATPR, Buffer (0x08) {
		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0
	})
	CreateWordField (ATPR, 0x00, SIZE)
	CreateWordField (ATPR, 0x02, VERS)
	CreateDWordField (ATPR, 0x04, FUNC)

	/* Version request */
	if (Arg0 == 0x0)
	{
		/* Assemble and return version information */
		SIZE = 0x08	/* Response length */
		VERS = 0x01	/* Version number */
		FUNC = 0x0F	/* Supported functions? */
		Return (ATPR)
	}

	/* Mux select */
	if (Arg0 == 0x2)
	{
		CreateByteField (Arg1, 0x02, PWST)
		Local0 = PWST
		Local0 &= 0x01
		If (Local0)
		{
			/* Enable discrete graphics */
			SHYB(0x01)
		}
		else
		{
			/* Enable integrated graphics */
			SHYB(0x00)
		}
	}
	Return(0)
}
