
bootblock-y += memlayout.ld
bootblock-y += chromeos.c
bootblock-y += bootblock.c

verstage-y += memlayout.ld
verstage-y += chromeos.c

romstage-y += memlayout.ld
romstage-y += chromeos.c
romstage-y += romstage.c

ramstage-y += memlayout.ld
ramstage-y += chromeos.c
ramstage-y += mainboard.c
