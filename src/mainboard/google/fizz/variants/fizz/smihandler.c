/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <gpio.h>

#define GPIO_APEX_VCOM_EN	GPP_B20
#define GPIO_APEX0_VR_EN	GPP_C8
#define GPIO_APEX1_VR_EN	GPP_C9

void variant_smi_sleep(u8 slp_typ)
{
	/* Power down the Apex chips, which share a rail with the SSD */
	if (slp_typ >= ACPI_S3) {
		gpio_set(GPIO_APEX_VCOM_EN, 0);
		gpio_set(GPIO_APEX0_VR_EN, 0);
		gpio_set(GPIO_APEX1_VR_EN, 0);
	}
}
