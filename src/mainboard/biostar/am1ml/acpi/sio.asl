/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
	Store(0x87, SIOI)
	Store(0x01, SIOI)
	Store(0x55, SIOI)
	Store(0x55, SIOI)
}

/* Exit the 8728 Config */
Method (XPNP)
{
	Store (0x02, SIOI)
	Store (0x02, SIOD)
}

/*
 * Keyboard PME is routed to SB700 Gevent3. We can wake
 * up the system by pressing the key.
 */
Method (SIOS, 1)
{
	/* We only enable KBD PME for S5. */
	If (LLess (Arg0, 0x05))
	{
		EPNP()
		/* DBGO("8728F\n") */
		Store (0x4, LDN)
		Store (One, ACTR)  /* Enable EC */
		/*
		Store (0x4, LDN)
		Store (0x04, APC4)
		*/  /* falling edge. which mode? Not sure. */
		Store (0x4, LDN)
		Store (0x08, APC1) /* clear PME status, Use 0x18 for mouse & KBD */
		Store (0x4, LDN)
		Store (0x08, APC0) /* enable PME, Use 0x18 for mouse & KBD */
		XPNP()
	}
}
