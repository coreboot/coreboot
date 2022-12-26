/* SPDX-License-Identifier: GPL-2.0-only */

/* WARNING
 * Switchable graphics not yet tested!
 */

/* Hybrid graphics enable/disable GPIO bitfields */
Name (HYG1, 0x004A0000)
Name (HYG2, 0x00020000)

/* GPIO control port */
Name (GPCP, DEFAULT_GPIOBASE)

/* GPIO control map */
OperationRegion (GPCM, SystemIO, GPCP, 0x3F)
Field (GPCM, ByteAcc, NoLock, Preserve) {
	GPUS, 32,
	GPIS, 32,
	Offset (0x0C),
	GPLV, 32,
	Offset (0x30),
	GQUS, 32,
	GQIS, 32,
	GQLV, 32
}

Method(SHYB, 1) {
	/* Switch hybrid graphics */
	if (Arg0 == 1)
	{
		/* Discrete graphics requested */
		GPLV |= HYG1
		GQLV |= HYG2
	}
	else
	{
		/* Integrated graphics requested */
		Local0 = HYG1 ^ 0xFFFFFFFF
		GPLV &= Local0
		Local0 = HYG2 ^ 0xFFFFFFFF
		GQLV &= Local0
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
	Return(Zero)
}
