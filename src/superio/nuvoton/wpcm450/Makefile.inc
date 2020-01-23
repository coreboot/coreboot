# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

bootblock-$(CONFIG_SUPERIO_NUVOTON_WPCM450) += early_init.c
romstage-$(CONFIG_SUPERIO_NUVOTON_WPCM450) += early_init.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_WPCM450) += superio.c
