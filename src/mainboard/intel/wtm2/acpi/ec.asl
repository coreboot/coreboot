Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, 10) // GPIO 10 is SMC_RUNTIME_SCI_N

	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xff)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x03),
		ACPR, 1,	// AC Power (1=present)
		    , 2,
		CFAN, 1,	// CPU Fan (1=on)
		    , 2,
		LIDS, 1,	// Lid State (1=open)
		    , 1,
		SPTR, 8,	// SMBUS Protocol Register
		SSTS, 8,	// SMBUS Status Register
		SADR, 8,	// SMBUS Address Register
		SCMD, 8,	// SMBUS Command Register
		SBFR, 256,	// SMBUS Block Buffer
		SCNT, 8,	// SMBUS Block Count

		Offset (0x3a),
		ECMD, 8,	// EC Command Register

		Offset (0x82),
		PECL, 8,	// PECI fractional (1/64 Celsius)
		PECH, 8,	// PECI integer (Celsius)
	}

	Name (_CRS, ResourceTemplate()
	{
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})
}
