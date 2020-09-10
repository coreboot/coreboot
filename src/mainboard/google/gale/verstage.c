/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <gpio.h>
#include <soc/verstage.h>

#define TPM_RESET_GPIO		19

static void ipq_setup_tpm(void)
{

	if (CONFIG(I2C_TPM)) {
		gpio_tlmm_config_set(TPM_RESET_GPIO, FUNC_SEL_GPIO,
				     GPIO_PULL_UP, GPIO_6MA, 1);
		gpio_set(TPM_RESET_GPIO, 0);
		udelay(100);
		gpio_set(TPM_RESET_GPIO, 1);

		/*
		 * ----- Per the SLB 9615XQ1.2 spec -----
		 *
		 * 4.7.1 Reset Timing
		 *
		 * The TPM_ACCESS_x.tpmEstablishment bit has the correct value
		 * and the TPM_ACCESS_x.tpmRegValidSts bit is typically set
		 * within 8ms after RESET# is deasserted.
		 *
		 * The TPM is ready to receive a command after less than 30 ms.
		 *
		 * --------------------------------------
		 *
		 * I'm assuming this means "wait for 30ms"
		 *
		 * If we don't wait here, subsequent QUP I2C accesses
		 * to the TPM either fail or timeout.
		 */
		mdelay(30);
	}
}

void verstage_mainboard_init(void)
{
	ipq_setup_tpm();
}
