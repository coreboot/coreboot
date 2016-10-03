/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <delay.h>
#include "ec.h"
#include "ec_commands.h"
#include <spi-generic.h>
#include <timer.h>

/* This is assuming that this driver is not used on x86. If that changes, this
   might need to become a CAR_GLOBAL or maybe even more complicated. */
static struct stopwatch cs_cooldown_sw;
static const long cs_cooldown_us = 200;

static const uint8_t EcFramingByte = 0xec;

#define PROTO3_MAX_PACKET_SIZE 268

static uint8_t req_buf[PROTO3_MAX_PACKET_SIZE];
static uint8_t resp_buf[PROTO3_MAX_PACKET_SIZE];

void *crosec_get_buffer(size_t size, int req)
{
	if (size > PROTO3_MAX_PACKET_SIZE) {
		printk(BIOS_DEBUG, "Proto v3 buffer request too large: %zu!\n",
			size);
		return NULL;
	}

	if (req)
		return req_buf;
	else
		return resp_buf;
}

static int crosec_spi_io(size_t req_size, size_t resp_size, void *context)
{
	struct spi_slave *slave = (struct spi_slave *)context;
	int ret = 0;

	while (!stopwatch_expired(&cs_cooldown_sw))
		/* Wait minimum delay between CS assertions. */;
	spi_claim_bus(slave);

	 /* Allow EC to ramp up clock after being awaken.
	  * See chrome-os-partner:32223 for more details. */
	udelay(CONFIG_EC_GOOGLE_CHROMEEC_SPI_WAKEUP_DELAY_US);

	if (spi_xfer(slave, req_buf, req_size, NULL, 0)) {
		printk(BIOS_ERR, "%s: Failed to send request.\n", __func__);
		ret = -1;
		goto out;
	}

	uint8_t byte;
	struct stopwatch sw;
	// Wait 1s for a framing byte.
	stopwatch_init_usecs_expire(&sw, USECS_PER_SEC);
	while (1) {
		if (spi_xfer(slave, NULL, 0, &byte, sizeof(byte))) {
			printk(BIOS_ERR, "%s: Failed to receive byte.\n",
			       __func__);
			ret = -1;
			goto out;
		}
		if (byte == EcFramingByte)
			break;

		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "%s: Timeout waiting for framing byte.\n",
			       __func__);
			ret = -1;
			goto out;
		}
	}

	if (spi_xfer(slave, NULL, 0, resp_buf, resp_size)) {
		printk(BIOS_ERR, "%s: Failed to receive response.\n", __func__);
		ret = -1;
	}

out:
	spi_release_bus(slave);
	stopwatch_init_usecs_expire(&cs_cooldown_sw, cs_cooldown_us);
	return ret;
}

int google_chromeec_command(struct chromeec_command *cec_command)
{
	static struct spi_slave *slave = NULL;
	if (!slave) {
		slave = spi_setup_slave(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS,
					CONFIG_EC_GOOGLE_CHROMEEC_SPI_CHIP);
		stopwatch_init(&cs_cooldown_sw);
	}
	return crosec_command_proto(cec_command, crosec_spi_io, slave);
}

#ifndef __PRE_RAM__
u8 google_chromeec_get_event(void)
{
	printk(BIOS_ERR, "%s: Not supported.\n", __func__);
	return 0;
}
#endif
