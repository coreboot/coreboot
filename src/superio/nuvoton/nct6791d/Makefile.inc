##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Omar Pakker <omarpakker+coreboot@gmail.com>
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

ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += superio.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += ../../common/ssdt.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += ../../common/generic.c
