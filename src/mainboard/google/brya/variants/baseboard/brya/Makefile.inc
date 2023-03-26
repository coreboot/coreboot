bootblock-y += gpio.c

romstage-y += memory.c
romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-y += ramstage.c

smm-$(CONFIG_SMMSTORE) += gpio.c
