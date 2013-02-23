/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define SMBUS_BASE_ADDR  0xB00
#define DIMENSION(array)(sizeof (array)/ sizeof (array [0]))

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
#define LTC4305_SMBUS_ADDR       0x94

typedef struct _DIMM_INFO_SMBUS{
	UINT8   SocketId;
	UINT8   MemChannelId;
	UINT8   DimmId;
	UINT8   SmbusAddress;
} DIMM_INFO_SMBUS;
/*
 * SPD address table - porting required
 */
STATIC CONST DIMM_INFO_SMBUS SpdAddrLookup [] =
{
	/* Socket, Channel, Dimm, Smbus */
	{0, 0, 0, 0xAC},
	{0, 0, 1, 0xAE},
	{0, 1, 0, 0xA8},
	{0, 1, 1, 0xAA},
	{0, 2, 0, 0xA4},
	{0, 2, 1, 0xA6},
	{0, 3, 0, 0xA0},
	{0, 3, 1, 0xA2},
	{1, 0, 0, 0xAC},
	{1, 0, 1, 0xAE},
	{1, 1, 0, 0xA8},
	{1, 1, 1, 0xAA},
	{1, 2, 0, 0xA4},
	{1, 2, 1, 0xA6},
	{1, 3, 0, 0xA0},
	{1, 3, 1, 0xA2}
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
AmdMemoryReadSPD (
		IN UINT32 Func,
		IN UINT32 Data,
		IN OUT AGESA_READ_SPD_PARAMS *SpdData
		);

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

STATIC
VOID
WritePmReg (
		IN UINT8 Reg,
		IN UINT8 Data
	   )
{
	__outbyte (0xCD6, Reg);
	__outbyte (0xCD7, Data);
}
STATIC
VOID
SetupFch (
		IN UINT16
		IN IoBase
	 )
{

	AMD_CONFIG_PARAMS         StdHeader;
	UINT32                    PciData32;
	UINT8                     PciData8;
	PCI_ADDR                  PciAddress;

	/* Set SMBUS MMIO. */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 20, 0, 0x90);
	PciData32 = (SMBUS_BASE_ADDR & 0xFFFFFFF0) | BIT0;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData32, &StdHeader);

	/* Enable SMBUS MMIO. */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 20, 0, 0xD2);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData8, &StdHeader); ;
	PciData8 |= BIT0;
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData8, &StdHeader);
	/* set SMBus clock to 400 KHz */
	__outbyte (IoBase + 0x0E, 66000000 / 400000 / 4);
}

/*
 *
 * ReadSmbusByteData - read a single SPD byte from any offset
 *
 */

