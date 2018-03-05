
# IMPORTANT!! When a new memory SPD is added here, please ensure that ddp_bitmap
# is correctly updated in memory.c to indicate if memory id is using DDP.
SPD_SOURCES = empty_ddr4			# 0b0000
SPD_SOURCES += micron_dimm_MT40A256M16GE-083E	# 0b0001
SPD_SOURCES += samsung_dimm_K4A8G165WB-BCRC	# 0b0010
SPD_SOURCES += hynix_dimm_H5AN4G6NBJR-UHC	# 0b0011
SPD_SOURCES += hynix_dimm_H5ANAG6NAMR-UHC	# 0b0100

# Used when PCH_MEM_CONFIG4 = 1 (LPDDR3)
SECONDARY_SPD_SOURCES += micron_dimm_MT52L512M32D2PF-107 # 0b0000
SECONDARY_SPD_SOURCES += micron_dimm_MT52L256M32D1PF-107 # 0b0001
SECONDARY_SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR-NUD   # 0b0010
SECONDARY_SPD_SOURCES += hynix_dimm_H9CCNNN8GTALAR-NUD   # 0b0011
SECONDARY_SPD_SOURCES += micron_dimm_MT52L1G32D4PG-107   # 0b0100

bootblock-y += gpio.c

romstage-y += memory.c

ramstage-y += gpio.c
ramstage-y += nhlt.c
ramstage-y += pl2.c
