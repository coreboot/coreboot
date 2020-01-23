# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

ramstage-$(CONFIG_SUPERIO_FINTEK_F71808A) += f71808a_multifunc.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F71808A) += f71808a_hwm.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F71808A) += superio.c
