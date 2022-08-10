## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_IBEXPEAK),y)

bootblock-y += bootblock.c

ramstage-y += pch.c
ramstage-y += azalia.c
ramstage-y += fadt.c
ramstage-y += lpc.c
ramstage-y += ../bd82x6x/pci.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += me.c
ramstage-y += ../bd82x6x/me_common.c
ramstage-y += smbus.c
ramstage-y += thermal.c
ramstage-y += ../common/pciehp.c

ramstage-y += ../bd82x6x/me_status.c

ramstage-$(CONFIG_ELOG) += ../bd82x6x/elog.c
ramstage-y += madt.c

smm-y += smihandler.c

romstage-y += early_pch.c
romstage-y +=../bd82x6x/early_me.c
romstage-y +=../bd82x6x/me_status.c
romstage-y += early_thermal.c
romstage-y += ../bd82x6x/early_rcba.c
romstage-y += early_cir.c
romstage-y += early_usb.c
romstage-y += setup_heci_uma.c

CPPFLAGS_common += -I$(src)/southbridge/intel/ibexpeak/include

endif
