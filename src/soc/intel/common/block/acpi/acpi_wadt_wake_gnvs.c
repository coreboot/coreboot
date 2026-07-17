/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/acpi_wake_source.h>
#include <device/mmio.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <stdint.h>

#define WADT_AC_REG		0x1800
#define WADT_DC_REG		0x1804
#define WADT_DISABLED		((uint32_t)~0)
#define WADT_AC_STATUS		(1 << 0)
#define WADT_DC_STATUS		(1 << 1)
#define WADT_STATUS_MASK	(WADT_AC_STATUS | WADT_DC_STATUS)

void acpi_fill_wadt_wake_gnvs(struct global_nvs *gnvs, const uint32_t *gpe0)
{
	uint8_t armed = gnvs->wadt_armed & WADT_STATUS_MASK;
	uint8_t status = 0;

	if (!(gpe0[GPE_STD] & WADT_STS)) {
		gnvs->wadt = 0;
		return;
	}

	if (read32p(PCH_PWRM_BASE_ADDRESS + WADT_AC_REG) == WADT_DISABLED)
		status |= WADT_AC_STATUS;
	if (read32p(PCH_PWRM_BASE_ADDRESS + WADT_DC_REG) == WADT_DISABLED)
		status |= WADT_DC_STATUS;

	/* Report only timers that were both expired and previously armed. */
	gnvs->wadt = status & armed;
}
