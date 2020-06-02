/* SPDX-License-Identifier: GPL-2.0-only */

// Intel SMBus Controller 0:1f.3

Device (SBUS)
{
	Name (_ADR, 0x001f0003)

#ifdef ENABLE_SMBUS_METHODS
	OperationRegion (SMBP, PCI_Config, 0x00, 0x100)
	Field(SMBP, DWordAcc, NoLock, Preserve)
	{
		Offset(0x40),
		,	2,
		I2CE,	1
	}

	OperationRegion (SMBI, SystemIO, SMBUS_IO_BASE, 0x20)
	Field (SMBI, ByteAcc, NoLock, Preserve)
	{
		HSTS,	8,	// Host Status
		,	8,
		HCNT,	8,	// Host Control
		HCMD,	8,	// Host Command
		TXSA,	8,	// Transmit Slave Address
		DAT0,	8,	// Host Data 0
		DAT1,	8,	// Host Data 1
		HBDB,	8,	// Host Block Data Byte
		PECK,	8,	// Packet Error Check
		RXSA,	8,	// Receive Slave Address
		RXDA,	16,	// Receive Slave Data
		AUXS,	8,	// Auxiliary Status
		AUXC,	8,	// Auxiliary Control
		SLPC,	8,	// SMLink Pin Control
		SBPC,	8,	// SMBus Pin Control
		SSTS,	8,	// Slave Status
		SCMD,	8,	// Slave Command
		NADR,	8,	// Notify Device Address
		NDLB,	8,	// Notify Data Low Byte
		NDLH,	8,	// Notify Data High Byte
	}

	// Kill all SMBus communication
	Method (KILL, 0, Serialized)
	{
		HCNT |= 0x02	// Send Kill
		HSTS |= 0xff	// Clean Status
	}

	// Check if last operation completed
	// return	Failure = 0, Success = 1
	Method (CMPL, 0, Serialized)
	{
		Local0 = 4000		// Timeout 200ms in 50us steps
		While (Local0) {
			If (HSTS & 0x02) {	// Completion Status?
				Return (1)	// Operation Completed
			} Else {
				Stall (50)
				Local0--
				If (Local0 == 0) {
					KILL()
				}
			}
		}

		Return (0)		//  Failure
	}


	// Wait for SMBus to become ready
	Method (SRDY, 0, Serialized)
	{
		Local0 = 200	// Timeout 200ms
		While (Local0) {
			If (HSTS & 0x40) {		// IN_USE?
				Sleep(1)		// Wait 1ms
				Local0--	// timeout--
				If (Local0 == 0) {
					Return (1)
				}
			} Else {
				Local0 = 0	// We're ready
			}
		}

		Local0 = 4000	// Timeout 200ms (50us * 4000)
		While (Local0) {
			If (HSTS & 0x01) {		// Host Busy?
				Stall(50)		// Wait 50us
				Local0--	// timeout--
				If (Local0 == 0) {
					KILL()
				}
			} Else {
				Return (0)		// Success
			}
		}

		Return (1)		// Failure
	}

	// SMBus Send Byte
	// Arg0:	Address
	// Arg1:	Data
	// Return:	1 = Success, 0=Failure

	Method (SSXB, 2, Serialized)
	{

		// Is the SMBus Controller Ready?
		If (SRDY()) {
			Return (0)
		}

		// Send Byte
		I2CE = 0		// SMBus Enable
		HSTS = 0xbf
		TXSA = Arg0	// Write Address
		HCMD = Arg1	// Write Data

		HCNT = 0x48	// Start + Byte Data Protocol

		If (CMPL()) {
			HSTS |= 0xff	// Clean up
			Return (1)		// Success
		}

		Return (0)
	}


	// SMBus Receive Byte
	// Arg0:	Address
	// Return:	0xffff = Failure, Data (8bit) = Success

	Method (SRXB, 2, Serialized)
	{

		// Is the SMBus Controller Ready?
		If (SRDY()) {
			Return (0xffff)
		}

		// Receive Byte
		I2CE = 0		// SMBus Enable
		HSTS = 0xbf
		TXSA = Arg0 | 1	// Write Address

		HCNT = 0x44	// Start

		If (CMPL()) {
			HSTS |= 0xff	// Clean up
			Return (DAT0)		// Success
		}

		Return (0xffff)
	}


	// SMBus Write Byte
	// Arg0:	Address
	// Arg1:	Command
	// Arg2:	Data
	// Return:	1 = Success, 0=Failure

	Method (SWRB, 3, Serialized)
	{

		// Is the SMBus Controller Ready?
		If (SRDY()) {
			Return (0)
		}

		// Send Byte
		I2CE = 0		// SMBus Enable
		HSTS = 0xbf
		TXSA = Arg0	// Write Address
		HCMD = Arg1	// Write Command
		DAT0 = Arg2	// Write Data

		HCNT = 0x48	// Start + Byte Protocol

		If (CMPL()) {
			HSTS |= 0xff	// Clean up
			Return (1)		// Success
		}

		Return (0)
	}


	// SMBus Read Byte
	// Arg0:	Address
	// Arg1:	Command
	// Return:	0xffff = Failure, Data (8bit) = Success

	Method (SRDB, 2, Serialized)
	{

		// Is the SMBus Controller Ready?
		If (SRDY()) {
			Return (0xffff)
		}

		// Receive Byte
		I2CE = 0			// SMBus Enable
		HSTS = 0xbf
		TXSA = Arg0 | 1	// Write Address
		HCMD = Arg1		// Command

		HCNT = 0x48		// Start

		If (CMPL()) {
			HSTS |= 0xff	// Clean up
			Return (DAT0)		// Success
		}

		Return (0xffff)
	}
#endif
}
