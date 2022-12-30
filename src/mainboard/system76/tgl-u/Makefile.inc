CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

ifeq ($(CONFIG_BOARD_SYSTEM76_GALP5),y)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
endif

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/gpio_early.c

romstage-y += variants/$(VARIANT_DIR)/romstage.c

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-y += variants/$(VARIANT_DIR)/ramstage.c

SPD_SOURCES = samsung-M471A1G44AB0-CWE
