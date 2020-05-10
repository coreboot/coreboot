# SPDX-License-Identifier: GPL-2.0-or-later

ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += superio.c
bootblock-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += early_init.c
romstage-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += early_init.c
