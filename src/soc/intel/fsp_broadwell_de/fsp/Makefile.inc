romstage-y += chipset_fsp_util.c

cbfs-files-$(CONFIG_FSP_MEMORY_DOWN_CH0DIMM0_SPD_PRESENT) += spd_ch0_dimm0.bin
spd_ch0_dimm0.bin-file := $(call strip_quotes,$(CONFIG_FSP_MEMORY_DOWN_CH0DIMM0_SPD_FILE))
spd_ch0_dimm0.bin-type := spd

cbfs-files-$(CONFIG_FSP_MEMORY_DOWN_CH0DIMM1_SPD_PRESENT) += spd_ch0_dimm1.bin
spd_ch0_dimm1.bin-file := $(call strip_quotes,$(CONFIG_FSP_MEMORY_DOWN_CH0DIMM1_SPD_FILE))
spd_ch0_dimm1.bin-type := spd

cbfs-files-$(CONFIG_FSP_MEMORY_DOWN_CH1DIMM0_SPD_PRESENT) += spd_ch1_dimm0.bin
spd_ch1_dimm0.bin-file := $(call strip_quotes,$(CONFIG_FSP_MEMORY_DOWN_CH1DIMM0_SPD_FILE))
spd_ch1_dimm0.bin-type := spd

cbfs-files-$(CONFIG_FSP_MEMORY_DOWN_CH1DIMM1_SPD_PRESENT) += spd_ch1_dimm1.bin
spd_ch1_dimm1.bin-file := $(call strip_quotes,$(CONFIG_FSP_MEMORY_DOWN_CH1DIMM1_SPD_FILE))
spd_ch1_dimm1.bin-type := spd
