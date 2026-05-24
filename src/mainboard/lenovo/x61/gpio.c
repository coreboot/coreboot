/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ThinkPad X61 GPIO configuration.
 *
 * Verified against inteltool.log from physical hardware running vendor BIOS:
 *   USE_SEL  = 0xf97e03fe  IO_SEL  = 0xc1ee6dfe  LVL  = 0xe392ddff
 *   GPI_INV  = 0x000039ff
 *   USE_SEL2 = 0x00010ef6  IO_SEL2 = 0x005501f1  LVL2 = 0x00fe0373
 *   USE_SEL3 = 0x00000000  IO_SEL3 = 0x00000000  LVL3 = 0x00000000
 *
 * ICH8M-E GPIO is register-compatible with the ICH7 common GPIO framework.
 * For direction: 0 = output, 1 = input (GP_IO_SEL convention).
 * GPI_INV inverts the polarity of GPIO inputs for SMI/SCI edge detection.
 */

#include <gpio.h>
#include <northbridge/intel/gm965/gm965.h>
#include <southbridge/intel/common/gpio.h>
#include <drivers/i2c/ck505/chip.h>

/* ================================================================== */
/* GPIO Set 1 - GPIO0..31                                             */
/* ================================================================== */

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	/* GPIO0  = native (SATA_GP#) */
	.gpio1  = GPIO_MODE_GPIO,  /* Input, active low (inverted) */
	.gpio2  = GPIO_MODE_GPIO,  /* Input, active low (inverted) */
	.gpio3  = GPIO_MODE_GPIO,  /* Input, active low (inverted) */
	.gpio4  = GPIO_MODE_GPIO,  /* Input, active low (inverted) */
	.gpio5  = GPIO_MODE_GPIO,  /* Input, active low (inverted) */
	.gpio6  = GPIO_MODE_GPIO,  /* Input, active low (inverted) */
	.gpio7  = GPIO_MODE_GPIO,  /* BDC_PRESENCE# (Bluetooth), active low */
	.gpio8  = GPIO_MODE_GPIO,  /* H8_WAKE# (EC wakes host), active low */
	.gpio9  = GPIO_MODE_GPIO,  /* Output, driven low */
	/* GPIO10-16 = native (USB OC, CLKRUN#, etc.) */
	.gpio17 = GPIO_MODE_GPIO,  /* Input */
	.gpio18 = GPIO_MODE_GPIO,  /* Input */
	.gpio19 = GPIO_MODE_GPIO,  /* Input */
	.gpio20 = GPIO_MODE_GPIO,  /* Output, driven high */
	.gpio21 = GPIO_MODE_GPIO,  /* Input */
	.gpio22 = GPIO_MODE_GPIO,  /* Input */
	/* GPIO23 = native */
	.gpio24 = GPIO_MODE_GPIO,  /* Input */
	/* GPIO25-26 = native */
	.gpio27 = GPIO_MODE_GPIO,  /* Output, driven low */
	.gpio28 = GPIO_MODE_GPIO,  /* Output, driven low */
	.gpio29 = GPIO_MODE_GPIO,  /* Output, driven high */
	.gpio30 = GPIO_MODE_GPIO,  /* Input */
	.gpio31 = GPIO_MODE_GPIO,  /* Input */
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio1  = GPIO_DIR_INPUT,
	.gpio2  = GPIO_DIR_INPUT,
	.gpio3  = GPIO_DIR_INPUT,
	.gpio4  = GPIO_DIR_INPUT,
	.gpio5  = GPIO_DIR_INPUT,
	.gpio6  = GPIO_DIR_INPUT,
	.gpio7  = GPIO_DIR_INPUT,
	.gpio8  = GPIO_DIR_INPUT,
	.gpio9  = GPIO_DIR_OUTPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio18 = GPIO_DIR_INPUT,
	.gpio19 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_OUTPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
	.gpio30 = GPIO_DIR_INPUT,
	.gpio31 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	/* Only output GPIOs need explicit level; unset fields default to LOW */
	.gpio20 = GPIO_LEVEL_HIGH,
	.gpio29 = GPIO_LEVEL_HIGH,
	/* gpio9=LOW, gpio27=LOW, gpio28=LOW: default */
};

