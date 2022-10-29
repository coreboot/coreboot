/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Description:
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * It is based on the Linux kernel driver tpm.c from Leendert van
 * Dorn, Dave Safford, Reiner Sailer, and Kyleen Hall.
 */

#ifndef __DRIVERS_TPM_SLB9635_I2C_TPM_H__
#define __DRIVERS_TPM_SLB9635_I2C_TPM_H__

#include <stdint.h>

enum tpm_timeout {
	TPM_TIMEOUT = 1,	/* msecs */
};

/* Size of external transmit buffer (used for stack buffer in tpm_sendrecv) */
#define TPM_BUFSIZE 1260

/* Number of bytes in the TPM header */
#define TPM_HEADER_SIZE 10

/* Index of fields in TPM command buffer */
#define TPM_CMD_SIZE_BYTE 2
#define TPM_CMD_ORDINAL_BYTE 6

/* Index of Count field in TPM response buffer */
#define TPM_RSP_SIZE_BYTE 2
#define TPM_RSP_RC_BYTE 6

#define	TPM_ACCESS(l)			(0x0000 | ((l) << 4))
#define	TPM_STS(l)			(0x0001 | ((l) << 4))
#define	TPM_DATA_FIFO(l)		(0x0005 | ((l) << 4))
#define	TPM_DID_VID(l)			(0x0006 | ((l) << 4))

struct tpm_chip;

struct tpm_vendor_specific {
	uint8_t req_complete_mask;
	uint8_t req_complete_val;
	uint8_t req_canceled;
	int (*recv)(struct tpm_chip *, uint8_t *, size_t);
	int (*send)(struct tpm_chip *, uint8_t *, size_t);
	void (*cancel)(struct tpm_chip *);
	uint8_t (*status)(struct tpm_chip *);
	int locality;
};

struct tpm_chip {
	int is_open;
	struct tpm_vendor_specific vendor;
};

/* ---------- Interface for TPM vendor ------------ */

int tpm_vendor_probe(unsigned int bus, uint32_t addr);

int tpm_vendor_init(struct tpm_chip *chip, unsigned int bus, uint32_t dev_addr);

#endif /* __DRIVERS_TPM_SLB9635_I2C_TPM_H__ */
