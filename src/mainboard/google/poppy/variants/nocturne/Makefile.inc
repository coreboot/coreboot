
SPD_SOURCES  = empty				# 0b0000
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF	# 0b0001  4GB
SPD_SOURCES += hynix_dimm_H9CCNNN8GTALAR-NUD	# 0b0010  4GB
SPD_SOURCES  += samsung_dimm_K4E6E304EB-EGCF	# 0b0011  8GB
SPD_SOURCES += hynix_dimm_H9CCNNNBJTALAR-NUD	# 0b0100  8GB
SPD_SOURCES += samsung_dimm_K4EBE304EB-EGCG	# 0b0101 16GB
SPD_SOURCES += hynix_dimm_H9CCNNNCLGALAR-NUD	# 0b0110 16GB
SPD_SOURCES += micron_dimm_MT52L512M32D2PF-107	# 0b0111  8GB
SPD_SOURCES += micron_dimm_MT52L1G32D4PG-107	# 0b1000 16GB
SPD_SOURCES += micron_dimm_MT52L256M32D1PF-107	# 0b1001  4GB
SPD_SOURCES += samsung_dimm_K4E6E304EC-EGCF	# 0b1010  8GB
SPD_SOURCES += samsung_dimm_K4EBE304EC-EGCF	# 0b1011 16GB
SPD_SOURCES += nayna_dimm_NT6CL256T32CM-H1	# 0b1100  4GB

bootblock-y += gpio.c

romstage-y += memory.c

ramstage-y += gpio.c
ramstage-y += nhlt.c
ramstage-y += mainboard.c
ramstage-y += ec.c

smm-y += ec.c
