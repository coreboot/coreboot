/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/dram/rcd.h>
#include <endian.h>
#include <lib.h>

/**
 * Registering Clock Driver (RCD) is responsible for driving address and control
 * nets on RDIMM and LRDIMM applications. Its operation is configurable by a set
 * of Register Control Words (RCWs). There are two ways of accessing RCWs:
 * in-band on the memory channel as an MRS commands ("MR7") or through I2C.
 *
 * From JESD82-31: "For changes to the control word setting, (...) the
 * controller needs to wait tMRD after _the last control word access_, before
 * further access _to the DRAM_ can take place". MRS is passed to rank 0 of the
 * DRAM, but MR7 is reserved so it is ignored by DRAM. tMRD (8nCK) applies here,
 * unless longer delay is needed for RCWs which control the clock timing (see
 * JESD82-31 for list of such). This makes sense from DRAMs point of view,
 * however we are talking to the Registering Clock Driver (RCD), not DRAM. From
 * parts marked in the sentence above one may assume that only one delay at the
 * end is necessary and RCWs can be written back to back; however, in the same
 * document in table 141 tMRD is defined as "Number of clock cycles between two
 * control word accesses, MRS accesses, or any DRAM commands".
 *
 * I2C access to RCWs is required to support byte (8b), word (16b) and double
 * word (32b) write size. Bigger blocks are not required. Reads must always be
 * 32b, 32b-aligned blocks, even when reading just one RCW. RCD ignores the two
 * lowest bits so unaligned accesses would return shifted values. RCWs are
 * tightly packed in I2C space, so it is not possible to write just one 4b RCW
 * without writing its neighbor. This is especially important for F0RC06,
 * Command Space Control Word, as it is able to reset the state of RCD. For this
 * reason, the mentioned register has NOP command (all 1's). JESD82-31 does not
 * specify timeouts required for such multi-RCWs writes, or any other writes.
 * These are not MRS accesses, so it would be strange to apply those timeouts.
 * Perhaps only the registers that actually change the clock settings require
 * time to stabilize. On the other hand, I2C is relatively slow, so it is
 * possible that the write itself is long enough.
 *
 * RCD I2C address is 0xBx (or 0x58 + DIMM number, depending on convention), it
 * is located on the same bus as SPD. It uses a bus command encoding, see
 * section 3.3 in JESD82-31 for description of reading and writing register
 * values.
 *
 * This file includes only functions for access through I2C - it is generic,
 * while MRS commands are passed to memory controller registers in an
 * implementation specific way.
 */

#define RCD_CMD_BEGIN		0x80
#define RCD_CMD_END		0x40
#define RCD_CMD_PEC		0x10
#define RCD_CMD_RD_DWORD	0x00
#define RCD_CMD_WR_BYTE		0x04
#define RCD_CMD_WR_WORD		0x08
#define RCD_CMD_WR_DWORD	0x0C
#define RCD_CMD_BUS_BYTE	0x00
#define RCD_CMD_BUS_BLOCK	0x02

/* Shorthand for block transfers */
#define RCD_CMD_BLOCK	(RCD_CMD_BEGIN | RCD_CMD_END | RCD_CMD_BUS_BLOCK)

/* Excluding size of data */
#define RCD_CMD_BYTES	4

/* Use byte fields to get rid of endianness issues. */
struct rcd_i2c_cmd {
	uint8_t cmd;
	uint8_t bytes;  /* From next byte up to PEC (excluding) */
	uint8_t reserved;
	uint8_t devfun;
	uint8_t reg_h;
	uint8_t reg_l;
	union {  /* Not used for reads, can use 1, 2 or 4 for writes */
		uint8_t bdata;
		uint32_t ddata;
	};
	/* Optional PEC */
} __packed;

#define RCD_STS_SUCCESS			0x01
#define RCD_STS_INTERNAL_TARGET_ABORT	0x10

/* Always 4 bytes data + status (for block commands) */
#define RCD_RSP_BYTES	5

struct rcd_i2c_rsp {
	uint8_t bytes;  /* From next byte up to PEC (excluding) */
	uint8_t status;
	union {
		uint8_t bdata;
		uint32_t ddata;
	};
	/* Optional PEC */
} __packed;

