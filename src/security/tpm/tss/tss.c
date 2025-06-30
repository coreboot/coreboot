/* SPDX-License-Identifier: BSD-3-Clause */

#include <console/console.h>
#include <drivers/crb/tpm.h>
#include <drivers/i2c/tpm/tpm.h>
#include <drivers/pc80/tpm/tpm.h>
#include <drivers/spi/tpm/tpm.h>
#include <security/tpm/tis.h>
#include <security/tpm/tss.h>

/*
 * This unit is meant to dispatch to either TPM1.2 or TPM2.0 TSS implementation
 * based on TPM family determined on probing during initialization.
 */

enum tpm_family tlcl_tpm_family = TPM_UNKNOWN;

tis_sendrecv_fn tlcl_tis_sendrecv;

/* Probe for TPM device and choose implementation based on the returned TPM family. */
tpm_result_t tlcl_lib_init(void)
{
	/* Don't probe for TPM more than once per stage. */
	static bool init_done;
	if (init_done)
		return tlcl_tpm_family == TPM_UNKNOWN ? TPM_CB_NO_DEVICE : TPM_SUCCESS;

	/* Set right away to make recursion impossible. */
	init_done = true;

	tlcl_tis_sendrecv = NULL;
	if (CONFIG(CRB_TPM) || CONFIG(AMD_CRB_FTPM))
		tlcl_tis_sendrecv = crb_tis_probe(&tlcl_tpm_family);
	if (CONFIG(MEMORY_MAPPED_TPM) && tlcl_tis_sendrecv == NULL)
		tlcl_tis_sendrecv = pc80_tis_probe(&tlcl_tpm_family);
	if (CONFIG(I2C_TPM) && tlcl_tis_sendrecv == NULL)
		tlcl_tis_sendrecv = i2c_tis_probe(&tlcl_tpm_family);
	if (CONFIG(SPI_TPM) && tlcl_tis_sendrecv == NULL)
		tlcl_tis_sendrecv = spi_tis_probe(&tlcl_tpm_family);

	if (tlcl_tis_sendrecv == NULL) {
		printk(BIOS_ERR, "%s: TIS probe failed\n", __func__);
		tlcl_tpm_family = TPM_UNKNOWN;
	} else if (tlcl_tpm_family != TPM_1 && tlcl_tpm_family != TPM_2) {
		printk(BIOS_ERR, "%s: TIS probe returned incorrect TPM family: %d\n", __func__,
		       tlcl_tpm_family);
		tlcl_tpm_family = TPM_UNKNOWN;
	}

	return tlcl_tpm_family == TPM_UNKNOWN ? TPM_CB_NO_DEVICE : TPM_SUCCESS;
}
