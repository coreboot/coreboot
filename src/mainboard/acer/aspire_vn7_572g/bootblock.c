/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include "include/ec.h"
#include "include/gpio.h"

#define ADC_3V_10BIT_GRANULARITY_MAX	(3005 / 1023)
#define PCB_VER_AD			1
#define MODEL_ID_AD			3

#define DGPU_PRESENT	GPP_A20	/* Active low */
#define DGPU_HOLD_RST	GPP_B4	/* Active low */
#define DGPU_PWR_EN	GPP_B21	/* Active low */

/* TODO/NB: Detection is still unreliable. Is a wait required? */
static void board_detect(void)
{
	printk(BIOS_DEBUG, "Mainboard: Detecting board SKU\n");

	uint16_t data_buffer = read_ec_adc_converter(MODEL_ID_AD);
	printk(BIOS_DEBUG, "BoardId (raw) = 0x%x\n", data_buffer);
	printk(BIOS_DEBUG, "BoardId: ");
	/* Board by max millivoltage range (of 10-bit, 3.005 V ADC) */
	if (data_buffer <= (1374 / ADC_3V_10BIT_GRANULARITY_MAX)) {
		printk(BIOS_ERR, "Reserved?\n");
	} else if (data_buffer <= (2017 / ADC_3V_10BIT_GRANULARITY_MAX)) {
		printk(BIOS_DEBUG, "Aspire VN7-792G (Newgate-SLS_dGPU)\n");
		printk(BIOS_CRIT, "WARNING: This board is unsupported!\n");
		printk(BIOS_CRIT, "Damage may result from programming incorrect GPIO table!\n");
	} else if (data_buffer <= (2259 / ADC_3V_10BIT_GRANULARITY_MAX)) {
		printk(BIOS_DEBUG, "Aspire VN7-592G (Rayleigh-SLS_960M)\n");
		printk(BIOS_CRIT, "WARNING: This board is unsupported!\n");
		printk(BIOS_CRIT, "Damage may result from programming incorrect GPIO table!\n");
	} else {
		printk(BIOS_DEBUG, "Aspire VN7-572G (Rayleigh-SL_dGPU)\n");
	}

	data_buffer = read_ec_adc_converter(PCB_VER_AD);
	printk(BIOS_DEBUG, "PCB version (raw) = 0x%x\n", data_buffer);
	printk(BIOS_DEBUG, "PCB version: ");
	/* PCB by max millivoltage range (of 10-bit, 3.005 V ADC) */
	if (data_buffer <= (2017 / ADC_3V_10BIT_GRANULARITY_MAX)) {
		printk(BIOS_ERR, "Reserved?\n");
	} else if (data_buffer <= (2259 / ADC_3V_10BIT_GRANULARITY_MAX)) {
		printk(BIOS_DEBUG, "-1\n");
	} else if (data_buffer <= (2493 / ADC_3V_10BIT_GRANULARITY_MAX)) {
		printk(BIOS_DEBUG, "SC\n");
	} else if (data_buffer <= (2759 / ADC_3V_10BIT_GRANULARITY_MAX)) {
		printk(BIOS_DEBUG, "SB\n");
	} else {
		printk(BIOS_DEBUG, "SA\n");
	}
}

static void dgpu_power_on(void)
{
	if (!gpio_get(DGPU_PRESENT)) {
		printk(BIOS_DEBUG, "dGPU present, enable power...\n");
		gpio_set(DGPU_HOLD_RST, 0);	// Assert dGPU_HOLD_RST#
		mdelay(2);
		gpio_set(DGPU_PWR_EN, 0);	// Assert dGPU_PWR_EN#
		mdelay(7);
		gpio_set(DGPU_HOLD_RST, 1);	// Deassert dGPU_HOLD_RST#
		mdelay(30);
	} else {
		printk(BIOS_DEBUG, "dGPU not present, disable power...\n");
		gpio_set(DGPU_HOLD_RST, 0);	// Assert dGPU_HOLD_RST#
		gpio_set(DGPU_PWR_EN, 1);	// Deassert dGPU_PWR_EN#
	}
}

void bootblock_mainboard_init(void)
{
	/* NB: Relocated from _early_init() so that debug logging works.
	 * However, if we use this to ensure that the user flashed the correct
	 * (future) variant, this must occur before any GPIOs are programmed.
	 */
	board_detect();
	dgpu_power_on();
}

void bootblock_mainboard_early_init(void)
{
	mainboard_config_stage_gpios();
}
