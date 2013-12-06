#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 Advanced Micro Devices, Inc.
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

## ABSOLUTE AGESA V5 ROOT PATH ##
AGESA_ROOT = src/vendorcode/amd/agesa/f10

AGESA_INC ?= -I$(src)/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(AGESA_ROOT)/Lib
AGESA_INC += -I$(AGESA_ROOT)/Legacy
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/NbCommon
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x10
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/IDENDIMM/
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/HY
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Main
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Family/0x10
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/Mem

## AGESA need sse feature ##
AGESA_CFLAGS = -msse3 -fno-zero-initialized-in-bss -fno-strict-aliasing

export AGESA_ROOT
export AGESA_INC
export AGESA_CFLAGS
CC := $(CC) $(AGESA_INC) $(AGESA_CFLAGS)

