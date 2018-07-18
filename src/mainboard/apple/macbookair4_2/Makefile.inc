romstage-y += gpio.c
ramstage-y += gnvs.c

cbfs-files-y += spd.bin
spd.bin-file := spd.bin
spd.bin-type := spd

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
