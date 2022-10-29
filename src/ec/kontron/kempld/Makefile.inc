## SPDX-License-Identifier: GPL-2.0-only
bootblock-$(CONFIG_EC_KONTRON_KEMPLD) += early_kempld.c
ramstage-$(CONFIG_EC_KONTRON_KEMPLD) += early_kempld.c
ramstage-$(CONFIG_EC_KONTRON_KEMPLD) += kempld.c
ramstage-$(CONFIG_EC_KONTRON_KEMPLD) += kempld_i2c.c
ramstage-$(CONFIG_EC_KONTRON_KEMPLD) += kempld_gpio.c
