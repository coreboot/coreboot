# SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_XILINX_ZYNQ7000),y)

bootblock-y += timer.c
romstage-y += timer.c
ramstage-y += timer.c

romstage-y += cbmem.c
ramstage-y += soc.c

bootblock-y += reset.c
romstage-y += reset.c
ramstage-y += reset.c

endif
