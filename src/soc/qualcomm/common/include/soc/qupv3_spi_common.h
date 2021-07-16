/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SPI_QUP_QCOM_HEADER___
#define __SPI_QUP_QCOM_HEADER___

#include <spi-generic.h>

int qup_spi_claim_bus(const struct spi_slave *slave);
int qup_spi_xfer(const struct spi_slave *slave, const void *dout,
	size_t bytes_out, void *din, size_t bytes_in);
void qup_spi_release_bus(const struct spi_slave *slave);
void qup_spi_init(unsigned int bus, unsigned int speed_hz);

#endif /*__SPI_QUP_QCOM_HEADER___*/
