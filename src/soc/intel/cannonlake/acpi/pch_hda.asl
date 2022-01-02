/* SPDX-License-Identifier: GPL-2.0-only */

/* Audio Controller - Device 31, Function 3 */

Device (HDAS)
{
	Name (_ADR, 0x001f0003)
	Name (_DDN, "Audio Controller")
	Name (UUID, ToUUID ("A69F886E-6CEB-4594-A41F-7B5DCE24C553"))

	/* Device is D3 wake capable */
	Name (_S0W, 3)

	/* NHLT Table Address populated from GNVS values */
	Name (NBUF, ResourceTemplate () {
		QWordMemory (ResourceConsumer, PosDecode, MinFixed,
			     MaxFixed, NonCacheable, ReadOnly,
			     0, 0, 0, 0, 1,,, NHLT, AddressRangeACPI)
	})

	/*
	 * Device Specific Method
	 * Arg0 - UUID
	 * Arg1 - Revision
	 * Arg2 - Function Index
	 */
	Method (_DSM, 4)
	{
		If (Arg0 == ^UUID) {
			/*
			 * Function 0: Function Support Query
			 * Returns a bitmask of functions supported.
			 */
			If (Arg2 == 0) {
				/*
				 * NHLT Query only supported for revision 1 and
				 * if NHLT address and length are set in NVS.
				 */
				If (Arg1 == 1 && LNotEqual (NHLA, Zero) && LNotEqual (NHLL, Zero)) {
					Return (Buffer (One) { 0x03 })
				} Else {
					Return (Buffer (One) { 0x01 })
				}
			}

			/*
			 * Function 1: Query NHLT memory address used by
			 * Intel Offload Engine Driver to discover any non-HDA
			 * devices that are supported by the DSP.
			 *
			 * Returns a pointer to NHLT table in memory.
			 */
			If (Arg2 == 1) {
				CreateQWordField (NBUF, ^NHLT._MIN, NBAS)
				CreateQWordField (NBUF, ^NHLT._MAX, NMAS)
				CreateQWordField (NBUF, ^NHLT._LEN, NLEN)

				Store (NHLA, NBAS)
				Store (NHLA, NMAS)
				Store (NHLL, NLEN)

				Return (NBUF)
			}
		}

		Return (Buffer (One) { 0x00 })
	}
}
