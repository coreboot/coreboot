#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

romstage-y += rd890_cfg.c
romstage-y += sb700_cfg.c
romstage-y += buildOpts.c
romstage-y += agesawrapper.c
romstage-y += BiosCallOuts.c
romstage-y += platform_oem.c

ramstage-y += rd890_cfg.c
ramstage-y += sb700_cfg.c
ramstage-y += buildOpts.c
ramstage-y += agesawrapper.c
ramstage-y += BiosCallOuts.c
ramstage-y += platform_oem.c

AGESA_PREFIX ?= $(src)/vendorcode/amd/agesa
CIMX_PREFIX ?= $(src)/vendorcode/amd/cimx
AGESA_ROOT ?= $(AGESA_PREFIX)/f15
NB_CIMX_ROOT ?= $(CIMX_PREFIX)/rd890
SB_CIMX_ROOT ?= $(CIMX_PREFIX)/sb700

subdirs-y += ../../../../$(AGESA_ROOT)

