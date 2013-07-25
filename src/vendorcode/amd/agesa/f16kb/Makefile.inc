#*****************************************************************************
#
# Copyright (c) 2012, Advanced Micro Devices, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Advanced Micro Devices, Inc. nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

# AGESA V5 Files
AGESA_ROOT = src/vendorcode/amd/agesa/f16kb

AGESA_INC =  -Isrc/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(AGESA_ROOT)/Lib
AGESA_INC += -I$(AGESA_ROOT)/Legacy
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x16
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x16/KB
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/KB
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Family/0x16
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Family/0x16
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Family/0x16
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Gfx
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Gfx/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxConfig
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieConfig
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxInitLibV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbNbInitLibV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbInitKB
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbSbLib
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieTrainingV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Family/0x16/KB
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/NbCommon
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Main
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Library
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbNbInitLibV4
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbIommuIvrs
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbIvrsLib
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbSbIommuLib
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbTable
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV4
AGESA_INC += -I$(AGESA_ROOT)/Proc/Fch
AGESA_INC += -I$(AGESA_ROOT)/Proc/Fch/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Debug
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieAspm
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Include/Library
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbSmuLibV7
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbNbInitLibV5
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieTrainingV2
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV5
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/CRAT
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxIntTableV3
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/RDWR2DTRAINING

AGESA_INC += -I$(src)/southbridge/amd/agesa/hudson

AGESA_CFLAGS =-march=k8-sse3 -mtune=k8-sse3 -fno-zero-initialized-in-bss -fno-strict-aliasing

export AGESA_ROOT := $(AGESA_ROOT)
export AGESA_INC  := $(AGESA_INC)
export AGESA_CFLAGS  := $(AGESA_CFLAGS)
CC := $(CC) $(AGESA_INC) $(AGESA_CFLAGS)
#######################################################################
