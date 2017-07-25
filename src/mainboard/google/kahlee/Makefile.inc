#
# This file is part of the coreboot project.
#
# Copyright (C) 2015-2017 Advanced Micro Devices, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/BiosCallOuts.c
bootblock-y += bootblock/OemCustomize.c
bootblock-y += ec.c
bootblock-y += gpio.c

romstage-y += BiosCallOuts.c
romstage-y += boardid.c
romstage-y += chromeos.c
romstage-y += gpio.c
romstage-y += OemCustomize.c

ramstage-y += BiosCallOuts.c
ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += ec.c
ramstage-y += gpio.c
ramstage-y += OemCustomize.c

verstage-y += chromeos.c
verstage-y += ec.c
