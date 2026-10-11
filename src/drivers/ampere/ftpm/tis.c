/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <security/tpm/tis.h>

#include "tpm.h"

static tpm_result_t crb_tpm_sendrecv(const uint8_t *sendbuf, size_t sbuf_size,
				     uint8_t *recvbuf, size_t *rbuf_len)
{
	int len = crb_tpm_process_command(sendbuf, sbuf_size, recvbuf, *rbuf_len);

	if (len <= 0)
		return TPM_CB_FAIL;

	*rbuf_len = len;

	return TPM_SUCCESS;
}

tis_sendrecv_fn crb_tis_probe(enum tpm_family *family)
{
	if (crb_tpm_init())
		return NULL;

	/* The CRB interface only exists in TPM 2.0. */
	if (family)
		*family = TPM_2;

	printk(BIOS_INFO, "Initialized Ampere fTPM\n");

	return &crb_tpm_sendrecv;
}
