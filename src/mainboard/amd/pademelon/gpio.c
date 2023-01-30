/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/agesawrapper.h>
#include <amdblocks/BiosCallOuts.h>
#include <gpio.h>
#include <soc/southbridge.h>
#include "gpio.h"

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	/* GFX presence detect */
	PAD_GPI(GPIO_9, PULL_DOWN),
	/* VDDP_VCTRL */
	PAD_GPO(GPIO_40, HIGH),
	/* PC SPKR */
	PAD_NF(GPIO_91, SPKR, PULL_NONE),
};

static const struct soc_amd_gpio gpio_set_stage_ram[] = {
#if CONFIG(HAVE_ACPI_RESUME)
	/* PCIE_WAKE - default, do not program */

	/* DEVSLP1 */
	PAD_NF(GPIO_70, DEVSLP1, PULL_UP),
	/* WLAND */
	PAD_WAKE(GPIO_137, PULL_UP, LEVEL_LOW, S3),
#else
	/* PCIE_WAKE, SCI */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_UP, EDGE_LOW),
	/* DEVSLP1 - default as GPIO, do not program */

	/* WLAND - default as GPIO, do not program */

#endif /* HAVE_ACPI_RESUME */
	/* BLINK - reselect GPIO OUTPUT HIGH to force BLINK */
	PAD_GPO(GPIO_11, HIGH),
};

const struct soc_amd_gpio *early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_reset);
	return gpio_set_stage_reset;
}

const struct soc_amd_gpio *gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_ram);
	return gpio_set_stage_ram;
}
