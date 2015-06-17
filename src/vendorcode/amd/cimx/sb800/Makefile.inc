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
# Foundation, Inc.
#

CPPFLAGS_x86_32 += -I$(src)/mainboard/$(MAINBOARDDIR)
CPPFLAGS_x86_32 += -I$(src)/southbridge/amd/cimx/sb800
CPPFLAGS_x86_32 += -I$(src)/include/cpu/amd/common
CPPFLAGS_x86_32 += -I$(src)/vendorcode/amd/cimx/sb800

CPPFLAGS_x86_64 += -I$(src)/mainboard/$(MAINBOARDDIR)
CPPFLAGS_x86_64 += -I$(src)/southbridge/amd/cimx/sb800
CPPFLAGS_x86_64 += -I$(src)/include/cpu/amd/common
CPPFLAGS_x86_64 += -I$(src)/vendorcode/amd/cimx/sb800

romstage-y += ACPILIB.c
romstage-y += AZALIA.c
romstage-y += DISPATCHER.c
romstage-y += ECfanc.c
romstage-y += ECfanLIB.c
romstage-y += GEC.c
romstage-y += Gpp.c
romstage-y += PMIO2LIB.c
romstage-y += SATA.c
romstage-y += SBCMN.c
romstage-y += SBMAIN.c
romstage-y += SBPort.c
romstage-y += MEMLIB.c
romstage-y += PCILIB.c
romstage-y += IOLIB.c
romstage-y += PMIOLIB.c
romstage-y += AMDLIB.c
romstage-y += SBPELIB.c
romstage-y += AMDSBLIB.c
romstage-y += ECLIB.c
romstage-y += EC.c
romstage-y += SMM.c
romstage-y += USB.c

ramstage-y += ACPILIB.c
ramstage-y += AZALIA.c
ramstage-y += DISPATCHER.c
ramstage-y += ECfanc.c
ramstage-y += ECfanLIB.c
ramstage-y += GEC.c
ramstage-y += Gpp.c
ramstage-y += PMIO2LIB.c
ramstage-y += SATA.c
ramstage-y += SBCMN.c
ramstage-y += SBMAIN.c
ramstage-y += SBPort.c
ramstage-y += MEMLIB.c
ramstage-y += PCILIB.c
ramstage-y += IOLIB.c
ramstage-y += PMIOLIB.c
ramstage-y += AMDLIB.c
ramstage-y += SBPELIB.c
ramstage-y += AMDSBLIB.c
ramstage-y += ECLIB.c
ramstage-y += EC.c
ramstage-y += SMM.c
ramstage-y += USB.c
#ramstage-y += LEGACY.c
#ramstage-y += SbModInf.c
