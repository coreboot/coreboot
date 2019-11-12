subdirs-y += sdram_params/
subdirs-y += panel_params/

bootblock-y += boardid.c
bootblock-y += bootblock.c
bootblock-y += memlayout.ld
bootblock-y += reset.c
decompressor-y += memlayout.ld

verstage-y += chromeos.c
verstage-y += early_init.c
verstage-y += reset.c
verstage-y += verstage.c
verstage-y += memlayout.ld

romstage-y += boardid.c
romstage-y += chromeos.c
romstage-y += early_init.c
romstage-y += memlayout.ld
romstage-y += reset.c
romstage-y += romstage.c
romstage-y += sdram_configs.c

ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += memlayout.ld
ramstage-$(CONFIG_BOARD_GOOGLE_FLAPJACK) += panel_flapjack.c
ramstage-$(CONFIG_BOARD_GOOGLE_KODAMA) += panel_kodama.c
ramstage-$(CONFIG_BOARD_GOOGLE_KRANE) += panel_krane.c
ramstage-$(CONFIG_BOARD_GOOGLE_KUKUI) += panel_kukui.c
ramstage-$(CONFIG_DRIVER_PARADE_PS8640) += panel_ps8640.c
ramstage-$(CONFIG_DRIVER_ANALOGIX_ANX7625) += panel_anx7625.c

ramstage-y += reset.c
