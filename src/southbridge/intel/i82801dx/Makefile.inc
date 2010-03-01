driver-y += i82801dx.o
driver-y += i82801dx_usb.o
driver-y += i82801dx_lpc.o
driver-y += i82801dx_ide.o
driver-y += i82801dx_usb2.o
driver-y += i82801dx_ac97.o
#driver-y += i82801dx_nic.o
#driver-y += i82801dx_pci.o
obj-y += i82801dx_reset.o

obj-$(CONFIG_HAVE_SMI_HANDLER) += i82801dx_smi.o
smmobj-$(CONFIG_HAVE_SMI_HANDLER) += i82801dx_smihandler.o
