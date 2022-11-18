/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/cdp.h>
#include <soc/ebi2.h>
#include <soc/clock.h>
#include <types.h>
#include <boardid.h>

void ipq_configure_gpio(const gpio_func_data_t *gpio, unsigned int count)
{
	int i;

	for (i = 0; i < count; i++) {
		gpio_tlmm_config(gpio->gpio, gpio->func, gpio->dir,
				gpio->pull, gpio->drvstr, gpio->enable);
		gpio++;
	}
}

static void configure_nand_gpio(void)
{
	/* EBI2 CS, CLE, ALE, WE, OE */
	gpio_tlmm_config(34, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(35, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(36, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(37, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(38, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);

	/* EBI2 BUSY */
	gpio_tlmm_config(39, 1, 0, GPIO_PULL_UP, GPIO_10MA, GPIO_DISABLE);

	/* EBI2 D7 - D0 */
	gpio_tlmm_config(40, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(41, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(42, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(43, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(44, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(45, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(46, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(47, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
}

void board_nand_init(void)
{
	struct ebi2cr_regs *ebi2_regs;

	if (board_id() != BOARD_ID_PROTO_0_2_NAND)
		return;

	ebi2_regs = (struct ebi2cr_regs *)EBI2CR_BASE;

	nand_clock_config();
	configure_nand_gpio();

	/* NAND Flash is connected to CS0 */
	clrsetbits32(&ebi2_regs->chip_select_cfg0, CS0_CFG_MASK,
		     CS0_CFG_SERIAL_FLASH_DEVICE);
}
