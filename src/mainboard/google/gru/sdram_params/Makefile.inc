## SPDX-License-Identifier: GPL-2.0-only

sdram-params :=
sdram-params += sdram-lpddr3-generic-4GB-928
sdram-params += sdram-lpddr3-generic-4GB-800

sdram-params += sdram-lpddr3-generic-2GB-928
sdram-params += sdram-lpddr3-generic-2GB-800

$(foreach params,$(sdram-params), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(params).c:struct) \
	$(eval $(params)-type := struct) \
	$(eval $(params)-compression := $(CBFS_COMPRESS_FLAG)) \
)
