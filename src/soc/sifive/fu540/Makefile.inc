# This file is part of the coreboot project.
#
# Copyright (C) 2018 Jonathan Neuschäfer
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

ifeq ($(CONFIG_SOC_SIFIVE_FU540),y)

bootblock-y += uart.c
bootblock-y += media.c
bootblock-y += bootblock.c

romstage-y += uart.c
romstage-y += media.c

ramstage-y += uart.c
ramstage-y += media.c
ramstage-y += cbmem.c

CPPFLAGS_common += -Isrc/soc/sifive/fu540/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "    GPT        $(notdir $(@))\n"
	@util/riscv/sifive-gpt.py $< $@

endif
