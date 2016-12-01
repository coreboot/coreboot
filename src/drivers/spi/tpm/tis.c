/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/early_variables.h>
#include <console/console.h>
#include <string.h>
#include <tpm.h>

#include "tpm.h"

static unsigned tpm_is_open CAR_GLOBAL;

static const struct {
	uint16_t vid;
	uint16_t did;
	const char *device_name;
} dev_map[] = {
	{ 0x15d1, 0x001b, "SLB9670" },
	{ 0x1ae0, 0x0028, "CR50" },
};

static const char *tis_get_dev_name(struct tpm2_info *info)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(dev_map); i++)
		if ((dev_map[i].vid == info->vendor_id) &&
		    (dev_map[i].did == info->device_id))
			return dev_map[i].device_name;
	return "Unknown";
}

int tis_open(void)
{
	if (car_get_var(tpm_is_open)) {
		printk(BIOS_ERR, "tis_open() called twice.\n");
		return -1;
	}
	return 0;
}

int tis_close(void)
{
	if (car_get_var(tpm_is_open)) {

		/*
		 * Do we need to do something here, like waiting for a
		 * transaction to stop?
		 */
		car_set_var(tpm_is_open, 0);
	}

	return 0;
}

int tis_init(void)
{
	struct spi_slave spi;
	struct tpm2_info info;

	if (spi_setup_slave(CONFIG_DRIVER_TPM_SPI_BUS,
			    CONFIG_DRIVER_TPM_SPI_CHIP, &spi)) {
		printk(BIOS_ERR, "Failed to setup TPM SPI slave\n");
		return -1;
	}

	if (tpm2_init(&spi)) {
		printk(BIOS_ERR, "Failed to initialize TPM SPI interface\n");
		return -1;
	}

	tpm2_get_info(&info);

	printk(BIOS_INFO, "Initialized TPM device %s revision %d\n",
	       tis_get_dev_name(&info), info.revision);

	return 0;
}


int tis_sendrecv(const uint8_t *sendbuf, size_t sbuf_size,
		 uint8_t *recvbuf, size_t *rbuf_len)
{
	int len = tpm2_process_command(sendbuf, sbuf_size, recvbuf, *rbuf_len);
	*rbuf_len = len;
	return 0;
}
