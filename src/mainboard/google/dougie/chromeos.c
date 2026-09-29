/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <drivers/tpm/cr50.h>

int cr50_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_GSC_AP_INT);
}
