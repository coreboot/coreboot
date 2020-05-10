/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA210_PAD_CFG_H
#define __SOC_NVIDIA_TEGRA210_PAD_CFG_H

#include <stdint.h>
#include <soc/pinmux.h>

struct pad_config {
	uint16_t pinmux_flags;	/* PU/PU, OD, INPUT, SFIO, etc */
	uint8_t gpio_index;		/* bank, port, index */
	uint16_t pinmux_index:9;
	uint16_t unused:1;
	uint16_t sfio:1;
	uint16_t gpio_out0:1;
	uint16_t gpio_out1:1;
	uint16_t pad_has_gpio:1;
	uint16_t por_pullup:1;
};

#define PAD_CFG_GPIO_INPUT(ball_, pinmux_flgs_)		\
	{						\
		.pinmux_flags = pinmux_flgs_ | PINMUX_INPUT_ENABLE,	\
		.gpio_index = PAD_TO_GPIO_##ball_,	\
		.pinmux_index = PINMUX_##ball_##_INDEX,	\
		.sfio = 0,				\
		.pad_has_gpio = PAD_HAS_GPIO_##ball_,	\
	}

#define PAD_CFG_GPIO_OUT0(ball_, pinmux_flgs_)		\
	{						\
		.pinmux_flags = pinmux_flgs_,		\
		.gpio_index = PAD_TO_GPIO_##ball_,	\
		.pinmux_index = PINMUX_##ball_##_INDEX,	\
		.sfio = 0,				\
		.gpio_out0 = 1,				\
		.pad_has_gpio = PAD_HAS_GPIO_##ball_,	\
	}

#define PAD_CFG_GPIO_OUT1(ball_, pinmux_flgs_)		\
	{						\
		.pinmux_flags = pinmux_flgs_,		\
		.gpio_index = PAD_TO_GPIO_##ball_,	\
		.pinmux_index = PINMUX_##ball_##_INDEX,	\
		.sfio = 0,				\
		.gpio_out1 = 1,				\
		.pad_has_gpio = PAD_HAS_GPIO_##ball_,	\
	}

#define PAD_CFG_SFIO(ball_, pinmux_flgs_, sfio_)	\
	{						\
		.pinmux_flags = pinmux_flgs_ |		\
				PINMUX_##ball_##_FUNC_##sfio_,	\
		.gpio_index = PAD_TO_GPIO_##ball_,	\
		.pinmux_index = PINMUX_##ball_##_INDEX,	\
		.sfio = 1,				\
		.pad_has_gpio = PAD_HAS_GPIO_##ball_,	\
	}

#define PAD_CFG_UNUSED(ball_)				\
	{						\
		.gpio_index = PAD_TO_GPIO_##ball_,	\
		.pinmux_index = PINMUX_##ball_##_INDEX,	\
		.unused = 1,				\
		.pad_has_gpio = PAD_HAS_GPIO_##ball_,	\
	}
/*
 * Configure the pads associated with entry according to the configuration.
 */
void soc_configure_pads(const struct pad_config * const entries, size_t num);
/* I2C6 requires special init as its pad lives int the SOR/DPAUX block */
void soc_configure_i2c6pad(void);
void soc_configure_host1x(void);
/* APE (Audio Processing Engine) requires special init */
void soc_configure_ape(void);

#endif /* __SOC_NVIDIA_TEGRA210_PAD_CFG_H */
