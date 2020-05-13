## SPDX-License-Identifier: GPL-2.0-only

ramstage-y += ec.c

romstage-y += chromeos.c
ramstage-y += chromeos.c

SPD_BIN = $(obj)/spd.bin

# Include spd ROM data
$(SPD_BIN):
	xxd -rg1 $(src)/mainboard/samsung/lumpy/spd.hex >| $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
bootblock-y += gpio.c
romstage-y += gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
bootblock-y += early_init.c
romstage-y += early_init.c