/* GPI_INV vendor value is 0x000039ff. */
static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio0 = GPIO_INVERT,
	.gpio1 = GPIO_INVERT,
	.gpio2 = GPIO_INVERT,
	.gpio3 = GPIO_INVERT,
	.gpio4 = GPIO_INVERT,
	.gpio5 = GPIO_INVERT,
	.gpio6 = GPIO_INVERT,
	.gpio7 = GPIO_INVERT,  /* BDC_PRESENCE#, active low */
	.gpio8 = GPIO_INVERT,  /* H8_WAKE#, active low */
	.gpio11 = GPIO_INVERT,
	.gpio12 = GPIO_INVERT,
	.gpio13 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
};

/* ================================================================== */
/* GPIO Set 2 - GPIO32..63                                            */
/* ================================================================== */

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	/* GPIO32 = native */
	.gpio33 = GPIO_MODE_GPIO,  /* HDA_DOCK_EN#, output driven high */
	.gpio34 = GPIO_MODE_GPIO,  /* Output, driven low */
	/* GPIO35 = native */
	.gpio36 = GPIO_MODE_GPIO,  /* Input */
	.gpio37 = GPIO_MODE_GPIO,  /* Input */
	.gpio38 = GPIO_MODE_GPIO,  /* Input */
	.gpio39 = GPIO_MODE_GPIO,  /* Input */
	/* GPIO40 = native */
	.gpio41 = GPIO_MODE_GPIO,  /* Output, driven high */
	.gpio42 = GPIO_MODE_GPIO,  /* SMBus mux: HIGH=DIMMs/SPD, LOW=AT24RF08C EEPROM */
	.gpio43 = GPIO_MODE_GPIO,  /* Output, driven low */
	/* GPIO44-47 = native */
	.gpio48 = GPIO_MODE_GPIO,  /* Input */
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio34 = GPIO_DIR_OUTPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio41 = GPIO_DIR_OUTPUT,
	.gpio42 = GPIO_DIR_OUTPUT,
	.gpio43 = GPIO_DIR_OUTPUT,
	.gpio48 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio33 = GPIO_LEVEL_HIGH,
	.gpio41 = GPIO_LEVEL_HIGH,
	/*
	 * GPIO42 = SMBus mux: HIGH routes SMBus to DIMMs (for SPD reads),
	 * LOW routes SMBus to the AT24RF08C EEPROM.
	 * Start HIGH so raminit can read SPD; mb_post_raminit_setup() will
	 * switch it LOW afterward, matching the X200/T400 pattern.
	 */
	.gpio42 = GPIO_LEVEL_HIGH,
	/* gpio34=LOW, gpio43=LOW: default */
};

/* ================================================================== */
/* GPIO map                                                            */
/* ================================================================== */

const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.blink		= &pch_gpio_set1_blink,
		.invert		= &pch_gpio_set1_invert,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
	},
};

enum smbus_mux {
	SMBUS_TO_EEPROM,
	SMBUS_TO_CK505_SPD,
};

static void set_smbus_mux(enum smbus_mux mux)
{
	switch (mux)
	{
	default:
	case SMBUS_TO_EEPROM:
		gpio_set(42, GPIO_LEVEL_LOW);
		break;
	case SMBUS_TO_CK505_SPD:
		gpio_set(42, GPIO_LEVEL_HIGH);
		break;
	}
}

void mb_post_raminit_setup(void)
{
	set_smbus_mux(SMBUS_TO_EEPROM);
}

void mb_pre_ck505_init(void)
{
	set_smbus_mux(SMBUS_TO_CK505_SPD);

}
void mb_post_ck505_init(void)
{
	set_smbus_mux(SMBUS_TO_EEPROM);
}
