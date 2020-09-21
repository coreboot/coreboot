/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _TPM_PPI_H_
#define _TPM_PPI_H_

#include <device/device.h>

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

#endif /* _TPM_PPI_H_ */
