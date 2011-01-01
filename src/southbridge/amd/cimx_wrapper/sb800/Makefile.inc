#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Advanced Micro Devices, Inc.
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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#

subdirs-y += ../../../../../src/vendorcode/amd/cimx/sb800
#TODO: merge with agesa lib function
subdirs-y += ../../../../../src/vendorcode/amd/cimx/lib

# SB800 Platform Files

romstage-y += sb800_cfg.c
romstage-y += sb800_early.c
romstage-y += sb800_smbus.c

ramstage-y += sb800_cfg.c
ramstage-y += sb800_late.c

driver-y   += sb800_smbus.c
driver-y   += sb800_lpc.c

