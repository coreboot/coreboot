/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/ipchksum.h>
#include <console/console.h>
#include <pc80/mc146818rtc.h>

#include "cse_lite_cmos.h"

/*
 * We need a region in CMOS to store the firmware versions.
 *
 * This can either be declared as part of the option
 * table or statically defined in the board config.
 */
#if CONFIG(USE_OPTION_TABLE)
# include "option_table.h"

#ifndef CMOS_VSTART_partition_fw
#error "The `CSE partition firmware` CMOS entry is missing, please add it to your cmos.layout."
#endif

#if CMOS_VSTART_partition_fw % 8 != 0
#error "The `CSE partition firmware` CMOS entry needs to be byte aligned, check your cmos.layout."
#endif	// CMOS_VSTART_partition_fw % 8 != 0

#if CMOS_VLEN_partition_fw != (32 * 8)
#error "The partition firmware entry needs to be 32 bytes long, check your cmos.layout."
#endif

# define PARTITION_FW_CMOS_OFFSET (CMOS_VSTART_partition_fw >> 3)

#else
# if (CONFIG_SOC_INTEL_CSE_FW_PARTITION_CMOS_OFFSET != 0)
#  define PARTITION_FW_CMOS_OFFSET CONFIG_SOC_INTEL_CSE_FW_PARTITION_CMOS_OFFSET
# else
#  error "Must configure CONFIG_SOC_INTEL_CSE_FW_PARTITION_CMOS_OFFSET"
# endif
#endif

#define PSR_BACKUP_STATUS_SIGNATURE 0x42525350	/* 'PSRB' */

/* Helper function to read CSE fpt information from cmos memory. */
void cmos_read_fw_partition_info(struct cse_specific_info *info)
{
	for (uint8_t *p = (uint8_t *)info, i = 0; i < sizeof(*info); i++, p++)
		*p = cmos_read(PARTITION_FW_CMOS_OFFSET + i);
}

/* Helper function to write CSE fpt information to cmos memory. */
void cmos_write_fw_partition_info(const struct cse_specific_info *info)
{
	for (uint8_t *p = (uint8_t *)info, i = 0; i < sizeof(*info); i++, p++)
		cmos_write(*p, PARTITION_FW_CMOS_OFFSET + i);
}

/* Read and validate `psr_backup_status` structure from CMOS */
static int psr_backup_status_cmos_read(struct psr_backup_status *psr)
{
	for (uint8_t *p = (uint8_t *)psr, i = 0; i < sizeof(*psr); i++, p++)
		*p = cmos_read(PARTITION_FW_CMOS_OFFSET + sizeof(struct cse_specific_info) + i);

	/* Verify signature */
	if (psr->signature != PSR_BACKUP_STATUS_SIGNATURE) {
		printk(BIOS_ERR, "PSR backup status invalid signature\n");
		return -1;
	}

	/* Verify checksum over signature and backup_status only */
	uint16_t csum = ipchksum(psr, offsetof(struct psr_backup_status, checksum));

	if (csum != psr->checksum) {
		printk(BIOS_ERR, "PSR backup status checksum mismatch\n");
		return -1;
	}

	return 0;
}

/* Write `psr_backup_status structure` to CMOS */
static void psr_backup_status_cmos_write(struct psr_backup_status *psr)
{
	/* Checksum over signature and backup_status only */
	psr->checksum = ipchksum(psr, offsetof(struct psr_backup_status, checksum));

	for (uint8_t *p = (uint8_t *)psr, i = 0; i < sizeof(*psr); i++, p++)
		cmos_write(*p, PARTITION_FW_CMOS_OFFSET + sizeof(struct cse_specific_info) + i);
}

/* Helper function to update the `psr_backup_status` in CMOS memory */
void update_psr_backup_status(int8_t status)
{
	struct psr_backup_status psr;

	/* Read and update psr_backup_status */
	if (psr_backup_status_cmos_read(&psr) < 0)
		/* Structure invalid, re-initialize */
		psr.signature = PSR_BACKUP_STATUS_SIGNATURE;

	psr.value = status;

	/* Write the new status to CMOS */
	psr_backup_status_cmos_write(&psr);

	printk(BIOS_INFO, "PSR backup status updated\n");
}

/*
 * Helper function to retrieve the current `psr_backup_status` in CMOS memory
 * Returns current status on success, the status can be PSR_BACKUP_DONE or PSR_BACKUP_PENDING.
 * Returns -1 in case of signature mismatch or checksum failure.
 */
int8_t get_psr_backup_status(void)
{
	struct psr_backup_status psr;

	if (psr_backup_status_cmos_read(&psr) < 0)
		return -1;

	return psr.value;
}
