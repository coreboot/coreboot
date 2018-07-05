/* Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* TPM error codes.
 *
 * Copy-pasted and lightly edited from TCG TPM Main Part 2 TPM Structures
 * Version 1.2 Level 2 Revision 103 26 October 2006 Draft.
 */

#ifndef TSS_ERRORS_H_
#define TSS_ERRORS_H_

#define TPM_E_BASE 0x0
#define TPM_E_NON_FATAL 0x800

#define TPM_E_AREA_LOCKED           ((uint32_t)0x0000003c)
#define TPM_E_BADINDEX              ((uint32_t)0x00000002)
#define TPM_E_BAD_PRESENCE          ((uint32_t)0x0000002d)
#define TPM_E_IOERROR               ((uint32_t)0x0000001f)
#define TPM_E_INVALID_POSTINIT      ((uint32_t)0x00000026)
#define TPM_E_MAXNVWRITES           ((uint32_t)0x00000048)
#define TPM_E_OWNER_SET             ((uint32_t)0x00000014)

#define TPM_E_NEEDS_SELFTEST ((uint32_t)(TPM_E_NON_FATAL + 1))
#define TPM_E_DOING_SELFTEST ((uint32_t)(TPM_E_NON_FATAL + 2))

#define TPM_E_ALREADY_INITIALIZED    ((uint32_t)0x00005000)  /* vboot local */
#define TPM_E_INTERNAL_INCONSISTENCY ((uint32_t)0x00005001)  /* vboot local */
#define TPM_E_MUST_REBOOT            ((uint32_t)0x00005002)  /* vboot local */
#define TPM_E_CORRUPTED_STATE        ((uint32_t)0x00005003)  /* vboot local */
#define TPM_E_COMMUNICATION_ERROR    ((uint32_t)0x00005004)  /* vboot local */
#define TPM_E_RESPONSE_TOO_LARGE     ((uint32_t)0x00005005)  /* vboot local */
#define TPM_E_NO_DEVICE              ((uint32_t)0x00005006)  /* vboot local */
#define TPM_E_INPUT_TOO_SMALL        ((uint32_t)0x00005007)  /* vboot local */
#define TPM_E_WRITE_FAILURE          ((uint32_t)0x00005008)  /* vboot local */
#define TPM_E_READ_EMPTY             ((uint32_t)0x00005009)  /* vboot local */
#define TPM_E_READ_FAILURE           ((uint32_t)0x0000500a)  /* vboot local */
#define TPM_E_NV_DEFINED             ((uint32_t)0x0000500b)  /* vboot local */

#endif /* TSS_ERRORS_H_ */