STATIC
AGESA_STATUS
ReadSmbusByteData (
		IN UINT16 Iobase,
		IN UINT8  Address,
		OUT UINT8 *ByteData,
		IN UINTN  Offset
		)
{
	UINTN  Status;
	UINT64 Limit;

	Address |= 1; 																// set read bit

	__outbyte (Iobase + 0, 0xFF);                // clear error status
	__outbyte (Iobase + 1, 0x1F);                // clear error status
	__outbyte (Iobase + 3, Offset);              // offset in eeprom
	__outbyte (Iobase + 4, Address);             // slave address and read bit
	__outbyte (Iobase + 2, 0x48);                // read byte command

	/* time limit to avoid hanging for unexpected error status (should never happen) */
	Limit = __rdtsc () + 2000000000 / 10;
	for (;;) {
		Status = __inbyte (Iobase);
		if (__rdtsc () > Limit) break;
		if ((Status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((Status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	*ByteData = __inbyte (Iobase + 5);
	if (Status == 2) Status = 0;                      // check for done with no errors
	return Status;
}
/*
 *
 * WriteSmbusByteData - Write a single SPD byte onto any offset
 *
 */
STATIC
AGESA_STATUS
WriteSmbusByteData (
		IN UINT16 Iobase,
		IN UINT8  Address,
		IN UINT8  ByteData,
		IN UINTN  Offset
		)
{
	UINTN  Status;
	UINT64 Limit;
	Address &= 0xFE; 														// set write bit

	__outbyte (Iobase + 0, 0xFF);                // clear error status
	__outbyte (Iobase + 1, 0x1F);                // clear error status
	__outbyte (Iobase + 3, Offset);              // offset in eeprom
	__outbyte (Iobase + 4, Address);   					// slave address and write bit
	__outbyte (Iobase + 5, ByteData);            // offset in byte data                                             //
	__outbyte (Iobase + 2, 0x48);                // write byte command
	/* time limit to avoid hanging for unexpected error status (should never happen) */
	Limit = __rdtsc () + 2000000000 / 10;
	for (;;) {
		Status = __inbyte (Iobase);
		if (__rdtsc () > Limit) break;
		if ((Status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((Status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}
	if (Status == 2) Status = 0;                      // check for done with no errors
	return Status;
}

/*
 *
 * ReadSmbusByte - read a single SPD byte from the default offset
 *                 this function is faster function readSmbusByteData
 *
 */

STATIC
AGESA_STATUS
ReadSmbusByte (
		IN UINT16 Iobase,
		IN UINT8  Address,
		OUT UINT8 *Buffer
	      )
{
	UINTN   Status;
	UINT64  Limit;

	__outbyte (Iobase + 0, 0xFF);                // clear error status
	__outbyte (Iobase + 1, 0x1F);                // clear error status
	__outbyte (Iobase + 2, 0x44);                // read command

	// time limit to avoid hanging for unexpected error status
	Limit = __rdtsc () + 2000000000 / 10;
	for (;;) {
		Status = __inbyte (Iobase);
		if (__rdtsc () > Limit) break;
		if ((Status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((Status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	Buffer [0] = __inbyte (Iobase + 5);
	if (Status == 2) Status = 0;                      // check for done with no errors
	return Status;
}

/*
 *
 * ReadSpd - Read one or more SPD bytes from a DIMM.
 *           Start with offset zero and read sequentially.
 *           Optimization relies on autoincrement to avoid
 *           sending offset for every byte.
 *           Reads 128 bytes in 7-8 ms at 400 KHz.
 *
 */

STATIC
AGESA_STATUS
ReadSpd (
		IN UINT16 IoBase,
		IN UINT8  SmbusSlaveAddress,
		OUT UINT8 *Buffer,
		IN UINTN  Count
	)
{
	UINTN Index, Status;

	/* read the first byte using offset zero */
	Status = ReadSmbusByteData (IoBase, SmbusSlaveAddress, Buffer, 0);
	if (Status) return Status;

	/* read the remaining bytes using auto-increment for speed */
	for (Index = 1; Index < Count; Index++){
		Status = ReadSmbusByte (IoBase, SmbusSlaveAddress, &Buffer [Index]);
		if (Status) return Status;
	}
	return 0;
}

AGESA_STATUS
AmdMemoryReadSPD (
		IN UINT32 Func,
		IN UINT32 Data,
		IN OUT AGESA_READ_SPD_PARAMS *SpdData
		)
{
	AGESA_STATUS Status;
	UINT8  SmBusAddress = 0;
	UINTN  Index;
	UINTN  MaxSocket = DIMENSION (SpdAddrLookup);

	for (Index = 0; Index < MaxSocket; Index ++){
		if ((SpdData->SocketId     == SpdAddrLookup[Index].SocketId)     &&
				(SpdData->MemChannelId == SpdAddrLookup[Index].MemChannelId) &&
				(SpdData->DimmId       == SpdAddrLookup[Index].DimmId)) {
			SmBusAddress = SpdAddrLookup[Index].SmbusAddress;
			break;
		}
	}


	if (SmBusAddress == 0) return AGESA_ERROR;

	SetupFch (SMBUS_BASE_ADDR);

	Status = WriteSmbusByteData (SMBUS_BASE_ADDR, LTC4305_SMBUS_ADDR, 0x80, 0x03);

	switch (SpdData->SocketId) {
		case 0:
			/* Switch onto the  First CPU Socket SMBUS */
			WriteSmbusByteData (SMBUS_BASE_ADDR, LTC4305_SMBUS_ADDR, 0x80, 0x03);
			break;
		case 1:
			/* Switch onto the  Second CPU Socket SMBUS */
			WriteSmbusByteData (SMBUS_BASE_ADDR, LTC4305_SMBUS_ADDR, 0x40, 0x03);
			break;
		default:
			/* Switch off two CPU Sockets SMBUS */
			WriteSmbusByteData (SMBUS_BASE_ADDR, LTC4305_SMBUS_ADDR, 0x00, 0x03);
			break;
	}
	Status =  ReadSpd (SMBUS_BASE_ADDR, SmBusAddress, SpdData->Buffer, 256);

	/*Output SPD Debug Message*/
	printk(BIOS_EMERG, "file '%s',line %d, %s()\n", __FILE__, __LINE__, __func__);
	printk(BIOS_DEBUG, " Status = %d\n",Status);
	printk(BIOS_DEBUG, "SocketId MemChannelId SpdData->DimmId SmBusAddress Buffer\n");
	printk(BIOS_DEBUG, "%x, %x, %x, %x, %x\n", SpdData->SocketId, SpdData->MemChannelId, SpdData->DimmId, SmBusAddress, SpdData->Buffer);

	/* Switch off two CPU Sockets SMBUS */
	WriteSmbusByteData (SMBUS_BASE_ADDR, LTC4305_SMBUS_ADDR, 0x00, 0x03);
	return Status;
}
