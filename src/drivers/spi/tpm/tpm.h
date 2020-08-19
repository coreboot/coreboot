/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __COREBOOT_SRC_DRIVERS_SPI_TPM_TPM_H
#define __COREBOOT_SRC_DRIVERS_SPI_TPM_TPM_H

#include <stddef.h>
#include <spi-generic.h>

/*
 * A tpm device descriptor, values read from the appropriate device regisrers
 * are cached here.
 */
struct tpm2_info {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t revision;
};

/* Structure describing the elements of Cr50 firmware version. */
struct cr50_firmware_version {
	int epoch;
	int major;
	int minor;
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

/* Indicates whether Cr50 ready pulses are guaranteed to be at least 100us. */
bool cr50_is_long_interrupt_pulse_enabled(void);

/* Get the cr50 firmware version information. */
void cr50_get_firmware_version(struct cr50_firmware_version *version);

#endif  /* ! __COREBOOT_SRC_DRIVERS_SPI_TPM_TPM_H */
