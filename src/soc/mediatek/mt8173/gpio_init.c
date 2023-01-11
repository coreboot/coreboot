/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/mipi.h>

/*
 * GPIO 47-56 are controlled by MIPI register by default.
 * When they are used as GPI, we have to set IES of MIPI register to 1.
 * Additionally, pulls of these pins are controlled by MIPI,
 * and pull-setting of these pins are forbidden in our driver.
 */
static void set_gpi_from_mipi(void)
{
	setbits32(&mt8173_mipi->mipi_rx_ana4c,
		1 << 0  |	/* RG_MIPI_GPI0_IES GPI47 */
		1 << 6  |	/* RG_MIPI_GPI1_IES GPI48 */
		1 << 12 |	/* RG_MIPI_GPI2_IES GPI49 */
		1 << 18 |	/* RG_MIPI_GPI3_IES GPI50 */
		1 << 24);	/* RF_MIPI_GPI4_IES GPI51 */

	setbits32(&mt8173_mipi->mipi_rx_ana50,
		1 << 0  |	/* RG_MIPI_GPI5_IES GPI52 */
		1 << 6  |	/* RG_MIPI_GPI6_IES GPI53 */
		1 << 12 |	/* RG_MIPI_GPI7_IES GPI54 */
		1 << 18 |	/* RG_MIPI_GPI8_IES GPI55 */
		1 << 24);	/* RF_MIPI_GPI9_IES GPI56 */
}

/*
 * overwrite the T/RDSEL default value of exmd_ctrl and
 * msdc2_ctrl5 as b'1010
 */
static void gpio_set_duty(enum external_power ext_power)
{
	/* EXMD control reg */
	if (ext_power == GPIO_EINT_1P8V) {
		/* exmd_ctrl[9:4] = b`000000, [3:0] = b`1010 */
		write16(&mtk_gpio->exmd_ctrl[0].rst, 0x3F5);
		write16(&mtk_gpio->exmd_ctrl[0].set, 0xA);
	} else if (ext_power == GPIO_EINT_3P3V) {
		/* exmd_ctrl[9:4] = b`001100, [3:0] = b`1010 */
		write16(&mtk_gpio->exmd_ctrl[0].rst, 0x335);
		write16(&mtk_gpio->exmd_ctrl[0].set, 0xCA);
	}

	/* other R/TDSEL */
	/* msdc2_ctrl5, bit[3:0] = b`1010 */
	write16(&mtk_gpio->msdc2_ctrl5.set, 0xA);
	write16(&mtk_gpio->msdc2_ctrl5.rst, 0x5);
}

void gpio_init(enum external_power ext_power)
{
	set_gpi_from_mipi();
	gpio_set_duty(ext_power);
}
