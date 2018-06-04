subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm

romstage-y += cpureginit.c
romstage-y += syspreinit.c
romstage-y += msrinit.c

ramstage-y += geode_lx_init.c
ramstage-y += cpubug.c

cpu_incs-y += $(src)/cpu/amd/geode_lx/cache_as_ram.inc

cbfs-files-$(CONFIG_GEODE_VSA_FILE) += vsa
vsa-file = $(call strip_quotes,$(CONFIG_VSA_FILENAME)):vsa
vsa-type = stage
vsa-required = VSA binary (binary and MASM source code available in coreboot/3rdparty/blobs repository)
