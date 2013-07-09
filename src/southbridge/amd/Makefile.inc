subdirs-$(CONFIG_SOUTHBRIDGE_AMD_AMD8111) += amd8111
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_AMD8131) += amd8131
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_AMD8132) += amd8132
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_AMD8151) += amd8151
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_RS690) += rs690
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_SB600) += sb600
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_RS780) += rs780
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_SB700) += sb700
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_SB800) += sb800
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_SR5650) += sr5650
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_CS5530) += cs5530
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_CS5535) += cs5535
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_CS5536) += cs5536
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_CIMX_SB700) += cimx
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_CIMX_SB800) += cimx
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_CIMX_SB900) += cimx
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_AGESA_HUDSON) += agesa
subdirs-$(CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE) += agesa

ifeq ($(CONFIG_HAVE_ACPI_RESUME), y)
ifeq ($(CONFIG_CPU_AMD_AGESA), y)

$(obj)/coreboot_s3nv.rom: $(obj)/config.h
	echo "    S3 NVRAM   $(CONFIG_S3_DATA_POS) (S3 storage area)"
	# force C locale, so cygwin awk doesn't try to interpret the 0xff below as UTF-8 (or worse)
	printf %d $(CONFIG_S3_DATA_SIZE) | LC_ALL=C awk '{for (i=0; i<$$1; i++) {printf "%c", 255}}' > $@.tmp
	mv $@.tmp $@

cbfs-files-y += s3nv
s3nv-file := $(obj)/coreboot_s3nv.rom
s3nv-position := $(CONFIG_S3_DATA_POS)
s3nv-type := raw

endif # CONFIG_CPU_AMD_AGESA == y
endif # CONFIG_HAVE_ACPI_RESUME == y
