/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cbmem.h>
#include <device/device.h>
#include <string.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/intel/broadwell/chip.h>
#include <soc/intel/common/acpi.h>
#include <assert.h>

/* Save wake source information for calculating ACPI _SWS values */
int soc_fill_acpi_wake(uint32_t *pm1, uint32_t **gpe0)
{
	struct chipset_power_state *ps = cbmem_find(CBMEM_ID_POWER_STATE);
	static uint32_t gpe0_sts[GPE0_REG_MAX];
	int i;

	assert(ps != NULL);

	*pm1 = ps->pm1_sts & ps->pm1_en;

	/* Mask off GPE0 status bits that are not enabled */
	*gpe0 = &gpe0_sts[0];
	for (i = 0; i < GPE0_REG_MAX; i++)
		gpe0_sts[i] = ps->gpe0_sts[i] & ps->gpe0_en[i];

	return GPE0_REG_MAX;
}

static void s3_resume_prepare(void)
{
	struct global_nvs *gnvs;

	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(struct global_nvs));
	if (gnvs == NULL)
		return;

	if (!acpi_is_wakeup_s3())
		memset(gnvs, 0, sizeof(struct global_nvs));
}

void broadwell_init_pre_device(void *chip_info)
{
	s3_resume_prepare();
	broadwell_run_reference_code();
}
