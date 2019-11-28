/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2018-2019 Qualcomm Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SPI_QUP_QCOM_HEADER___
#define __SPI_QUP_QCOM_HEADER___

#include <spi-generic.h>

int qup_spi_claim_bus(const struct spi_slave *slave);
int qup_spi_xfer(const struct spi_slave *slave, const void *dout,
	size_t bytes_out, void *din, size_t bytes_in);
void qup_spi_release_bus(const struct spi_slave *slave);
void qup_spi_init(unsigned int bus, unsigned int speed_hz);

#endif /*__SPI_QUP_QCOM_HEADER___*/
