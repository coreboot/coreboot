/* SPDX-License-Identifier: GPL-2.0-only */

/* SMBUS Support */
Mutex (SBX0, 0x00)
OperationRegion (SMB0, SystemIO, 0xB00, 0x0C)
	Field (SMB0, ByteAcc, NoLock, Preserve) {
		HSTS,   8,  /* SMBUS status */
		SSTS,   8,  /* SMBUS slave status */
		HCNT,   8,  /* SMBUS control */
		HCMD,   8,  /* SMBUS host cmd */
		HADD,   8,  /* SMBUS address */
		DAT0,   8,  /* SMBUS data0 */
		DAT1,   8,  /* SMBUS data1 */
		BLKD,   8,  /* SMBUS block data */
		SCNT,   8,  /* SMBUS slave control */
		SCMD,   8,  /* SMBUS shadow cmd */
		SEVT,   8,  /* SMBUS slave event */
		SDAT,   8   /* SMBUS slave data */
}

Method (WCLR, 0, NotSerialized) { /* clear SMBUS status register */
	HSTS = 0x1e
	Local0 = 0xfa
	While ((HSTS & 0x1e) != 0) && (Local0 > 0)) {
		Stall (0x64)
		Local0--
	}

	Return (Local0)
}

Method (SWTC, 1, NotSerialized) {
	Local0 = Arg0
	Local2 = 0x07
	Local1 = 1
	While (Local1 == 1) {
		Local3 = HSTS & 0x1e
		If (Local3 != 0) { /* read success */
			If (Local3 != 0x02) {
				Local2 = 0
			}

			Local1 = 0
		}
		Else {
			If (Local0 < 0x0A) { /* read failure */
				Local2 = 0x10
				Local1 = 0
			}
			Else {
				Sleep (0x0A) /* 10 ms, try again */
				Local0 -= 0x0a
			}
		}
	}

	Return (Local2)
}

Method (SMBR, 3, NotSerialized) {
	Local0 = 0x07
	If (Acquire (SBX0, 0xFFFF) == 0) {
		Local0 = WCLR () /* clear SMBUS status register before read data */
		If (Local0 == 0) {
			Release (SBX0)
			Return (0x0)
		}

		HSTS = 0x1f
		HADD = (Arg1 << 1) | 1
		HCMD = Arg2
		If (Arg0 == 0x07) {
			HCNT = 0x48 /* read byte */
		}

		Local1 = SWTC (0x03E8) /* 1000 ms */
		If (Local1 == 0) {
			If (Arg0 == 0x07) {
				Local0 = DAT0
			}
		}
		Else {
			Local0 = Local1
		}

		Release (SBX0)
	}

	/* DBGO("the value of SMBusData0 register ") */
	/* DBGO(Arg2) */
	/* DBGO(" is ") */
	/* DBGO(Local0) */
	/* DBGO("\n") */

	Return (Local0)
}
