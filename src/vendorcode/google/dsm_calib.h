/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DSM_CALIB_H__
#define __DSM_CALIB_H__

#include <stddef.h>
#include <stdint.h>
#include <types.h>

/**
 * get_dsm_calibration_from_key - Gets value related to DSM calibration from VPD
 * @key: The key in RO_VPD. The valid prefix is "dsm_calib_". The valid keys are
 *   documented in https://chromeos.google.com/partner/dlm/docs/factory/vpd.html.
 * @value: Output value. The value read from VPD parsed into uint64_t integer.
 *
 * Returns CB_SUCCESS on success or CB_ERR on failure.
 */
enum cb_err get_dsm_calibration_from_key(const char *key, uint64_t *value);

#endif /* __DSM_CALIB_H__ */
