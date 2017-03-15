/*
 * Copyright (C) 2011 Infineon Technologies
 *
 * Authors:
 * Peter Huewe <huewe.external@infineon.com>
 *
 * Version: 2.1.1
 *
 * Description:
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * It is based on the Linux kernel driver tpm.c from Leendert van
 * Dorn, Dave Safford, Reiner Sailer, and Kyleen Hall.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

enum tis_access {
	TPM_ACCESS_VALID = 0x80,
	TPM_ACCESS_ACTIVE_LOCALITY = 0x20,
	TPM_ACCESS_REQUEST_PENDING = 0x04,
	TPM_ACCESS_REQUEST_USE = 0x02,
};

enum tis_status {
	TPM_STS_VALID = 0x80,
	TPM_STS_COMMAND_READY = 0x40,
	TPM_STS_GO = 0x20,
	TPM_STS_DATA_AVAIL = 0x10,
	TPM_STS_DATA_EXPECT = 0x08,
};

#define	TPM_ACCESS(l)			(0x0000 | ((l) << 4))
#define	TPM_STS(l)			(0x0001 | ((l) << 4))
#define	TPM_DATA_FIFO(l)		(0x0005 | ((l) << 4))
#define	TPM_DID_VID(l)			(0x0006 | ((l) << 4))

struct tpm_chip;

struct tpm_vendor_specific {
	uint8_t req_complete_mask;
	uint8_t req_complete_val;
	uint8_t req_canceled;
	int irq;
	int (*irq_status)(int irq);
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

struct tpm_input_header {
	uint16_t tag;
	uint32_t length;
	uint32_t ordinal;
} __attribute__ ((packed));

struct tpm_output_header {
	uint16_t tag;
	uint32_t length;
	uint32_t return_code;
} __attribute__ ((packed));

struct timeout_t {
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
} __attribute__ ((packed));

struct duration_t {
	uint32_t tpm_short;
	uint32_t tpm_medium;
	uint32_t tpm_long;
} __attribute__ ((packed));

typedef union {
	struct timeout_t timeout;
	struct duration_t duration;
} cap_t;

struct tpm_getcap_params_in {
	uint32_t cap;
	uint32_t subcap_size;
	uint32_t subcap;
} __attribute__ ((packed));

struct tpm_getcap_params_out {
	uint32_t cap_size;
	cap_t cap;
} __attribute__ ((packed));

typedef union {
	struct tpm_input_header in;
	struct tpm_output_header out;
} tpm_cmd_header;

typedef union {
	struct tpm_getcap_params_out getcap_out;
	struct tpm_getcap_params_in getcap_in;
} tpm_cmd_params;

struct tpm_cmd_t {
	tpm_cmd_header header;
	tpm_cmd_params params;
} __attribute__ ((packed));

/* ---------- Interface for TPM vendor ------------ */

int tpm_vendor_probe(unsigned int bus, uint32_t addr);

int tpm_vendor_init(struct tpm_chip *chip, unsigned int bus, uint32_t dev_addr);

void tpm_vendor_cleanup(struct tpm_chip *chip);

#endif /* __DRIVERS_TPM_SLB9635_I2C_TPM_H__ */
