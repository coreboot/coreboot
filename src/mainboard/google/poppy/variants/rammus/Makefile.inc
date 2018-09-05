SPD_SOURCES  = micron_dimm_MT52L512M32D2PF-093	# 0b0000
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF     # 0b0001
SPD_SOURCES += micron_dimm_MT52L256M32D1PF-093	# 0b0010
SPD_SOURCES += samsung_dimm_K4E6E304EC-EGCF     # 0b0011

bootblock-y += gpio.c

romstage-y += memory.c

ramstage-y += gpio.c
ramstage-y += nhlt.c
ramstage-y += mainboard.c
