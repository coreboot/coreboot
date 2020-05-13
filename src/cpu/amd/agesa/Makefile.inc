# SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY14) += family14
subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY15_TN) += family15tn
subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY16_KB) += family16kb

ifeq ($(CONFIG_HAVE_ACPI_RESUME), y)

$(obj)/coreboot_s3nv.rom: $(obj)/config.h
	echo "    S3 NVRAM   $(CONFIG_S3_DATA_POS) (S3 storage area)"
	# force C locale, so cygwin awk doesn't try to interpret the 0xff below as UTF-8 (or worse)
	printf %d $(CONFIG_S3_DATA_SIZE) | LC_ALL=C awk '{for (i=0; i<$$1; i++) {printf "%c", 255}}' > $@.tmp
	mv $@.tmp $@

cbfs-files-y += s3nv
s3nv-file := $(obj)/coreboot_s3nv.rom
s3nv-position := $(CONFIG_S3_DATA_POS)
s3nv-type := raw

endif # CONFIG_HAVE_ACPI_RESUME == y
