# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += superio.c
bootblock-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += early_init.c
romstage-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += early_init.c
