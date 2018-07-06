bootblock-y += bootblock.c
bootblock-y += memlayout.ld
decompressor-y += memlayout.ld

verstage-y += chromeos.c
verstage-y += memlayout.ld

romstage-y += chromeos.c
romstage-y += memlayout.ld
romstage-y += romstage.c

ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += memlayout.ld
