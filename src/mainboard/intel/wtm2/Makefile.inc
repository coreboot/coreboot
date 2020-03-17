##
## This file is part of the coreboot project.
##
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

romstage-y += gpio.c

verstage-y += chromeos.c
romstage-y += chromeos.c
ramstage-y += chromeos.c

romstage-y += pei_data.c
ramstage-y += pei_data.c
