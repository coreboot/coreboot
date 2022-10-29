## SPDX-License-Identifier: GPL-2.0-only
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += gpio.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += gpio.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += gpio.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += gpio.c
verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += gpio.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += gpio_dev.c
