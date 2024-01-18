# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-$(CONFIG_SUPERIO_NSC_PC87417) += early_init.c
romstage-$(CONFIG_SUPERIO_NSC_PC87417) += early_init.c
ramstage-$(CONFIG_SUPERIO_NSC_PC87417) += superio.c
