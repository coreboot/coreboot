sdram-params :=
sdram-params += sdram-lpddr4x-MT29VZZZBD9DQKPR-046-6GB
sdram-params += sdram-lpddr4x-MT29VZZZAD8GQFSL-046-4GB

$(foreach params,$(sdram-params), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(params).c:struct) \
	$(eval $(params)-type := struct) \
	$(eval $(params)-compression := $(CBFS_COMPRESS_FLAG)) \
)
