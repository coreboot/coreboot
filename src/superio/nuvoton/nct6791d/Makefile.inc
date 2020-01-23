# SPDX-License-Identifier: GPL-2.0-only
# This file is part of the coreboot project.

ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += superio.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += ../../common/ssdt.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += ../../common/generic.c
