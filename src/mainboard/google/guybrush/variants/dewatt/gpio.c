/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>

/* This table is used by guybrush variant */
static const struct soc_amd_gpio override_ramstage_gpio_table[] = {
	/* Unused TP247*/
	PAD_NC(GPIO_3),
	/* Unused TP218*/
	PAD_NC(GPIO_4),
	/* Unused TP245*/
	PAD_NC(GPIO_8),
	/* Unused TP244*/
	PAD_NC(GPIO_11),
	/* Unused TP194*/
	PAD_NC(GPIO_17),
	/* Unused TP195*/
	PAD_NC(GPIO_18),
	/* Unused TP243*/
	PAD_NC(GPIO_21),
	/* Unused TP196*/
	PAD_NC(GPIO_24),
	/* Unused TP219*/
	PAD_NC(GPIO_42),
	/* Unused TP217*/
	PAD_NC(GPIO_69),
	/* Unused TP235*/
	PAD_NC(GPIO_115),
	/* Unused TP205*/
	PAD_NC(GPIO_116),
	/* Unused TP226*/
	PAD_NC(GPIO_140),
	/* Unused TP225*/
	PAD_NC(GPIO_142),
	/* Unused TP227*/
	PAD_NC(GPIO_144),
	/* SOC_TCHPAD_INT_ODL */
	PAD_SCI(GPIO_9, PULL_NONE, LEVEL_LOW),
};

static const struct soc_amd_gpio override_early_gpio_table[] = {
	/* Unused TP245*/
	PAD_NC(GPIO_8),
	/* Unused TP195*/
	PAD_NC(GPIO_18),
	/* Unused TP196*/
	PAD_NC(GPIO_24),
	/* Unused TP217*/
	PAD_NC(GPIO_69),
};

/* This table is used by guybrush variant */
static const struct soc_amd_gpio override_romstage_gpio_table[] = {
	/* Unused TP195*/
	PAD_NC(GPIO_18),
	/* Unused TP217*/
	PAD_NC(GPIO_69),
};


static const struct soc_amd_gpio override_bootblock_gpio_table[] = {
	/* Unused TP196*/
	PAD_NC(GPIO_24),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_ramstage_gpio_table);
	return override_ramstage_gpio_table;
}

const struct soc_amd_gpio *variant_early_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_early_gpio_table);
	return override_early_gpio_table;
}

const struct soc_amd_gpio *variant_romstage_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_romstage_gpio_table);
	return override_romstage_gpio_table;
}

const struct soc_amd_gpio *variant_bootblock_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_bootblock_gpio_table);
	return override_bootblock_gpio_table;
}
