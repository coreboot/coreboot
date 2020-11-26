bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += mainboard.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
