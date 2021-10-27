CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += gpio_early.c

romstage-y += romstage.c

ramstage-y += ramstage.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
