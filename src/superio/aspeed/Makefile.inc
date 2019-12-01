##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Ronald G. Minnich
## Copyright (C) 2018 Eltan B.V.
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

## include generic fintek pre-ram stage driver
romstage-$(CONFIG_SUPERIO_ASPEED_COMMON_PRE_RAM) += common/early_serial.c
bootblock-$(CONFIG_SUPERIO_ASPEED_COMMON_PRE_RAM) += common/early_serial.c

subdirs-y += ast2400
subdirs-y += common
