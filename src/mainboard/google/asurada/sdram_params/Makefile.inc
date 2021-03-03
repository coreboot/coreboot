sdram-params :=

# Generic parameters
sdram-params += sdram-lpddr4x-EMCP-1RANK-4GB
sdram-params += sdram-lpddr4x-EMCP-2RANK-4GB
sdram-params += sdram-lpddr4x-EMCP-2RANK-8GB
sdram-params += sdram-lpddr4x-DISCRETE-1RANK-4GB
sdram-params += sdram-lpddr4x-DISCRETE-2RANK-4GB
sdram-params += sdram-lpddr4x-DISCRETE-2RANK-8GB
sdram-params += sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE

sdram-params += sdram-lpddr4x-MT29VZZZBD9DQKPR-046-6GB
sdram-params += sdram-lpddr4x-MT29VZZZAD8GQFSL-046-4GB
sdram-params += sdram-lpddr4x-KMDP6001DA-B425-4GB
sdram-params += sdram-lpddr4x-KMDV6001DA-B620-4GB

$(foreach params,$(sdram-params), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(params).c:struct) \
	$(eval $(params)-type := struct) \
	$(eval $(params)-compression := $(CBFS_COMPRESS_FLAG)) \
)
