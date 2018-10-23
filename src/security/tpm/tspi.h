/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Copyright 2018 Facebook Inc.
 * Copyright 2018 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef TSPI_H_
#define TSPI_H_

#include <security/tpm/tss.h>
#include <commonlib/tcpa_log_serialized.h>
#include <commonlib/region.h>

#define TPM_PCR_MAX_LEN		64
#define HASH_DATA_CHUNK_SIZE	1024

/**
 * Add table entry for cbmem TCPA log.
 */
void tcpa_log_add_table_entry(const char *name, const uint32_t pcr,
			      const uint8_t *digest,
			      const size_t digest_length);

/**
 * Ask vboot for a digest and extend a TPM PCR with it.
 * @param pcr sets the pcr index
 * @param digest sets the hash to extend into the tpm
 * @param digest_len the length of the digest
 * @param name sets additional info where the digest comes from
 * @return TPM_SUCCESS on success. If not a tpm error is returned
 */
uint32_t tpm_extend_pcr(int pcr, uint8_t *digest, size_t digest_len,
			const char *name);

/**
 * Issue a TPM_Clear and reenable/reactivate the TPM.
 * @return TPM_SUCCESS on success. If not a tpm error is returned
 */
uint32_t tpm_clear_and_reenable(void);

/**
 * Start the TPM and establish the root of trust.
 * @param s3flag tells the tpm setup if we wake up from a s3 state on x86
 * @return TPM_SUCCESS on success. If not a tpm error is returned
 */
uint32_t tpm_setup(int s3flag);

/**
 * Measure a given region device and extend given PCR with the result.
 * @param *rdev Pointer to the region device to measure
 * @param pcr Index of the PCR which will be extended by this measure
 * @param *rname Name of the region that is measured
 * @return TPM error code in case of error otherwise TPM_SUCCESS
 */
uint32_t tpm_measure_region(const struct region_device *rdev, uint8_t pcr,
			    const char *rname);

#endif /* TSPI_H_ */
