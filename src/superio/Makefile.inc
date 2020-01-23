# SPDX-License-Identifier: GPL-2.0-only
# This file is part of the coreboot project.

subdirs-y += aspeed
subdirs-y += fintek
subdirs-y += intel
subdirs-y += ite
subdirs-y += nsc
subdirs-y += nuvoton
subdirs-y += renesas
subdirs-y += serverengines
subdirs-y += smsc
subdirs-y += via
subdirs-y += winbond

ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += common/conf_mode.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_64) += common/conf_mode.c
