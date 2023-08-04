## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_ARCH_X86),y)

ramstage-y += isa-dma.c
ramstage-y += i8259.c
ramstage-y += keyboard.c
ramstage-$(CONFIG_SPKMODEM) += spkmodem.c
romstage-$(CONFIG_SPKMODEM) += spkmodem.c

all_x86-y += i8254.c
smm-y += i8254.c

endif
