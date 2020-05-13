# SPDX-License-Identifier: GPL-2.0-only

CONFIG_GPIO_C:=$(call strip_quotes, $(CONFIG_GPIO_C))

bootblock-y += early_init.c
romstage-y += early_init.c

ramstage-y += cstates.c
romstage-y += $(CONFIG_GPIO_C)
