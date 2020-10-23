/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <soc/iobp.h>
#include <soc/rcba.h>

#define IOBP_RETRY 1000

static inline int iobp_poll(void)
{
	unsigned int try;

	for (try = IOBP_RETRY; try > 0; try--) {
		u16 status = RCBA16(IOBPS);
		if ((status & IOBPS_READY) == 0)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "IOBP: timeout waiting for transaction to complete\n");
	return 0;
}

u32 pch_iobp_read(u32 address)
{
	u16 status;

	if (!iobp_poll())
		return 0;

	/* Set the address */
	RCBA32(IOBPIRI) = address;

	/* READ OPCODE */
	status = RCBA16(IOBPS);
	status &= ~IOBPS_MASK;
	status |= IOBPS_READ;
	RCBA16(IOBPS) = status;

	/* Undocumented magic */
	RCBA16(IOBPU) = IOBPU_MAGIC;

	/* Set ready bit */
	status = RCBA16(IOBPS);
	status |= IOBPS_READY;
	RCBA16(IOBPS) = status;

	if (!iobp_poll())
		return 0;

	/* Check for successful transaction */
	status = RCBA16(IOBPS);
	if (status & IOBPS_TX_MASK) {
		printk(BIOS_ERR, "IOBP: read 0x%08x failed\n", address);
		return 0;
	}

	/* Read IOBP data */
	return RCBA32(IOBPD);
}

void pch_iobp_write(u32 address, u32 data)
{
	u16 status;

	if (!iobp_poll())
		return;

	/* Set the address */
	RCBA32(IOBPIRI) = address;

	/* WRITE OPCODE */
	status = RCBA16(IOBPS);
	status &= ~IOBPS_MASK;
	status |= IOBPS_WRITE;
	RCBA16(IOBPS) = status;

	RCBA32(IOBPD) = data;

	/* Undocumented magic */
	RCBA16(IOBPU) = IOBPU_MAGIC;

	/* Set ready bit */
	status = RCBA16(IOBPS);
	status |= IOBPS_READY;
	RCBA16(IOBPS) = status;

	if (!iobp_poll())
		return;

	/* Check for successful transaction */
	status = RCBA16(IOBPS);
	if (status & IOBPS_TX_MASK) {
		printk(BIOS_ERR, "IOBP: write 0x%08x failed\n", address);
		return;
	}

	printk(BIOS_SPEW, "IOBP: set 0x%08x to 0x%08x\n", address, data);
}

void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue)
{
	u32 data = pch_iobp_read(address);

	/* Update the data */
	data &= andvalue;
	data |= orvalue;

	pch_iobp_write(address, data);
}

void pch_iobp_exec(u32 addr, u16 op_code, u8 route_id, u32 *data, u8 *resp)
{
	if (!data || !resp)
		return;

	*resp = -1;
	if (!iobp_poll())
		return;

	/* RCBA2330[31:0] = Address */
	RCBA32(IOBPIRI) = addr;
	/* RCBA2338[15:8] = opcode */
	RCBA16(IOBPS) = (RCBA16(IOBPS) & 0x00ff) | op_code;
	/* RCBA233A[15:8] = 0xf0 RCBA233A[7:0] = Route ID */
	RCBA16(IOBPU) = IOBPU_MAGIC | route_id;

	if (op_code == IOBP_PCICFG_WRITE)
		RCBA32(IOBPD) = *data;
	/* Set RCBA2338[0] to trigger IOBP transaction*/
	RCBA16(IOBPS) = RCBA16(IOBPS) | 0x1;

	if (!iobp_poll())
		return;

	*resp = (RCBA16(IOBPS) & IOBPS_TX_MASK) >> 1;
	*data = RCBA32(IOBPD);
}
