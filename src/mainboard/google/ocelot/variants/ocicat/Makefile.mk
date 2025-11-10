## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
romstage-y += gpio.c
romstage-y += memory.c
romstage-$(CONFIG_FW_CONFIG) += fw_config.c
romstage-$(CONFIG_CHROMEOS_DRAM_PART_NUMBER_IN_CBI) += part_num_to_dram_id.c
ramstage-y += gpio.c
romstage-$(CONFIG_FW_CONFIG) += variant.c
ramstage-$(CONFIG_FW_CONFIG) += variant.c
ramstage-$(CONFIG_FW_CONFIG) += fw_config.c
