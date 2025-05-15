/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <ec/google/common/mec.h>
#include <stdint.h>

#include "ec.h"

/* MEC uses 0x800/0x804 as register/index pair, thus an 8-byte resource. */
#define MEC_EMI_BASE		0x800
#define MEC_EMI_SIZE		8

/* For MEC, access ranges 0x800 thru 0x9ff using EMI interface instead of LPC */
#define MEC_EMI_RANGE_START EC_HOST_CMD_REGION0
#define MEC_EMI_RANGE_END   (EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SIZE)

bool chipset_emi_read_bytes(u16 port, size_t length, u8 *dest, u8 *csum)
{
	/* Access desired range though EMI interface */
	if (port >= MEC_EMI_RANGE_START && port <= MEC_EMI_RANGE_END) {
		u8 ret = mec_io_bytes(MEC_IO_READ, MEC_EMI_BASE, port - MEC_EMI_RANGE_START,
				      dest, length);
		if (csum)
			*csum += ret;
		return true;
	}
	return false;
}

bool chipset_emi_write_bytes(u16 port, size_t length, u8 *msg, u8 *csum)
{
	/* Access desired range though EMI interface */
	if (port >= MEC_EMI_RANGE_START && port <= MEC_EMI_RANGE_END) {
		u8 ret = mec_io_bytes(MEC_IO_WRITE, MEC_EMI_BASE, port - MEC_EMI_RANGE_START,
				      msg, length);
		if (csum)
			*csum += ret;
		return true;
	}
	return false;
}

void chipset_ioport_range(uint16_t *base, size_t *size)
{
	*base = MEC_EMI_BASE;
	*size = MEC_EMI_SIZE;
}
