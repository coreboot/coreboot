/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_PC80_TPM_TPM_H
#define DRIVERS_PC80_TPM_TPM_H

#include <security/tpm/tis.h>

tis_sendrecv_fn pc80_tis_probe(enum tpm_family *family);

#endif /* DRIVERS_PC80_TPM_TPM_H */
