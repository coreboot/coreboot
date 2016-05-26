bootblock-y += bootblock.c
bootblock-y += ec.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c
