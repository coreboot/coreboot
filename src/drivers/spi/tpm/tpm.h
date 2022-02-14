/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __COREBOOT_SRC_DRIVERS_SPI_TPM_TPM_H
#define __COREBOOT_SRC_DRIVERS_SPI_TPM_TPM_H

#include <drivers/tpm/cr50.h>
#include <stddef.h>
#include <spi-generic.h>

#define TPM_LOCALITY_0_SPI_BASE 0x00d40000

/*
 * A tpm device descriptor, values read from the appropriate device regisrers
 * are cached here.
 */
struct tpm2_info {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t revision;
};

/*
 * Initialize a TPM2 device: read its id, claim locality of zero, verify that
 * this indeed is a TPM2 device. Use the passed in handle to access the right
 * SPI port.
 *
 * Return 0 on success, non-zero on failure.
 */
int tpm2_init(struct spi_slave *spi_if);

/*
 * Each command processing consists of sending the command to the TPM, by
 * writing it into the FIFO register, then polling the status register until
 * the TPM is ready to respond, then reading the response from the FIFO
 * regitster. The size of the response can be gleaned from the 6 byte header.
 *
 * This function places the response into the tpm2_response buffer and returns
 * the size of the response.
 */
size_t tpm2_process_command(const void *tpm2_command, size_t command_size,
			    void *tpm2_response, size_t max_response);

/* Get information about previously initialized TPM device. */
void tpm2_get_info(struct tpm2_info *info);

#endif  /* ! __COREBOOT_SRC_DRIVERS_SPI_TPM_TPM_H */
