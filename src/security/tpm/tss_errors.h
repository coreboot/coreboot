/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * TPM error codes.
 *
 * Copy-pasted and lightly edited from TCG TPM Main Part 2 TPM Structures
 * Version 1.2 Level 2 Revision 116 1 March 2011.
 */

#ifndef TSS_ERRORS_H_
#define TSS_ERRORS_H_

#include <stdint.h>

typedef uint32_t tpm_result_t;
#define TPM_Vendor_Specific32 0x400

#define TPM_BASE 0x0

#define TPM_NON_FATAL (0x800 + TPM_BASE)
#define TPM_CB_ERROR TPM_Vendor_Specific32

#define TPM_SUCCESS               ((tpm_result_t) (TPM_BASE + 0x00))
#define TPM_BADINDEX              ((tpm_result_t) (TPM_BASE + 0x02))
#define TPM_BAD_PARAMETER         ((tpm_result_t) (TPM_BASE + 0x03))
#define TPM_FAIL                  ((tpm_result_t) (TPM_BASE + 0x09))
#define TPM_OWNER_SET             ((tpm_result_t) (TPM_BASE + 0x14))
#define TPM_IOERROR               ((tpm_result_t) (TPM_BASE + 0x1F))
#define TPM_INVALID_POSTINIT      ((tpm_result_t) (TPM_BASE + 0x26))
#define TPM_BAD_PRESENCE          ((tpm_result_t) (TPM_BASE + 0x2D))
#define TPM_AREA_LOCKED           ((tpm_result_t) (TPM_BASE + 0x3C))
#define TPM_MAXNVWRITES           ((tpm_result_t) (TPM_BASE + 0x48))

#define TPM_RETRY          ((tpm_result_t) (TPM_NON_FATAL + 0x00))
#define TPM_NEEDS_SELFTEST ((tpm_result_t) (TPM_NON_FATAL + 0x01))
#define TPM_DOING_SELFTEST ((tpm_result_t) (TPM_NON_FATAL + 0x02))

/* The following values are defind at the offset 0x480 which is a combination
 *    of the 32-bit vendor specific value from the TCG standard(0x400) and an
 *    offset of 0x80 to assist in identifying these return values when the 8-bit
 *    truncated value is used.
 *
 * Valid offset range is 128-255(0x80-0xFF)
*/

#define TPM_CB_ALREADY_INITIALIZED    ((tpm_result_t) (TPM_CB_ERROR + 0x80))
#define TPM_CB_INTERNAL_INCONSISTENCY ((tpm_result_t) (TPM_CB_ERROR + 0x81))
#define TPM_CB_MUST_REBOOT            ((tpm_result_t) (TPM_CB_ERROR + 0x82))
#define TPM_CB_CORRUPTED_STATE        ((tpm_result_t) (TPM_CB_ERROR + 0x83))
#define TPM_CB_COMMUNICATION_ERROR    ((tpm_result_t) (TPM_CB_ERROR + 0x84))
#define TPM_CB_RESPONSE_TOO_LARGE     ((tpm_result_t) (TPM_CB_ERROR + 0x85))
#define TPM_CB_NO_DEVICE              ((tpm_result_t) (TPM_CB_ERROR + 0x86))
#define TPM_CB_INPUT_TOO_SMALL        ((tpm_result_t) (TPM_CB_ERROR + 0x87))
#define TPM_CB_WRITE_FAILURE          ((tpm_result_t) (TPM_CB_ERROR + 0x88))
#define TPM_CB_READ_EMPTY             ((tpm_result_t) (TPM_CB_ERROR + 0x89))
#define TPM_CB_READ_FAILURE           ((tpm_result_t) (TPM_CB_ERROR + 0x8A))
#define TPM_CB_NV_DEFINED             ((tpm_result_t) (TPM_CB_ERROR + 0x8B))
#define TPM_CB_INVALID_ARG            ((tpm_result_t) (TPM_CB_ERROR + 0x8C))
#define TPM_CB_HASH_ERROR             ((tpm_result_t) (TPM_CB_ERROR + 0x8D))
#define TPM_CB_NO_SUCH_COMMAND        ((tpm_result_t) (TPM_CB_ERROR + 0x8E))
#define TPM_CB_RANGE                  ((tpm_result_t) (TPM_CB_ERROR + 0x8F))
#define TPM_CB_FAIL                   ((tpm_result_t) (TPM_CB_ERROR + 0x90))
#define TPM_CB_TIMEOUT                ((tpm_result_t) (TPM_CB_ERROR + 0x91))
#define TPM_CB_PROBE_FAILURE          ((tpm_result_t) (TPM_CB_ERROR + 0x92))

#endif /* TSS_ERRORS_H_ */
