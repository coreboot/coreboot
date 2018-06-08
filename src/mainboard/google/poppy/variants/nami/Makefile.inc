
# IMPORTANT!! When a new memory SPD is added here, please ensure that ddp_bitmap
# is correctly updated in memory.c to indicate if memory id is using DDP.
SPD_SOURCES = empty_ddr4			# 0b0000
SPD_SOURCES += micron_dimm_MT40A256M16GE-083E	# 0b0001
SPD_SOURCES += samsung_dimm_K4A8G165WB-BCRC	# 0b0010
SPD_SOURCES += hynix_dimm_H5AN4G6NBJR-UHC	# 0b0011
SPD_SOURCES += hynix_dimm_H5ANAG6NAMR-UHC	# 0b0100
SPD_SOURCES += empty_ddr4			# 0b0101
SPD_SOURCES += hynix_dimm_H5AN8G6NAFR-UHC	# 0b0110
SPD_SOURCES += samsung_dimm_K4A4G165WE-BCRC	# 0b0111
SPD_SOURCES += samsung_dimm_K4A8G165WC-BCTD	# 0b1000
SPD_SOURCES += empty_ddr4			# 0b1001
SPD_SOURCES += empty_ddr4			# 0b1010
SPD_SOURCES += empty_ddr4			# 0b1011
SPD_SOURCES += empty_ddr4			# 0b1100
SPD_SOURCES += empty_ddr4			# 0b1101
SPD_SOURCES += micron_dimm_MT40A512M16LY-075E	# 0b1110
SPD_SOURCES += hynix_dimm_H5AN8G6NCJR-VKC	# 0b1111

# Used when PCH_MEM_CONFIG4 = 1 (LPDDR3)
SEC_SPD_SOURCES += micron_dimm_MT52L512M32D2PF-107 # 0b0000
SEC_SPD_SOURCES += micron_dimm_MT52L256M32D1PF-107 # 0b0001
SEC_SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR-NUD   # 0b0010
SEC_SPD_SOURCES += hynix_dimm_H9CCNNN8GTALAR-NUD   # 0b0011
SEC_SPD_SOURCES += micron_dimm_MT52L1G32D4PG-107   # 0b0100
SEC_SPD_SOURCES += hynix_dimm_H9CCNNNCLGALAR-NUD   # 0b0101
SEC_SPD_SOURCES += nayna_dimm_NT6CL256T32CM-H1     # 0b0110

bootblock-y += gpio.c

romstage-y += memory.c

ramstage-y += gpio.c
ramstage-y += nhlt.c
ramstage-y += mainboard.c

# Add OEM ID table
cbfs-files-y += oem.bin
oem.bin-file := $(call strip_quotes,$(CONFIG_OEM_BIN_FILE))
oem.bin-type := raw
