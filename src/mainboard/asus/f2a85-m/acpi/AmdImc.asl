//BTDC Due to IMC Fan, ACPI control codes
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
	Store(0, Local0)
	Store(50, Local1)
	While (LAnd (LNotEqual(Local0, 0xFA), LGreater(Local1,0))) {
		Store(MRG0, Local0)
		Sleep(10)
		Decrement(Local1)
	}
}

//Init
Method (ITZE, 0)
{
	Store(0, MRG0)
	Store(0xB5, MRG1)
	Store(0, MRG2)
	Store(0x96, MSTI)
	WACK()

	Store(0, MRG0)
	Store(0, MRG1)
	Store(0, MRG2)
	Store(0x80, MSTI)
	WACK()

	Or(MRG2, 0x01, Local0)

	Store(0, MRG0)
	Store(0, MRG1)
	Store(Local0, MRG2)
	Store(0x81, MSTI)
	WACK()
}

//Sleep
Method (IMSP, 0)
{
	Store(0, MRG0)
	Store(0xB5, MRG1)
	Store(0, MRG2)
	Store(0x96, MSTI)
	WACK()

	Store(0, MRG0)
	Store(1, MRG1)
	Store(0, MRG2)
	Store(0x98, MSTI)
	WACK()

	Store(0, MRG0)
	Store(0xB4, MRG1)
	Store(0, MRG2)
	Store(0x96, MSTI)
	WACK()
}

//Wake
Method (IMWK, 0)
{
	Store(0, MRG0)
	Store(0xB5, MRG1)
	Store(0, MRG2)
	Store(0x96, MSTI)
	WACK()

	Store(0, MRG0)
	Store(0, MRG1)
	Store(0, MRG2)
	Store(0x80, MSTI)
	WACK()

	Or(MRG2, 0x01, Local0)

	Store(0, MRG0)
	Store(0, MRG1)
	Store(Local0, MRG2)
	Store(0x81, MSTI)
	WACK()
}
