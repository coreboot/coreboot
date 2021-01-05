sdram-params :=
sdram-params += sdram-lpddr4x-K4UBE3D4AA-MGCR-4GB
sdram-params += sdram-lpddr4x-H9HCNNNCPMALHR-4GB
sdram-params += sdram-lpddr4x-H9HCNNNCPMMLXR-NEE-4GB
sdram-params += sdram-lpddr4x-K4UBE3D4AA-MGCL-4GB
sdram-params += sdram-lpddr4x-KMDH6001DA-B422-4GB
sdram-params += sdram-lpddr4x-KMDP6001DA-B425-4GB
sdram-params += sdram-lpddr4x-KMDV6001DA-B620-4GB
sdram-params += sdram-lpddr4x-MT29VZZZAD8DQKSL-4GB
sdram-params += sdram-lpddr4x-MT53E1G32D4NQ-4GB
sdram-params += sdram-lpddr4x-MT53E2G32D4NQ-046-8GB
sdram-params += sdram-lpddr4x-SDADA4CR-128G-4GB
sdram-params += sdram-lpddr4x-H9HCNNNFAMMLXR-NEE-8GB
sdram-params += sdram-lpddr4x-MT53E1G32D2NP-046-4GB

$(foreach params,$(sdram-params), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(params).c:struct) \
	$(eval $(params)-type := struct) \
	$(eval $(params)-compression := $(CBFS_COMPRESS_FLAG)) \
)
