	Method (XBCM, 1, NotSerialized)
	{
		Store (Divide (Multiply (Arg0, BCLM), 100), BCLV)
	}

	Method (XBQC, 0, NotSerialized)
	{
		/* Find value close to BCLV in BRIG (which must be ordered) */
		Store (BCLV, Local0)			// Current value
		Store (BCLM, Local1)			// For calculations
		Store (2, Local2)			// Loop index
		While (LLess (Local2, Subtract (SizeOf (BRIG), 1))) {
			Store (DeRefOf (Index (BRIG, Local2)), Local3)
			/* Use same calculation as XBCM, to get exact matches */
			Store (Divide (Multiply (Local3, Local1), 100), Local3)

			If (LLessEqual (Local0, Local3)) {
				Return (DeRefOf (Index (BRIG, Local2)))
			}
			Add (Local2, 1, Local2)
		}
		/* Didn't find greater/equal value: use the last */
		Return (DeRefOf (Index (BRIG, Local2)))
	}
