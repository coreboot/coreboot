/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <superio/smsc/sch5545/sch5545.h>
#include <superio/smsc/sch5545/sch5545_emi.h>

#include <baseboard/sch5545_ec.h>

static uint16_t emi_bar;

static const struct ec_val_reg ec_gpio_init_table[] = {
	/*
	 * Probably some early GPIO initialization, setting GPIO functions.
	 * The LSBs in third column match the GPIO config registers offsets for
	 * non-default GPIOs.
	 */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x08cc }, /* GP063 (def) / KBDRST# */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x08d0 }, /* GP064 (def) / A20M */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x089c }, /* GP047 / TXD1 (def) */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x0878 }, /* GP036 (def) / SMBCLK1 */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x0880 }, /* GP040 (def) / SMBDAT1 */
	{ EC_GPIO_OD | EC_GPIO_FUNC1, 0x0884 }, /* GP041 (def) / IO_PME# */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x08e4 }, /* GP071 (def) / IO_SMI# */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x08e0 }, /* GP070 (def) / SPEAKER */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x0848 }, /* GP022 (def) / PWM1 */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x084c }, /* GP023 (def) / PWM2 */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x0850 }, /* GP024 (def) / PWM3 */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x083c }, /* GP017 / TACH1 (def) */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x0840 }, /* GP020 / TACH2 (def) */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x0844 }, /* GP021 / TACH3 (def) */
	{ EC_GPIO_PP | EC_GPIO_FUNC1, 0x0814 }, /* GP005 (def) / PECI_REQ# */
};

static const struct ec_val_reg ec_hwm_early_init_table[] = {
	/* Probably some early hardware monitor initialization */
	{ 0xff, 0x0005 },
	{ 0x30, 0x00f0 },
	{ 0x10, 0x00f8 },
	{ 0x00, 0x00f9 },
	{ 0x00, 0x00fa },
	{ 0x00, 0x00fb },
	{ 0x00, 0x00ea },
	{ 0x00, 0x00eb },
	{ 0x7c, 0x00ef },
	{ 0x03, 0x006e },
	{ 0x51, 0x02d0 },
	{ 0x01, 0x02d2 },
	{ 0x12, 0x059a },
	{ 0x11, 0x059e },
	{ 0x14, 0x05a2 },
	{ 0x55, 0x05a3 },
	{ 0x01, 0x02db },
	{ 0x01, 0x0040 },
};

static void ec_read_write_reg_timeout(uint16_t ldn, uint8_t *val, uint16_t reg,
				      uint8_t rw_bit)
{
	uint16_t timeout = 0;
	rw_bit &= 1;
	sch5545_emi_ec2h_mailbox_clear();
	sch5545_emi_ec_write16(0x8000, (ldn << 1) | 0x100 | rw_bit);

	sch5545_emi_set_ec_addr(0x8004);

	if (rw_bit)
		outb(*val, emi_bar + SCH5545_EMI_EC_DATA);

	outb(reg & 0xff, emi_bar + SCH5545_EMI_EC_DATA + 2);
	outb((reg >> 8) & 0xff, emi_bar + SCH5545_EMI_EC_DATA + 3);
	sch5545_emi_h2ec_mbox_write(1);

	do {
		timeout++;
		if ((sch5545_emi_ec2h_mbox_read() & 1) != 0)
			break;
	} while (timeout < 0xfff);

	sch5545_emi_set_int_src(0x11);
	sch5545_emi_h2ec_mbox_write(0xc0);

	if (!rw_bit)
		*val = inb(emi_bar + SCH5545_EMI_EC_DATA);
}

static void ec_init_gpios(void)
{
	unsigned int i;
	uint8_t val;

	for (i = 0; i < ARRAY_SIZE(ec_gpio_init_table); i++) {
		val = ec_gpio_init_table[i].val;
		ec_read_write_reg_timeout(EC_GPIO_LDN, &val, ec_gpio_init_table[i].reg,
					  WRITE_OP);
	}
}

static void ec_early_hwm_init(void)
{
	unsigned int i;
	uint8_t val;

	for (i = 0; i < ARRAY_SIZE(ec_hwm_early_init_table); i++) {
		val = ec_hwm_early_init_table[i].val;
		ec_read_write_reg_timeout(EC_HWM_LDN, &val, ec_hwm_early_init_table[i].reg,
					  WRITE_OP);
	}
}

void sch5545_ec_early_init(void)
{
	emi_bar = sch5545_read_emi_bar(0x2e);

	ec_init_gpios();
	ec_early_hwm_init();
}
