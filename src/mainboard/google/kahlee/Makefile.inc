#
# This file is part of the coreboot project.
#
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
bootblock-y += BiosCallOuts.c
bootblock-y += ec.c

romstage-y += BiosCallOuts.c
romstage-y += chromeos.c
romstage-y += OemCustomize.c

ramstage-y += BiosCallOuts.c
ramstage-y += chromeos.c
ramstage-y += ec.c
ramstage-y += OemCustomize.c

verstage-y += chromeos.c
verstage-y += ec.c

smm-y += smihandler.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
