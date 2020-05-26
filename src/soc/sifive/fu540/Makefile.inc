# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_SIFIVE_FU540),y)

bootblock-y += uart.c
bootblock-y += clint.c
bootblock-y += spi.c
bootblock-y += bootblock.c
bootblock-y += clock.c

romstage-y += uart.c
romstage-y += clint.c
romstage-y += spi.c
romstage-y += sdram.c
romstage-y += cbmem.c
romstage-y += otp.c
romstage-y += clock.c

ramstage-y += uart.c
ramstage-y += clint.c
ramstage-y += spi.c
ramstage-y += sdram.c
ramstage-y += cbmem.c
ramstage-y += otp.c
ramstage-y += clock.c
ramstage-y += chip.c

CPPFLAGS_common += -Isrc/soc/sifive/fu540/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "    GPT        $(notdir $(@))\n"
	@util/riscv/sifive-gpt.py $< $@

endif
