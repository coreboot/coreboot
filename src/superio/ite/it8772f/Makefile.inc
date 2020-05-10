# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-$(CONFIG_SUPERIO_ITE_IT8772F) += early_init.c
romstage-$(CONFIG_SUPERIO_ITE_IT8772F) += early_init.c
ramstage-$(CONFIG_SUPERIO_ITE_IT8772F) += superio.c
smm-$(CONFIG_SUPERIO_ITE_IT8772F) += early_init.c
