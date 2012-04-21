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

NB_CIMX_INC  = -I$(src)/mainboard/$(MAINBOARDDIR)
NB_CIMX_INC  += -I$(src)/northbridge/amd/cimx/rd890
NB_CIMX_INC  += -I$(src)/include/cpu/amd/common
NB_CIMX_INC  += -I$(CIMX_ROOT)/rd890

romstage-y += amdAcpiIvrs.c
romstage-y += amdAcpiLib.c
romstage-y += amdAcpiMadt.c
romstage-y += amdDebugOutLib.c
romstage-y += amdSbLib.c
#romstage-y += nbDispatcher.c
romstage-y += nbEventLog.c
romstage-y += nbHtInit.c
romstage-y += nbHtInterface.c
romstage-y += nbInit.c
romstage-y += nbInitializer.c
romstage-y += nbInterface.c
romstage-y += nbIoApic.c
romstage-y += nbIommu.c
romstage-y += nbLib.c
romstage-y += nbMaskedMemoryInit.c
romstage-y += nbMiscInit.c
romstage-y += nbModuleInfo.c
romstage-y += nbPcieAspm.c
romstage-y += nbPcieCplBuffers.c
romstage-y += nbPcieEarlyHwLib.c
romstage-y += nbPcieHotplug.c
romstage-y += nbPcieInitEarly.c
romstage-y += nbPcieInitLate.c
romstage-y += nbPcieLateHwLib.c
romstage-y += nbPcieLib.c
romstage-y += nbPcieLinkWidth.c
romstage-y += nbPciePllControl.c
romstage-y += nbPciePortRemap.c
#romstage-y += nbPcieRecovery.c
romstage-y += nbPcieSb.c
romstage-y += nbPcieWorkarounds.c
romstage-y += nbPowerOnReset.c
#romstage-y += nbRecovery.c
#romstage-y += nbRecoveryInitializer.c
romstage-y += nbMaskedMemoryInit32.S

ramstage-y += amdAcpiIvrs.c
ramstage-y += amdAcpiLib.c
ramstage-y += amdAcpiMadt.c
ramstage-y += amdDebugOutLib.c
ramstage-y += amdSbLib.c
#ramstage-y += nbDispatcher.c
ramstage-y += nbEventLog.c
ramstage-y += nbHtInit.c
ramstage-y += nbHtInterface.c
ramstage-y += nbInit.c
ramstage-y += nbInitializer.c
ramstage-y += nbInterface.c
ramstage-y += nbIoApic.c
ramstage-y += nbIommu.c
ramstage-y += nbLib.c
ramstage-y += nbMaskedMemoryInit.c
ramstage-y += nbMiscInit.c
ramstage-y += nbModuleInfo.c
ramstage-y += nbPcieAspm.c
ramstage-y += nbPcieCplBuffers.c
ramstage-y += nbPcieEarlyHwLib.c
ramstage-y += nbPcieHotplug.c
ramstage-y += nbPcieInitEarly.c
ramstage-y += nbPcieInitLate.c
ramstage-y += nbPcieLateHwLib.c
ramstage-y += nbPcieLib.c
ramstage-y += nbPcieLinkWidth.c
ramstage-y += nbPciePllControl.c
ramstage-y += nbPciePortRemap.c
#ramstage-y += nbPcieRecovery.c
ramstage-y += nbPcieSb.c
ramstage-y += nbPcieWorkarounds.c
ramstage-y += nbPowerOnReset.c
#ramstage-y += nbRecovery.c
#ramstage-y += nbRecoveryInitializer.c
ramstage-y += nbMaskedMemoryInit32.S

NB_CIMX_CFLAGS =
export CIMX_ROOT
export NB_CIMX_INC
export NB_CIMX_CFLAGS
CC := $(CC) $(NB_CIMX_CFLAGS) $(NB_CIMX_INC)

#######################################################################

