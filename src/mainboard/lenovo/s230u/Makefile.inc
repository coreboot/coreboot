bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += ec.c

# FIXME: SPD images for samsung_8gb and hynix_8gb are missing.
# It's possible that no mainboards with that variation were manufactured.
SPD_SOURCES  = elpida_4gb			# 0b0000
SPD_SOURCES += samsung_4gb			# 0b0001
SPD_SOURCES += hynix_4gb			# 0b0010
SPD_SOURCES += elpida_8gb			# 0b0011
SPD_SOURCES += elpida_8gb			# 0b0100  FIXME: samsung_8gb
SPD_SOURCES += elpida_8gb			# 0b0101  FIXME: hynix_8gb
SPD_SOURCES += elpida_2gb			# 0b0110
SPD_SOURCES += samsung_2gb			# 0b0111
SPD_SOURCES += hynix_2gb			# 0b1000

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
bootblock-y += early_init.c
romstage-y += early_init.c
