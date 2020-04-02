/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/*
 * This device provides an ACPI interface to read VPD keys from either
 * the RO_VPD or RW_VPD region.  For example:
 *
 *     VPD.VPDF ("RO", "ro_key_name")
 *     VPD.VPDF ("RW", "rw_key_name")
 */

Device (VPD)
{
	Name (_HID, "GOOG000F")
	Name (_UID, 1)
	Name (_STA, 0xf)

	Name (VOFS, 0x600)	/* Start of VPD header in VPD region */
	Name (VIHL, 0x10)	/* Length of VPD info header */
	Name (VPET, 0x00)	/* VPD Entry Type: Terminator */
	Name (VPES, 0x01)	/* VPD Entry Type: String */
	Name (VPEI, 0xfe)	/* VPD Entry Type: Info (header) */
	Name (MORE, 0x80)	/* Bit to indicate more length bytes */

	Name (VPTR, Zero)	/* Pointer to current byte in VPD for parser */
	Name (VEND, Zero)	/* End of VPD region */

	/*
	 * VLOC() - Return location and length of VPD region in memory.
	 *          These values must be initialized in GNVS by coreboot.
	 *
	 * Returns: Package indicating start and length of region:
	 *   [0] = Address of the start of VPD region.
	 *   [1] = Length of the VPD region.
	 */
	Method (VLOC, 1, Serialized)
	{
		Switch (ToString (Arg0))
		{
			Case ("RO") {
				Return (Package () { \ROVP, \ROVL })
			}
			Case ("RW") {
				Return (Package () { \RWVP, \RWVL })
			}
			Default {
				Return (Package () { Zero, Zero })
			}
		}
	}

	/*
	 * VVPD() - Verify VPD info header.
	 * Arg0: VPD partition base address.
	 * Returns: VPD length or Zero if VPD header is not valid.
	 */
	Method (VVPD, 1, Serialized)
	{
		Local0 = Arg0 + ^VOFS

		OperationRegion (VPDH, SystemMemory, Local0, ^VIHL)
		Field (VPDH, DWordAcc, NoLock, Preserve)
		{
			TYPE, 8,      /* VPD Header Tag (=0xfe) */
			KLEN, 8,      /* Key length (=9) */
			IVER, 8,      /* Info version (=1) */
			SIGN, 64,     /* Signature (="gVpdInfo") */
			VLEN, 8,      /* Value length (=4) */
			SIZE, 32,     /* VPD length */
		}

		If (TYPE != ^VPEI) {
			Return (Zero)
		}
		If (KLEN != 9) {
			Return (Zero)
		}
		If (IVER != 1) {
			Return (Zero)
		}
		If (ToString (SIGN) != "gVpdInfo") {
			Return (Zero)
		}
		If (VLEN != 4) {
			Return (Zero)
		}

		Return (SIZE)
	}

	/* Return next byte from VPD at pointer VPTR, and increment VPTR. */
	Method (VPRB, 0, Serialized)
	{
		If (^VPTR > ^VEND) {
			Printf ("Access beyond end of VPD region")
			Return (Zero)
		}

		Local0 = ^VPTR
		OperationRegion (VPDR, SystemMemory, Local0, One)
		Field (VPDR, DWordAcc, NoLock, Preserve)
		{
			BYTE, 8,
		}

		/* Increment address pointer */
		^VPTR++
		Return (BYTE)
	}

	/* Extract and return next string from VPD. */
	Method (VPDE, 0, Serialized)
	{
		Local0 = One	/* Indicates if there are more bytes */
		Local1 = Zero	/* Length */

		/* Decode the string length */
		While (Local0) {
			/* Read the next byte at indicated address */
			Local2 = ^VPRB ()

			/* Update the more bit from the byte in Local2 */
			Local0 = Local2 >> 7

			/* Save the length bits from Local2 */
			Local1 <<= 7
			Local1 |= Local2 & 0x7f
		}
		If (Local1 == Zero) {
			Return (Zero)
		}

		/* Extract the string */
		Local3 = Zero
		Local4 = ""
		While (Local3 < Local1) {
			Concatenate (Local4, ToString (^VPRB ()), Local4)
			Local3++
		}

		Return (Local4)
	}

	/*
	 * VPDS() - Find next VPD key and value.
	 * Returns: Package containing key and value:
	 *   [0] = VPD key string
	 *   [1] = VPD value string
	 */
	Method (VPDS, 0, Serialized)
	{
		Name (VPKV, Package () { "", "" })

		/* Read the VPD type and ensure it is a string */
		If (^VPRB () != ^VPES) {
			Printf ("VPDS: Type is not a string")
			Return (VPKV)
		}

		/* Extract the key string and value */
		VPKV[0] = VPDE ()
		VPKV[1] = VPDE ()

		Return (VPKV)
	}

	/*
	 * VPDF() - Find VPD key with matching name.
	 * Arg0: VPD Partition, either "RO" or "RW".
	 * Arg1: VPD key name to search for.
	 * Returns: VPD string corresponding to VPD key, or Zero if not found.
	 */
	Method (VPDF, 2, Serialized)
	{
		Local0 = VLOC (Arg0)

		/* Start of VPD region */
		^VPTR = DerefOf (Local0[0])

		/* End address of VPD region */
		^VEND = ^VPTR + DerefOf (Local0[1])

		If (^VPTR == Zero || ^VEND == Zero) {
			Printf ("Unable to find VPD region")
			Return (Zero)
		}

		/* Verify VPD info header and save size */
		Local0 = VVPD (^VPTR)
		If (Local0 == Zero) {
			Printf ("VPD region %o did not verify", Arg0)
			Return (Zero)
		}

		/* Set VPD pointer to start of VPD entries */
		^VPTR += ^VOFS + ^VIHL

		/* Search through VPD entries until key is found */
		Local1 = ""
		While (Local1 != ToString (Arg1)) {
			Local2 = VPDS ()
			Local1 = DerefOf (Local2[0])
			If (Local1 == "") {
				Printf ("VPD KEY %o not found", Arg1)
				Return (Zero)
			}
		}
		Local3 = DerefOf (Local2[1])

		Printf ("Found VPD KEY %o = %o", Local1, Local3)
		Return (Local3)
	}
}
