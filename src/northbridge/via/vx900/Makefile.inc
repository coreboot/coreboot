##
## This file is part of the coreboot project.
##
## Copyright (C) 2011-2013 Alexandru Gagniuc <mr.nuke.me@gmail.com>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(CONFIG_NORTHBRIDGE_VIA_VX900),y)

romstage-y += pci_util.c
romstage-y += early_smbus.c
romstage-y += early_vx900.c
romstage-y += early_host_bus_ctl.c
romstage-y += raminit_ddr3.c
romstage-y += memmap.c
romstage-y += ./../../../device/dram/ddr3.c
romstage-y += ./../../../southbridge/via/common/early_smbus_delay.c
romstage-y += ./../../../southbridge/via/common/early_smbus_is_busy.c
romstage-y += ./../../../southbridge/via/common/early_smbus_print_error.c
romstage-y += ./../../../southbridge/via/common/early_smbus_reset.c
romstage-y += ./../../../southbridge/via/common/early_smbus_wait_until_ready.c
romstage-y += ./../../../drivers/pc80/pc/udelay_io.c

ramstage-y += pci_util.c
ramstage-y += pcie.c
ramstage-y += northbridge.c
ramstage-y += chrome9hd.c
ramstage-y += traf_ctrl.c
ramstage-y += sata.c
ramstage-y += lpc.c
ramstage-y += memmap.c

# The buildsystem only includes this file if CONFIG_VGA is selected.
# We need to do some VGA I/O before the VGA can be initialized. We can make good
# use of some of the functions there, so include them unconditionally
ramstage-y += ./../../../drivers/pc80/vga/vga_io.c


bootblock-y += romstrap.ld
bootblock-y += romstrap.S

endif
