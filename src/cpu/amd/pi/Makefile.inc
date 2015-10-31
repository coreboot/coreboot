#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

subdirs-$(CONFIG_CPU_AMD_PI_00630F01) += 00630F01
subdirs-$(CONFIG_CPU_AMD_PI_00730F01) += 00730F01
subdirs-$(CONFIG_CPU_AMD_PI_00660F01) += 00660F01

romstage-y += s3_resume.c
ramstage-y += s3_resume.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c

cpu_incs-y += $(src)/cpu/amd/pi/cache_as_ram.inc

romstage-y += heapmanager.c
ramstage-y += heapmanager.c
ramstage-y += amd_late_init.c

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
