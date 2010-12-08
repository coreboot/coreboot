/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

// Intel SMBus Controller 0:1f.3

Device (SBUS)
{
	Name (_ADR, 0x001f0003)

	OperationRegion (SMBP, PCI_Config, 0x00, 0x100)
	Field(SMBP, DWordAcc, NoLock, Preserve)
	{
		Offset(0x40),
		,	2,
		I2CE,	1
	}

	OperationRegion (SMBI, SystemIO, 0x400, 0x20)
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

#ifdef ENABLE_SMBUS_METHODS
	// Kill all SMBus communication
	Method (KILL, 0, Serialized)
	{
		Or (HCNT, 0x02, HCNT)	// Send Kill
		Or (HSTS, 0xff, HSTS)	// Clean Status
	}

	// Check if last operation completed
	// return	Failure = 0, Success = 1
	Method (CMPL, 0, Serialized)
	{
		Store (4000, Local0)		// Timeout 200ms in 50us steps
		While (Local0) {
			If (And(HSTS, 0x02)) {	// Completion Status?
				Return (1)	// Operation Completed
			} Else {
				Stall (50)
				Decrement (Local0)
				If (LEqual(Local0, 0)) {
					KILL()
				}
			}
		}

		Return (0)		//  Failure
	}


	// Wait for SMBus to become ready
	Method (SRDY, 0, Serialized)
	{
		Store (200, Local0)	// Timeout 200ms
		While (Local0) {
			If (And(HSTS, 0x40)) {		// IN_USE?
				Sleep(1)		// Wait 1ms
				Decrement(Local0)	// timeout--
				If (LEqual(Local0, 0)) {
					Return (1)
				}
			} Else {
				Store (0, Local0)	// We're ready
			}
		}

		Store (4000, Local0)	// Timeout 200ms (50us * 4000)
		While (Local0) {
			If (And (HSTS, 0x01)) {		// Host Busy?
				Stall(50)		// Wait 50us
				Decrement(Local0)	// timeout--
				If (LEqual(Local0, 0)) {
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
		Store (0, I2CE)		// SMBus Enable
		Store (0xbf, HSTS)
		Store (Arg0, TXSA)	// Write Address
		Store (Arg1, HCMD)	// Write Data

		Store (0x48, HCNT)	// Start + Byte Data Protocol

		If (CMPL()) {
			Or (HSTS, 0xff, HSTS)	// Clean up
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
		Store (0, I2CE)		// SMBus Enable
		Store (0xbf, HSTS)
		Store (Or (Arg0, 1), TXSA)	// Write Address

		Store (0x44, HCNT)	// Start

		If (CMPL()) {
			Or (HSTS, 0xff, HSTS)	// Clean up
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
		Store (0, I2CE)		// SMBus Enable
		Store (0xbf, HSTS)
		Store (Arg0, TXSA)	// Write Address
		Store (Arg1, HCMD)	// Write Command
		Store (Arg2, DAT0)	// Write Data

		Store (0x48, HCNT)	// Start + Byte Protocol

		If (CMPL()) {
			Or (HSTS, 0xff, HSTS)	// Clean up
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
		Store (0, I2CE)			// SMBus Enable
		Store (0xbf, HSTS)
		Store (Or (Arg0, 1), TXSA)	// Write Address
		Store (Arg1, HCMD)		// Command

		Store (0x48, HCNT)		// Start

		If (CMPL()) {
			Or (HSTS, 0xff, HSTS)	// Clean up
			Return (DAT0)		// Success
		}

		Return (0xffff)
	}
#endif
}

