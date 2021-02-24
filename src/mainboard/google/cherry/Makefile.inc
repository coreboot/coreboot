bootblock-y += memlayout.ld
bootblock-y += bootblock.c
bootblock-y += chromeos.c

verstage-y += memlayout.ld
verstage-y += chromeos.c
verstage-y += reset.c

romstage-y += memlayout.ld
romstage-y += chromeos.c
romstage-y += romstage.c

ramstage-y += memlayout.ld
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += reset.c
