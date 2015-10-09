romstage-y += early_southbridge.c
romstage-y += gpio.c
ramstage-y += gnvs.c

cbfs-files-y += spd.bin
spd.bin-file := spd.bin
spd.bin-type := spd
