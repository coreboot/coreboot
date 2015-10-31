##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Ronald G. Minnich
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

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
