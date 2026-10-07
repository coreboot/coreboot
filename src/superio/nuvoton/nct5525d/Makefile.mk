# SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5525D) += superio.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5525D) += ../../common/ssdt.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5525D) += ../../common/generic.c
