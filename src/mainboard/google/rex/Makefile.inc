bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += mainboard.c

subdirs-y += variants/baseboard/$(BASEBOARD_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/$(BASEBOARD_DIR)/include
