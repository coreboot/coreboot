/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
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

/*
 * ACPI style embedded controller commands
 *
 * Controlled by the following preprocessor defines:
 * EC_SC_IO	I/o address of the EC_SC register
 * EC_DATA_IO	I/o address of the EC_DATA register
 */

#define EC_MUTEX	ECMX
#define WAIT_EC_SC	WECC
#define SEND_EC_COMMAND	SECC
#define SEND_EC_DATA	SECD
#define RECV_EC_DATA	RECD
#define EC_READ		ECRD
#define EC_WRITE	ECWR
#define EC_SC		ECSC
#define EC_DATA		ECDT

#define EC_OBF		0x01 /* Output buffer full (EC_DATA) */
#define EC_IBF		0x02 /* Input buffer full (EC_DATA or EC_SC) */

#define EC_ERROR_MASK	0xff00
#define EC_TIMEOUT	0x8000

#define EC_READ_CMD	0x80
#define EC_WRITE_CMD	0x81

Mutex(EC_MUTEX, 1)

OperationRegion(ERSC, SystemIO, EC_SC_IO, 1)
Field(ERSC, ByteAcc, NoLock, Preserve) { EC_SC, 8 }
OperationRegion(ERDT, SystemIO, EC_DATA_IO, 1)
Field(ERDT, ByteAcc, NoLock, Preserve) { EC_DATA, 8 }

/*
 * Wait for a bit in the status and command (EC_SC) register
 *
 * The caller is responsible of acquiring the EC_MUTEX before
 * calling this method.
 *
 * Arg0: Mask, Arg1: State waiting for
 * Returns EC_TIMEOUT if timed out, 0 else
 */
Method (WAIT_EC_SC, 2)
{
	Store (0x7ff, Local0) /* Timeout */
	While (LAnd (LNotEqual (And (EC_SC, Arg0), Arg1), Decrement (Local0))) {
		Stall (10)
	}
	If (Local0) {
		Return (0)
	} Else {
		Return (EC_TIMEOUT)
	}
}

/*
 * Send command byte in Arg0 to status and command (EC_SC) register
 *
 * The caller is responsible of acquiring the EC_MUTEX before
 * calling this method.
 *
 * Returns EC_TIMEOUT if timed out, 0 else
 */
Method (SEND_EC_COMMAND, 1)
{
	Store (WAIT_EC_SC (EC_IBF, 0), Local0)
	If (LNot (Local0)) {
		Store (Arg0, EC_SC)
	}
	Return (Local0)
}

/*
 * Send data byte in Arg0 to data (EC_DATA) register
 *
 * The caller is responsible of acquiring the EC_MUTEX before
 * calling this method.
 *
 * Returns EC_TIMEOUT if timed out, 0 else
 */
Method (SEND_EC_DATA, 1)
{
	Store (WAIT_EC_SC (EC_IBF, 0), Local0)
	If (LNot (Local0)) {
		Store (Arg0, EC_DATA)
	}
	Return (Local0)
}

/*
 * Read one byte of data from data (EC_DATA) register
 *
 * The caller is responsible of acquiring the EC_MUTEX before
 * calling this method.
 *
 * Returns EC_TIMEOUT if timed out, the read data byte else
 */
Method (RECV_EC_DATA)
{
	Store (WAIT_EC_SC (EC_OBF, EC_OBF), Local0)
	If (LNot (Local0)) {
		Return (EC_DATA)
	} Else {
		Return (Local0)
	}
}

/*
 * Read one byte from ec memory (cmd 0x80)
 *
 * Arg0: Address (1 byte) to read from
 * Returns EC_TIMEOUT if timed out, the read data byte else
 */
Method (EC_READ, 1)
{
	Acquire (EC_MUTEX, 0xffff)
	Store (SEND_EC_COMMAND (EC_READ_CMD), Local0)
	If (LNot (Local0)) {
		Store (SEND_EC_DATA (Arg0), Local0)
	}
	If (LNot (Local0)) {
		Store (RECV_EC_DATA (), Local0)
	}
	Release (EC_MUTEX)

	Return (Local0)
}

/*
 * Write one byte to ec memory (cmd 0x81)
 *
 * Arg0: Address (1 byte) to write to
 * Arg1: Byte to write
 * Returns EC_TIMEOUT if timed out, 0 else
 */
Method (EC_WRITE, 2)
{
	Acquire (EC_MUTEX, 0xffff)
	Store (SEND_EC_COMMAND (EC_WRITE_CMD), Local0)
	If (LNot (Local0)) {
		Store (SEND_EC_DATA (Arg0), Local0)
	}
	If (LNot (Local0)) {
		Store (SEND_EC_DATA (Arg1), Local0)
	}
	Release (EC_MUTEX)

	Return (Local0)
}