/* Reads a register storing its value in the host's byte order. Returns non-zero on success. */
static int rcd_readd(unsigned int bus, uint8_t slave, uint8_t reg, uint32_t *data)
{
	struct i2c_msg seg[2];
	struct rcd_i2c_cmd cmd = {
		.cmd = RCD_CMD_BLOCK | RCD_CMD_RD_DWORD,
		.bytes = RCD_CMD_BYTES,
		.reg_l = reg
	};
	struct rcd_i2c_rsp rsp = { 0xaa, 0x55 };

	seg[0].flags = 0;
	seg[0].slave = slave;
	seg[0].buf   = (uint8_t *)&cmd;
	seg[0].len   = cmd.bytes + 2;  /* + .cmd and .bytes fields */

	i2c_transfer(bus, seg, 1);

	seg[0].len   = 1;	/* Send just the command again */
	seg[1].flags = I2C_M_RD;
	seg[1].slave = slave;
	seg[1].buf   = (uint8_t *)&rsp;
	seg[1].len   = RCD_RSP_BYTES + 1;  /* + .bytes field */

	i2c_transfer(bus, seg, ARRAY_SIZE(seg));

	/* Data is sent MSB to LSB, i.e. higher registers to lower. */
	*data = be32toh(rsp.ddata);

	return rsp.status == RCD_STS_SUCCESS;
}

static int rcd_writed(unsigned int bus, uint8_t slave, uint8_t reg, uint32_t data)
{
	struct i2c_msg seg;
	struct rcd_i2c_cmd cmd = {
		.cmd = RCD_CMD_BLOCK | RCD_CMD_WR_DWORD,
		.bytes = RCD_CMD_BYTES + sizeof(data),
		.reg_l = reg,
		/* Data is sent MSB to LSB, i.e. higher registers to lower. */
		.ddata = htobe32(data)
	};

	seg.flags = 0;
	seg.slave = slave;
	seg.buf   = (uint8_t *)&cmd;
	seg.len   = cmd.bytes + 2;  /* + .cmd and .bytes fields */

	return i2c_transfer(bus, &seg, 1);
}

static int rcd_writeb(unsigned int bus, uint8_t slave, uint8_t reg, uint8_t data)
{
	struct i2c_msg seg;
	struct rcd_i2c_cmd cmd = {
		.cmd = RCD_CMD_BLOCK | RCD_CMD_WR_BYTE,
		.bytes = RCD_CMD_BYTES + sizeof(data),
		.reg_l = reg,
		.bdata = data
	};

	seg.flags = 0;
	seg.slave = slave;
	seg.buf   = (uint8_t *)&cmd;
	seg.len   = cmd.bytes + 2;  /* + .cmd and .bytes fields */

	return i2c_transfer(bus, &seg, 1);
}

int rcd_write_reg(unsigned int bus, uint8_t slave, enum rcw_idx reg,
		  uint8_t data)
{
	if (reg < F0RC00_01 || reg > F0RCFx) {
		printk(BIOS_ERR, "Trying to write to illegal RCW %#2.2x\n",
		       reg);
		return 0;
	}

	return rcd_writeb(bus, slave, reg, data);
}

int rcd_write_32b(unsigned int bus, uint8_t slave, enum rcw_idx reg,
		  uint32_t data)
{
	if (reg < F0RC00_01 || reg > F0RCFx) {
		printk(BIOS_ERR, "Trying to write to illegal RCW %#2.2x\n",
		       reg);
		return 0;
	}

	if (reg & 3) {
		/*
		 * RCD would silently mask out the lowest bits, assume that this
		 * is not what caller wanted.
		 */
		printk(BIOS_ERR, "Unaligned RCW %#2.2x, aborting\n", reg);
		return 0;
	}

	return rcd_writed(bus, slave, reg, data);
}

void dump_rcd(unsigned int bus, u8 addr)
{
	/* Can only read in 32b chunks */
	uint8_t buf[RCW_ALL_ALIGNED];
	int i;

	for (i = 0; i < RCW_ALL_ALIGNED; i += sizeof(uint32_t)) {
		uint32_t data;
		if (!rcd_readd(bus, addr, i, &data)) {
			printk(BIOS_ERR, "Failed to read RCD (%d-%02x) at offset %#2.2x\n",
			       bus, addr, i);
			return;
		}
		/* We want to dump memory the way it's stored, so make sure it's in LE. */
		*(uint32_t *)&buf[i] = htole32(data);
	}

	printk(BIOS_DEBUG, "RCD dump for I2C address %#2.2x:\n", addr);
	hexdump(buf, sizeof(buf));
}
