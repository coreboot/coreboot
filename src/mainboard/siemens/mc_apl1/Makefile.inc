bootblock-y += bootblock.c

# The inclusion of romstage.c is not necessary here.
# It is put down only to the better understanding.
# The file is already included over src/arch/x86/Makefile.inc.
romstage-y += romstage.c

ramstage-y += mainboard.c

all-$(CONFIG_NC_FPGA_POST_CODE) += post.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
