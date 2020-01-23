# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

ramstage-$(CONFIG_SUPERIO_FINTEK_F71869AD) += f71869ad_multifunc.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F71869AD) += f71869ad_hwm.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F71869AD) += superio.c
