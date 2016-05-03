bootblock-y += bootblock.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-y += mainboard.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos_ramstage.c
