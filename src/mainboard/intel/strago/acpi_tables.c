/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>
#include <soc/device_nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* Enable DPTF */
	gnvs->dpte = 1;

	/* PMIC is configured in I2C1, hidden it from OS */
	struct device_nvs *dev_nvs = acpi_get_device_nvs();
	dev_nvs->lpss_en[LPSS_NVS_I2C2] = 0;
}

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_MOBILE;
}
