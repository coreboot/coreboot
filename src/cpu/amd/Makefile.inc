subdirs-$(CONFIG_CPU_AMD_SOCKET_F) += socket_F
subdirs-$(CONFIG_CPU_AMD_SOCKET_F_1207) += socket_F_1207
subdirs-$(CONFIG_CPU_AMD_SOCKET_754) += socket_754
subdirs-$(CONFIG_CPU_AMD_SOCKET_939) += socket_939
subdirs-$(CONFIG_CPU_AMD_SOCKET_940) += socket_940
subdirs-$(CONFIG_CPU_AMD_SOCKET_AM2) += socket_AM2
subdirs-$(CONFIG_CPU_AMD_SOCKET_AM2R2) += socket_AM2r2
subdirs-$(CONFIG_CPU_AMD_SOCKET_AM3) += socket_AM3
subdirs-$(CONFIG_CPU_AMD_SOCKET_ASB2) += socket_ASB2
subdirs-$(CONFIG_CPU_AMD_SOCKET_C32) += socket_C32
subdirs-$(CONFIG_CPU_AMD_GEODE_GX1) += geode_gx1
subdirs-$(CONFIG_CPU_AMD_GEODE_GX2) += geode_gx2
subdirs-$(CONFIG_CPU_AMD_GEODE_LX) += geode_lx
subdirs-$(CONFIG_CPU_AMD_SC520) += sc520
subdirs-$(CONFIG_CPU_AMD_SOCKET_S1G1) += socket_S1G1

subdirs-$(CONFIG_CPU_AMD_AGESA) += agesa

$(obj)/s3.rom:
	echo "    S3 NVRAM   0xffff0000 (S3 storage area)"
	echo -ne '\xFF' > $@
	for ((i=0;i<20479;i++)) do echo -ne '\xFF' >> $@ ; done

ifeq ($(CONFIG_HAVE_ACPI_RESUME), y)
cbfs-files-y += s3nv
s3nv-file := $(obj)/s3.rom
s3nv-position := 0xffff0000
s3nv-type := raw
endif
