#*****************************************************************************
#
# Copyright (C) 2012 Advanced Micro Devices, Inc.
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

# CIMX Root directory
CIMX_ROOT = $(src)/vendorcode/amd/cimx

SB_CIMX_INC  = -I$(src)/mainboard/$(MAINBOARDDIR)
SB_CIMX_INC  += -I$(src)/southbridge/amd/cimx/sb700
SB_CIMX_INC  += -I$(src)/include/cpu/amd/common
SB_CIMX_INC  += -I$(CIMX_ROOT)/sb700

romstage-y += ACPILIB.c
romstage-y += AMDLIB.c
romstage-y += AMDSBLIB.c
romstage-y += AZALIA.c
romstage-y += DEBUG.c
romstage-y += DISPATCHER.c
romstage-y += EC.c
romstage-y += FLASH.c
romstage-y += SATA.c
romstage-y += SBCMN.c
romstage-y += SBCMNLIB.c
romstage-y += SBMAIN.c
romstage-y += SBPOR.c
romstage-y += SMM.c
romstage-y += USB.c

ramstage-y += ACPILIB.c
ramstage-y += AMDLIB.c
ramstage-y += AMDSBLIB.c
ramstage-y += AZALIA.c
ramstage-y += DEBUG.c
ramstage-y += DISPATCHER.c
ramstage-y += EC.c
ramstage-y += FLASH.c
ramstage-y += SATA.c
ramstage-y += SBCMN.c
ramstage-y += SBCMNLIB.c
ramstage-y += SBMAIN.c
ramstage-y += SBPOR.c
ramstage-y += SMM.c
ramstage-y += USB.c
ramstage-y += LEGACY.c

SB_CIMX_CFLAGS =
export CIMX_ROOT
export SB_CIMX_INC
export SB_CIMX_CFLAGS
CC := $(CC) $(SB_CIMX_CFLAGS) $(SB_CIMX_INC)

#######################################################################

