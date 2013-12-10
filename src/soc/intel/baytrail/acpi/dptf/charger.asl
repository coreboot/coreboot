Device (TCHG)
{
	Name (_HID, "INT3403")
	Name (_UID, 0)
	Name (PTYP, 0x0B)
	Name (_STR, Unicode("Battery Charger"))

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	Name (PPSS, Package ()
	{
		Package () { 0, 0, 0, 0, 0, 0x880, "mA", 0 }, /* 2.1A */
		Package () { 0, 0, 0, 0, 1, 0x800, "mA", 0 }, /* 2.0A */
		Package () { 0, 0, 0, 0, 2, 0x600, "mA", 0 }, /* 1.5A */
		Package () { 0, 0, 0, 0, 3, 0x400, "mA", 0 }, /* 1.0A */
		Package () { 0, 0, 0, 0, 4, 0x200, "mA", 0 }, /* 0.5A */
		Package () { 0, 0, 0, 0, 5, 0x000, "mA", 0 }, /* 0.0A */
	})

	Method (PPPC)
	{
		Return (0)
	}

	Method (SPPC, 1, Serialized)
	{
		/* TODO: Tell EC to limit battery charging */
	}
}
