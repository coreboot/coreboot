
#default CONFIG_AGP_APERTURE_SIZE=0x4000000
#default CONFIG_HAVE_HIGH_TABLES=1


driver-y += northbridge.o
driver-y += misc_control.o
obj-y += get_sblk_pci1234.o
obj-$(CONFIG_HAVE_ACPI_TABLES) += amdk8_acpi.o

# Not sure what to do with these yet. How did raminit_test even work?
# Should be a target in -y form.
#if CONFIG_K8_REV_F_SUPPORT
#
#makerule raminit_test
#	depends "$(TOP)/src/northbridge/amd/amdk8/raminit_test.c"
#	depends "$(TOP)/src/northbridge/amd/amdk8/raminit_f.c"
#	action "$(HOSTCC) $(HOSTCFLAGS) $(CPUFLAGS) -Wno-unused-function -I$(TOP)/src/include -g  $< -o $@"
#end
#
#else
#
#makerule raminit_test
#	depends "$(TOP)/src/northbridge/amd/amdk8/raminit_test.c"
#	depends "$(TOP)/src/northbridge/amd/amdk8/raminit.c"
#	action "$(HOSTCC) $(HOSTCFLAGS) $(CPUFLAGS) -Wno-unused-function -I$(TOP)/src/include -g  $< -o $@"
#end
#
#end
#
