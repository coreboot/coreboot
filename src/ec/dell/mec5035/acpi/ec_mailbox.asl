/* SPDX-License-Identifier: GPL-2.0-only */

/* Mailbox Interface Mutex */
Mutex (MBLK, 0)

/* EC Mailbox Interface */
OperationRegion(ECMB, SystemIO, 0x910, 2)
Field(ECMB, ByteAcc, NoLock, Preserve)
{
	EMBI, 8, /* Index port */
	EMBD, 8, /* Data port */
}

/*
 * Read mailbox registers
 * Arg0: Start offset
 * Arg1: Count
 * Return: A buffer containing the data
 */
Method (EMBR, 2)
{
	Acquire (MBLK, 0xffff)
	Local0 = Buffer (Arg1) {}
	For (Local1 = 0, Local1 < Arg1, Local1++) {
		EMBI = 0x10 + Arg0 + Local1
		Local0[Local1] = EMBD
	}
	Release (MBLK)
	Return (Local0)
}

/*
 * Write mailbox registers
 * Arg0: Start offset
 * Arg1: Buffer of data to write
 */
Method (EMBW, 2)
{
	Acquire (MBLK, 0xffff)
	For (Local0 = 0, Local0 < SizeOf(Arg1), Local0++) {
		EMBI = 0x10 + Arg0 + Local0
		EMBD = DerefOf(Arg1[Local0])
	}
	Release (MBLK)
}

/*
 * EC Mailbox Command
 * Arg0: Command byte
 * Arg1: Start Offset of arguments
 * Arg2: Buffer of argument data
 */
Method (EMBC, 3)
{
	Acquire (MBLK, 0xffff)
	/* Write arguments */
	EMBW (Arg1, Arg2)

	/* Write command */
	EMBI = 0
	EMBD = Arg0

	/* Wait for EC */
	Local0 = 1
	While (Local0) {
		EMBI = 0
		Local0 = EMBD
	}
	Release (MBLK)
}
