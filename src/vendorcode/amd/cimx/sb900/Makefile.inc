#
#
# SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_x86_32 += -I$(src)/southbridge/amd/cimx/sb900
CPPFLAGS_x86_32 += -I$(src)/vendorcode/amd/cimx/sb900

CPPFLAGS_x86_64 += -I$(src)/southbridge/amd/cimx/sb900
CPPFLAGS_x86_64 += -I$(src)/vendorcode/amd/cimx/sb900

romstage-y += AcpiLib.c
romstage-y += Azalia.c
romstage-y += Dispatcher.c
romstage-y += EcFanc.c
romstage-y += EcFanLib.c
romstage-y += Gec.c
romstage-y += Gpp.c
romstage-y += Pmio2Lib.c
romstage-y += Sata.c
romstage-y += SbCmn.c
romstage-y += SbMain.c
romstage-y += SBPort.c
romstage-y += MemLib.c
romstage-y += PciLib.c
romstage-y += IoLib.c
romstage-y += PmioLib.c
romstage-y += AmdLib.c
romstage-y += SbPeLib.c
romstage-y += AmdSbLib.c
romstage-y += EcLib.c
romstage-y += Ec.c
romstage-y += Smm.c
romstage-y += Usb.c
romstage-y += Hwm.c

ramstage-y += AcpiLib.c
ramstage-y += Azalia.c
ramstage-y += Dispatcher.c
ramstage-y += EcFanc.c
ramstage-y += EcFanLib.c
ramstage-y += Gec.c
ramstage-y += Gpp.c
ramstage-y += Pmio2Lib.c
ramstage-y += Sata.c
ramstage-y += SbCmn.c
ramstage-y += SbMain.c
ramstage-y += SBPort.c
ramstage-y += MemLib.c
ramstage-y += PciLib.c
ramstage-y += IoLib.c
ramstage-y += PmioLib.c
ramstage-y += AmdLib.c
ramstage-y += SbPeLib.c
ramstage-y += AmdSbLib.c
ramstage-y += EcLib.c
ramstage-y += Ec.c
ramstage-y += Smm.c
ramstage-y += Usb.c
#ramstage-y += Legacy.c
#ramstage-y += SbModInf.c
ramstage-y += Debug.c
ramstage-y += Hwm.c
