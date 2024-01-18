## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_PCI_OPTION_ROM_RUN_REALMODE) += x86.c
ramstage-$(CONFIG_PCI_OPTION_ROM_RUN_REALMODE) += x86_asm.S
ramstage-$(CONFIG_PCI_OPTION_ROM_RUN_REALMODE) += x86_interrupts.c
