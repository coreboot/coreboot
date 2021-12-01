/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>

#include "i2ec.h"

#define SIO_DEV		PNP_DEV(0x2e, 0)

/* SIO depth 2 index/data pair */
#define D2ADR		0x2e
#define D2DAT		0x2f

/* SIO depth 2 address space */
#define I2EC_ADDR_L	0x10
#define I2EC_ADDR_H	0x11
#define I2EC_DATA	0x12

/*
 * Read/write SIO "depth 2" registers
 */

static uint8_t sio_d2_read(uint8_t addr)
{
	pnp_write_config(SIO_DEV, D2ADR, addr);
	return pnp_read_config(SIO_DEV, D2DAT);
}

static void sio_d2_write(uint8_t addr, uint8_t val)
{
	pnp_write_config(SIO_DEV, D2ADR, addr);
	pnp_write_config(SIO_DEV, D2DAT, val);
}

/*
 * Read/write I2EC registers through SIO "depth 2" address space
 */

uint8_t ec_d2i2ec_read(uint16_t addr)
{
	sio_d2_write(I2EC_ADDR_H, addr >> 8 & 0xff);
	sio_d2_write(I2EC_ADDR_L, addr      & 0xff);
	return sio_d2_read(I2EC_DATA);
}

void ec_d2i2ec_write(uint16_t addr, uint8_t val)
{
	sio_d2_write(I2EC_ADDR_H, addr >> 8 & 0xff);
	sio_d2_write(I2EC_ADDR_L, addr      & 0xff);
	sio_d2_write(I2EC_DATA, val);
}
