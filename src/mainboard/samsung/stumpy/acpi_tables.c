/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <device/device.h>
#include <soc/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Enable Front USB ports in S3 by default */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/*
	 * Enable Front USB ports in S5 by default
	 * to be consistent with back port behavior
	 */
	gnvs->s5u0 = 1;
	gnvs->s5u1 = 1;
}
