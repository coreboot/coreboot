##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 Peter Stuge <peter@stuge.se>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

ifeq ($(CONFIG_SUPERIO_ITE_IT8712F),y)

STAGE0_CHIPSET_SRC += $(src)/superio/ite/it8712f/stage1.c
STAGE0_CHIPSET_SRC += $(src)/device/pnp_raw.c

# Always add to variables, as there could be more than one Super I/O.
STAGE2_CHIPSET_SRC += $(src)/superio/ite/it8712f/superio.c

endif
