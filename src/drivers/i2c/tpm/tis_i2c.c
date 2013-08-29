/*
 * Copyright 2013 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdint.h>
#include <string.h>
#include <device/i2c.h>
#include "tis.h"
#include "tpm.h"

static unsigned bus = 0;

static int opened;

void tis_set_i2c_bus(unsigned _bus)
{
	bus = _bus;
}

int tis_open(void)
{
	opened = 1;
	if (tpm_open(bus, CONFIG_DRIVER_TPM_I2C_ADDR)) {
		opened = 0;
		return -1;
	}
	return 0;
}

int tis_close(void)
{
	opened = 0;
	tpm_close();
	return 0;
}

int tis_init(void)
{
	int chip = CONFIG_DRIVER_TPM_I2C_ADDR;

	/*
	 * Probe TPM twice; the first probing might fail because TPM is asleep,
	 * and the probing can wake up TPM.
	 */
	uint8_t tmp;
	if (!bus)
		return -1;

	if (i2c_read(bus, chip, 0, 0, &tmp, sizeof(tmp)) &&
			i2c_read(bus, chip, 0, 0, &tmp, sizeof(tmp)))
		return -1;

	return 0;
}

int tis_sendrecv(const uint8_t *sendbuf, size_t sbuf_size,
		uint8_t *recvbuf, size_t *rbuf_len)
{
	uint8_t buf[TPM_BUFSIZE];

	if (!opened && tis_open())
		return -1;

	if (sizeof(buf) < sbuf_size)
		return -1;

	memcpy(buf, sendbuf, sbuf_size);

	int len = tpm_transmit(buf, sbuf_size);

	if (len < 10) {
		*rbuf_len = 0;
		return -1;
	}

	if (len > *rbuf_len) {
		*rbuf_len = len;
		return -1;
	}

	memcpy(recvbuf, buf, len);
	*rbuf_len = len;

	return 0;
}
