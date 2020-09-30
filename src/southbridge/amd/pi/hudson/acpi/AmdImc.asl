/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion(IMIO, SystemIO, 0x3E, 0x02)
Field(IMIO , ByteAcc, NoLock, Preserve) {
	IMCX,8,
	IMCA,8
}

IndexField(IMCX, IMCA, ByteAcc, NoLock, Preserve) {
	Offset(0x80),
	MSTI, 8,
	MITS, 8,
	MRG0, 8,
	MRG1, 8,
	MRG2, 8,
	MRG3, 8,
}

Method(WACK, 0)
{
	Local0 = 0
	Local1 = 50
	While ((Local0 != 0xfa) && (Local1 > 0)) {
		Local0 = MRG0
		Sleep(10)
		Local1--
	}
}

//Init
Method (ITZE, 0)
{
	MRG0 = 0
	MRG1 = 0xb5
	MRG2 = 0
	MSTI = 0x96
	WACK()

	MRG0 = 0
	MRG1 = 0
	MRG2 = 0
	MSTI = 0x80
	WACK()

	Local0 = MRG2 | 0x01

	MRG0 = 0
	MRG1 = 0
	MRG2 = Local0
	MSTI = 0x81
	WACK()
}
