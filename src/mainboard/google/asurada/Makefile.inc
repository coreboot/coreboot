subdirs-y += sdram_params/

bootblock-y += memlayout.ld
bootblock-y += bootblock.c

verstage-y += memlayout.ld
verstage-y += reset.c

romstage-y += memlayout.ld
romstage-y += boardid.c
romstage-y += romstage.c
romstage-y += sdram_configs.c

ramstage-y += memlayout.ld
ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += reset.c
