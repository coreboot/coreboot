## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_UCB_RISCV),y)

romstage-y += cbmem.c

ramstage-y += cbmem.c

ramstage-y += chip.c

endif
