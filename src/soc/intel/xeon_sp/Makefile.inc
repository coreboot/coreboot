##
## This file is part of the coreboot project.
##
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(CONFIG_XEON_SP_COMMON_BASE),y)

subdirs-$(CONFIG_SOC_INTEL_SKYLAKE_SP) += skx
subdirs-$(CONFIG_SOC_INTEL_COOPERLAKE_SP) += cpx

bootblock-y += bootblock.c spi.c lpc.c gpio.c
romstage-y += romstage.c reset.c util.c spi.c gpio.c
ramstage-y += uncore.c reset.c util.c lpc.c spi.c gpio.c
postcar-y += spi.c

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/include
CPPFLAGS_common += -I$(CONFIG_FSP_HEADER_PATH)

endif ## XEON_SP_COMMON_BASE
