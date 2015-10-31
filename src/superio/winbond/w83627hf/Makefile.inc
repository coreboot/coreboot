##
## This file is part of the coreboot project.
##
## Copyright (C) 2000 AG Electronics Ltd.
## Copyright (C) 2003-2004 Linux Networx
## Copyright (C) 2004 Tyan By LYH change from PC87360
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

romstage-$(CONFIG_SUPERIO_WINBOND_W83627HF) += early_init.c
ramstage-$(CONFIG_SUPERIO_WINBOND_W83627HF) += superio.c
