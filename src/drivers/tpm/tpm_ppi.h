/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _TPM_PPI_H_
#define _TPM_PPI_H_

#include <device/device.h>
#include <boot/coreboot_tables.h>

#if CONFIG(HAVE_ACPI_TABLES)
void tpm_ppi_acpi_fill_ssdt(const struct device *dev);
#else
static inline void tpm_ppi_acpi_fill_ssdt(const struct device *dev)
{
}
#endif

/* Return codes */
/* Function 2 */
#define PPI2_RET_SUCCESS 0
#define PPI2_RET_NOT_SUPPORTED 1
#define PPI2_RET_GENERAL_FAILURE 2

/* Function 3 */
#define PPI3_RET_SUCCESS 0
#define PPI3_RET_GENERAL_FAILURE 1

/* Function 4 */
#define PPI4_RET_NONE 0
#define PPI4_RET_SHUTDOWN 1
#define PPI4_RET_REBOOT 2
#define PPI4_RET_OS_VENDOR_SPECIFIC 3

/* Function 5 */
#define PPI5_RET_SUCCESS 0
#define PPI5_RET_GENERAL_FAILURE 1

/* Function 6 */
#define PPI6_RET_NOT_IMPLEMENTED 3

/* Function 7 */
#define PPI7_RET_SUCCESS 0
#define PPI7_RET_NOT_IMPLEMENTED 1
#define PPI7_RET_GENERAL_FAILURE 2
#define PPI7_RET_BLOCKED_BY_FIRMWARE 3

/* Function 8 */
#define PPI8_RET_NOT_IMPLEMENTED 0
#define PPI8_RET_FIRMWARE_ONLY 1
#define PPI8_RET_BLOCKED_FOR_OS_BY_FW 2
#define PPI8_RET_ALLOWED_WITH_PP 3
#define PPI8_RET_ALLOWED 4

/* TCG Physical Presence Interface */
#define TPM_PPI_UUID   "3dddfaa6-361b-4eb4-a424-8d10089d1653"
/* TCG Memory Clear Interface */
#define TPM_MCI_UUID   "376054ed-cc13-4675-901c-4756d7f2d45d"

/*
 * Physical Presence Interface Specification Version 1.30 Revision 00.52
 * Table 1 Physical Presence Interface Operation Summary for TPM 1.2
 */
#define TPM_NOOP 0
#define TPM_ENABLE 1
#define TPM_DISABLE 2
#define TPM_ACTIVATE 3
#define TPM_DEACTIVATE 4
#define TPM_CLEAR 5
#define TPM_ENABLE_ACTIVATE 6
#define TPM_DEACTIVATE_DISABLE 7
#define TPM_SETOWNERINSTALL_TRUE 8
#define TPM_SETOWNERINSTALL_FALSE 9
#define TPM_ENABLE_ACTIVATE_SETOWNERINSTALL_TRUE 10
#define TPM_SETOWNERINSTALL_FALSE_DEACTIVATE_DISABLE 11
#define TPM_CLEAR_ENABLE_ACTIVATE 14
#define TPM_SET_NOPPIPROVISION_FALSE 15
#define TPM_SET_NOPPIPROVISION_TRUE 16
#define TPM_SET_NOPPICLEAR_FALSE 17
#define TPM_SET_NOPPICLEAR_TRUE 18
#define TPM_SET_NOPPIMAINTAINANCE_FALSE 19
#define TPM_SET_NOPPIMAINTAINANCE_TRUE 20
#define TPM_ENABLE_ACTIVE_CLEAR 21
#define TPM_ENABLE_ACTIVE_CLEAR_ENABLE_ACTIVE 22

/*
 * Physical Presence Interface Specification Version 1.30 Revision 00.52
 * Table 2 Physical Presence Interface Operation Summary for TPM 2.0
 */
#define TPM2_NOOP 0
#define TPM2_ENABLE 1
#define TPM2_DISABLE 2
#define TPM2_CLEAR 5
#define TPM2_CLEAR_ENABLE_ACTIVE 14
#define TPM2_SET_PP_REQUIRED_FOR_CLEAR_TRUE 17
#define TPM2_SET_PP_REQUIRED_FOR_CLEAR_FALSE 18
#define TPM2_ENABLE_CLEAR 21
#define TPM2_ENABLE_CLEAR2 22
#define TPM2_SET_PCR_BANKS 23
#define TPM2_CHANGE_EPS 24
#define TPM2_SET_PP_REQUIRED_FOR_CHANGE_PCRS_FALSE 25
#define TPM2_SET_PP_REQUIRED_FOR_CHANGE_PCRS_TRUE 26
#define TPM2_SET_PP_REQUIRED_FOR_TURN_ON_FALSE 27
#define TPM2_SET_PP_REQUIRED_FOR_TURN_ON_TRUE 28
#define TPM2_SET_PP_REQUIRED_FOR_TURN_OFF_FALSE 29
#define TPM2_SET_PP_REQUIRED_FOR_TURN_OFF_TRUE 30
#define TPM2_SET_PP_REQUIRED_FOR_CHANGE_EPS_FALSE 31
#define TPM2_SET_PP_REQUIRED_FOR_CHANGE_EPS_TRUE 32
#define TPM2_LOG_ALL_DIGEST 33
#define TPM2_DISABLE_ENDORSMENT_ENABLE_STORAGE_HISTORY 34
#define TPM2_ENABLE_BLOCK_SID 96
#define TPM2_DISABLE_BLOCK_SID 97
#define TPM2_SET_PP_REQUIRED_FOR_ENABLE_BLOCK_SID_TRUE 98
#define TPM2_SET_PP_REQUIRED_FOR_ENABLE_BLOCK_SID_FALSE 99
#define TPM2_SET_PP_REQUIRED_FOR_DISABLE_BLOCK_SID_TRUE 100
#define TPM2_SET_PP_REQUIRED_FOR_DISABLE_BLOCK_SID_FALSE 101

#define VENDOR_SPECIFIC_OFFSET 0x80

 /*
  * The layout of the buffer matches the QEMU virtual memory device that is generated
  * by QEMU. See files 'hw/i386/acpi-build.c' and 'include/hw/acpi/tpm.h' for details.
  */
struct cb_tpm_ppi_payload_handshake {
	uint8_t func[256];	/* Firmware sets values for each supported operation.
				 * See defined values below. */
	uint8_t ppin;		/* SMI interrupt to use. Set by firmware.
				 * Not supported. */
	uint32_t ppip;		/* ACPI function index to pass to SMM code.
				 * Set by ACPI. Not supported. */
	uint32_t pprp;		/* Result of last executed operation.
				 * Set by firmware. See function index 5 for values. */
	uint32_t pprq;		/* Operation request number to execute.
				 * See 'Physical Presence Interface Operation Summary'
				 * tables in specs. Set by ACPI. */
	uint32_t pprm;		/* Operation request optional parameter.
				 * Values depend on operation. Set by ACPI. */
	uint32_t lppr;		/* Last executed operation request number.
				 * Copied from pprq field by firmware. */
	uint32_t fret;		/* Result code from SMM function. Not supported. */
	uint8_t res1[0x040];	/* Reserved */
	uint8_t next_step;	/* Operation to execute after reboot by firmware.
				 * Used by firmware. */
} __packed;

void lb_tpm_ppi(struct lb_header *header);

#endif /* _TPM_PPI_H_ */
