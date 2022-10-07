/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <string.h>

void *backup_default_smm_area(void)
{
	void *save_area;
	const void *default_smm = (void *)SMM_DEFAULT_BASE;

	if (!CONFIG(HAVE_ACPI_RESUME))
		return NULL;

	/*
	 * The buffer needs to be preallocated regardless. In the non-resume
	 * path it will be allocated for handling resume. Note that cbmem_add()
	 * does a find before the addition.
	 */
	save_area = cbmem_add(CBMEM_ID_SMM_SAVE_SPACE, SMM_DEFAULT_SIZE);

	if (save_area == NULL) {
		printk(BIOS_DEBUG, "SMM save area not added.\n");
		return NULL;
	}

	/* Only back up the area on S3 resume. */
	if (acpi_is_wakeup_s3()) {
		memcpy(save_area, default_smm, SMM_DEFAULT_SIZE);
		return save_area;
	}

	/*
	 * Not the S3 resume path. No need to restore memory contents after
	 * SMM relocation.
	 */
	return NULL;
}

void restore_default_smm_area(void *smm_save_area)
{
	void *default_smm = (void *)SMM_DEFAULT_BASE;

	if (smm_save_area == NULL)
		return;

	memcpy(default_smm, smm_save_area, SMM_DEFAULT_SIZE);
}
