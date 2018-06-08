/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Copyright 2018 Facebook Inc.
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
#ifndef CR50_TSS_STRUCTURES_H_
#define CR50_TSS_STRUCTURES_H_

#include <stdint.h>

/* FIXME: below is not enough to differentiate between vendors commands
   of numerous devices. However, the current tpm2 APIs aren't very amenable
   to extending generically because the marshaling code is assuming all
   knowledge of all commands. */
#define TPM2_CR50_VENDOR_COMMAND ((TPM_CC)(TPM_CC_VENDOR_BIT_MASK | 0))
#define TPM2_CR50_SUB_CMD_NVMEM_ENABLE_COMMITS (21)
#define TPM2_CR50_SUB_CMD_TURN_UPDATE_ON (24)

/**
 * CR50 specific tpm command to enable nvmem commits before internal timeout
 * expires.
 */
uint32_t tlcl_cr50_enable_nvcommits(void);

/**
 * CR50 specific tpm command to restore header(s) of the dormant RO/RW
 * image(s) and in case there indeed was a dormant image, trigger reboot after
 * the timeout milliseconds. Note that timeout of zero means "NO REBOOT", not
 * "IMMEDIATE REBOOT".
 *
 * Return value indicates success or failure of accessing the TPM; in case of
 * success the number of restored headers is saved in num_restored_headers.
 */
uint32_t tlcl_cr50_enable_update(uint16_t timeout_ms,
				 uint8_t *num_restored_headers);

#endif /* CR50_TSS_STRUCTURES_H_ */
