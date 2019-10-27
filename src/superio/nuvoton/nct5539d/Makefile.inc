##
## This file is part of the coreboot project.
##
## Copyright (C) 2019 Pavel Sayekat <pavelsayekat@gmail.com>
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

ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5539D) += superio.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5539D) += ../../common/ssdt.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT5539D) += ../../common/generic.c
