ifeq ($(CONFIG_NORTHBRIDGE_AMD_AMDFAM10),y)

subdirs-y += ../amdht
subdirs-y += ../amdmct/wrappers
subdirs-$(CONFIG_DIMM_DDR3) += ../amdmct/mct_ddr3
subdirs-$(CONFIG_DIMM_DDR2) += ../amdmct/mct

# Generic ROMSTAGE stuff
romstage-y += reset_test.c debug.c setup_resource_map.c raminit_sysinfo_in_ram.c
romstage-y += raminit_amdmct.c pci.c early_ht.c amdfam10_util.c

# RAMSTAGE
ramstage-y += northbridge.c misc_control.c link_control.c nb_control.c
ramstage-y += amdfam10_util.c ht_config.c get_pci1234.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c

# Enable this if you want to check the values of the PCI routing registers.
# Call show_all_routes() anywhere amdfam10.h is included.
#ramstage-y += util.c

# Reserve 2x CONFIG_S3_DATA_SIZE to allow for random file placement
# (not respecting erase sector boundaries) within CBFS
$(obj)/coreboot_s3nv.rom: $(obj)/config.h
	echo "    S3 NVRAM   $(CONFIG_S3_DATA_POS) (S3 storage area)"
	# force C locale, so cygwin awk doesn't try to interpret the 0xff below as UTF-8 (or worse)
	printf %d $(CONFIG_S3_DATA_SIZE) | LC_ALL=C awk '{for (i=0; i<$$1*2; i++) {printf "%c", 255}}' > $@.tmp
	mv $@.tmp $@

cbfs-files-$(CONFIG_HAVE_ACPI_RESUME) += s3nv
s3nv-file := $(obj)/coreboot_s3nv.rom
s3nv-align := $(CONFIG_S3_DATA_SIZE)
s3nv-type := raw

endif
