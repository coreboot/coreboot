	Name (BRIG, Package (0x12)
	{
		100, /* default AC */
		100, /* default Battery */
		  2,
		  4,
		  5,
		  7,
		  9,
		 11,
		 13,
		 18,
		 20,
		 24,
		 29,
		 33,
		 40,
		 50,
		 67,
		100,
	})

	Method (XBCM, 1, NotSerialized)
	{
		Store (ShiftLeft (Arg0, 4), BCLV)
		Store (0x80000000, CR1)
		Store (0x0610, BCLM)
	}

	Method (XBQC, 0, NotSerialized)
	{
		Store (BCLV, Local0)
		ShiftRight (Local0, 4, Local0)
		Return (Local0)
	}
