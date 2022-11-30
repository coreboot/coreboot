bootblock-y += memlayout.ld
bootblock-y += bootblock.c
bootblock-y += chromeos.c

verstage-y += memlayout.ld
verstage-y += chromeos.c
verstage-y += reset.c

romstage-y += memlayout.ld
romstage-y += chromeos.c
romstage-y += regulator.c
romstage-y += reset.c
romstage-y += romstage.c
romstage-y += sdram_configs.c

ramstage-y += memlayout.ld
ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += display.c
ramstage-y += mainboard.c
ramstage-y += panel.c
ramstage-y += panel_geralt.c
ramstage-y += regulator.c
ramstage-y += reset.c
