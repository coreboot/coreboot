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
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

romstage-y += pci_util.c
romstage-y += early_smbus.c
romstage-y += early_vx900.c
romstage-y += early_host_bus_ctl.c
romstage-y += raminit_ddr3.c
romstage-y += ./../../../device/dram/ddr3.c
romstage-y += ./../../../southbridge/via/common/early_smbus_delay.c
romstage-y += ./../../../southbridge/via/common/early_smbus_is_busy.c
romstage-y += ./../../../southbridge/via/common/early_smbus_print_error.c
romstage-y += ./../../../southbridge/via/common/early_smbus_reset.c
romstage-y += ./../../../southbridge/via/common/early_smbus_wait_until_ready.c
romstage-y += ./../../../drivers/pc80/udelay_io.c

ramstage-y += pci_util.c
ramstage-y += pcie.c
ramstage-y += northbridge.c
ramstage-y += chrome9hd.c
ramstage-y += traf_ctrl.c
ramstage-y += sata.c
ramstage-y += lpc.c

# The buildsystem only includes this file if CONFIG_VGA is selected.
# We need to do some VGA I/O before the VGA can be initialized. We can make good
# use of some of the functions there, so include them unconditionally
ramstage-y += ./../../../drivers/pc80/vga/vga_io.c


chipset_bootblock_inc += $(src)/northbridge/via/vx900/romstrap.inc
chipset_bootblock_lds += $(src)/northbridge/via/vx900/romstrap.lds
