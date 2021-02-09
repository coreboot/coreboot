CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += gpio_early.c

ramstage-y += ramstage.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += tas5825m.c
