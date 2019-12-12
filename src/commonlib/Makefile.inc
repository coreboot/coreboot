subdirs-y += storage

bootblock-y += mem_pool.c
verstage-y += mem_pool.c
romstage-y += mem_pool.c
ramstage-y += mem_pool.c
postcar-y += mem_pool.c

bootblock-y += iobuf.c
verstage-y += iobuf.c
romstage-y += iobuf.c
ramstage-y += iobuf.c
smm-y += iobuf.c
postcar-y += iobuf.c

bootblock-y += region.c
verstage-y += region.c
romstage-y += region.c
ramstage-y += region.c
smm-y += region.c
postcar-y += region.c

ramstage-$(CONFIG_PLATFORM_USES_FSP1_1) += fsp_relocate.c
ramstage-$(CONFIG_PLATFORM_USES_FSP2_0) += fsp_relocate.c

bootblock-y += cbfs.c
verstage-y += cbfs.c
romstage-y += cbfs.c
ramstage-y += cbfs.c
smm-y += cbfs.c
postcar-y += cbfs.c

bootblock-y += bsd/cbfs_private.c
verstage-y += bsd/cbfs_private.c
romstage-y += bsd/cbfs_private.c
postcar-y += bsd/cbfs_private.c
ramstage-y += bsd/cbfs_private.c
smm-y += bsd/cbfs_private.c

bootblock-y += bsd/cbfs_mcache.c
verstage-y += bsd/cbfs_mcache.c
romstage-y += bsd/cbfs_mcache.c
postcar-y += bsd/cbfs_mcache.c
ramstage-y += bsd/cbfs_mcache.c
smm-y += bsd/cbfs_mcache.c

decompressor-y += bsd/lz4_wrapper.c
bootblock-y += bsd/lz4_wrapper.c
verstage-y += bsd/lz4_wrapper.c
romstage-y += bsd/lz4_wrapper.c
ramstage-y += bsd/lz4_wrapper.c
postcar-y += bsd/lz4_wrapper.c

ramstage-y += sort.c
