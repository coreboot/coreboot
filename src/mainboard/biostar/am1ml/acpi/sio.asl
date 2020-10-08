/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (IOID, SystemIO, 0x2E, 0x02)
Field (IOID, ByteAcc, NoLock, Preserve)
{
	SIOI,   8,    SIOD,   8		/* 0x2E and 0x2F */
}

IndexField (SIOI, SIOD, ByteAcc, NoLock, Preserve)
{
	Offset (0x07),
	LDN,	8,	/* Logical Device Number */
	Offset (0x20),
	CID1,	8,	/* Chip ID Byte 1, 0x87 */
	CID2,	8,	/* Chip ID Byte 2, 0x12 */
	Offset (0x30),
	ACTR,	8,	/* Function activate */
	Offset (0xF0),
	APC0,	8,	/* APC/PME Event Enable Register */
	APC1,	8,	/* APC/PME Status Register */
	APC2,	8,	/* APC/PME Control Register 1 */
	APC3,	8,	/* Environment Controller Special Configuration Register */
	APC4,	8	/* APC/PME Control Register 2 */
}

/* Enter the 8728 Config */
Method (EPNP)
{
	SIOI = 0x87
	SIOI = 0x01
	SIOI = 0x55
	SIOI = 0x55
}

/* Exit the 8728 Config */
Method (XPNP)
{
	SIOI = 0x02
	SIOD = 0x02
}

/*
 * Keyboard PME is routed to SB700 Gevent3. We can wake
 * up the system by pressing the key.
 */
Method (SIOS, 1)
{
	/* We only enable KBD PME for S5. */
	If (Arg0 < 0x05)
	{
		EPNP()
		/* DBGO("8728F\n") */
		LDN = 0x4
		ACTR = 1  /* Enable EC */
		/*
		LDN = 0x4
		APC4 = 0x04
		*/  /* falling edge. which mode? Not sure. */
		LDN = 0x4
		APC1 = 0x08 /* clear PME status, Use 0x18 for mouse & KBD */
		LDN = 0x4
		APC0 = 0x08 /* enable PME, Use 0x18 for mouse & KBD */
		XPNP()
	}
}
