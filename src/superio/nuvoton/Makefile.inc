##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 Advanced Micro Devices, Inc.
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

## include generic nuvoton pre-ram stage driver
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_SUPERIO_NUVOTON_COMMON_ROMSTAGE) += common/early_serial.c
endif
romstage-$(CONFIG_SUPERIO_NUVOTON_COMMON_ROMSTAGE) += common/early_serial.c

subdirs-$(CONFIG_SUPERIO_NUVOTON_WPCM450) += wpcm450
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += nct5104d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT5572D) += nct5572d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT6776) += nct6776
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT6779D) += nct6779d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += nct6791d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NPCD378) += npcd378
