# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_SIFIVE_FU740),y)

CPPFLAGS_common += -Isrc/soc/sifive/fu740/include

bootblock-y += uart.c
bootblock-y += clint.c
bootblock-y += spi.c
bootblock-y += sdram.c
bootblock-y += cbmem.c
bootblock-y += otp.c
bootblock-y += clock.c
bootblock-y += ddrregs.c
bootblock-y += chip.c
bootblock-y += gpio.c


ramstage-y += uart.c
ramstage-y += clint.c
ramstage-y += spi.c
ramstage-y += sdram.c
ramstage-y += cbmem.c
ramstage-y += otp.c
ramstage-y += clock.c
ramstage-y += chip.c

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "    GPT        $(notdir $(@))\n"
	@util/riscv/sifive-gpt.py $< $@

endif
