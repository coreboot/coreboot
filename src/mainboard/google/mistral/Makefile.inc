
bootblock-y += memlayout.ld
bootblock-y += chromeos.c
bootblock-y += reset.c
bootblock-y += bootblock.c

verstage-y += memlayout.ld
verstage-y += chromeos.c
verstage-y += reset.c
verstage-y += verstage.c

romstage-y += memlayout.ld
romstage-y += chromeos.c
romstage-y += reset.c
romstage-y += romstage.c

ramstage-y += memlayout.ld
ramstage-y += chromeos.c
ramstage-y += reset.c
ramstage-y += mainboard.c
