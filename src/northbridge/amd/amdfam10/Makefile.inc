ifeq ($(CONFIG_NORTHBRIDGE_AMD_AMDFAM10),y)

ramstage-y += northbridge.c
ramstage-y += misc_control.c
ramstage-y += link_control.c
ramstage-y += nb_control.c
romstage-y += amdfam10_util.c
ramstage-y += amdfam10_util.c

ramstage-y += ht_config.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c

ramstage-y += get_pci1234.c

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

ramstage-$(CONFIG_DIMM_DDR3) += ../amdmct/mct_ddr3/s3utils.c

endif
