bootblock-y += bootblock.c

# The inclusion of romstage.c is not necessary here.
# It is put down only to the better understanding.
# The file is already included over src/arch/x86/Makefile.inc.
romstage-y += romstage.c
romstage-y += gpio.c

ramstage-y += mainboard.c
ramstage-y += gpio.c
