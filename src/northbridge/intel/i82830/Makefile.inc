driver-y += northbridge.o
driver-$(CONFIG_VGA_ROM_RUN) += vga.o

smmobj-$(CONFIG_HAVE_SMI_HANDLER) += i82830_smihandler.o
