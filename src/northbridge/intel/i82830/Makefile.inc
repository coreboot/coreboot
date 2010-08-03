driver-y += northbridge.o
driver-y += vga.o

smmobj-$(CONFIG_HAVE_SMI_HANDLER) += i82830_smihandler.o
