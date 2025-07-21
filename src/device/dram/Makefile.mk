## SPDX-License-Identifier: GPL-2.0-only

romstage-y += ddr_common.c
ramstage-y += ddr_common.c
ramstage-y += spd.c

romstage-$(CONFIG_DRAM_SUPPORT_DDR5) += ddr5.c
ramstage-$(CONFIG_DRAM_SUPPORT_DDR5) += ddr5.c

romstage-$(CONFIG_DRAM_SUPPORT_LPDDR4) += lpddr4.c
ramstage-$(CONFIG_DRAM_SUPPORT_LPDDR4) += lpddr4.c

romstage-$(CONFIG_DRAM_SUPPORT_DDR4) += ddr4.c
romstage-$(CONFIG_DRAM_SUPPORT_DDR4) += rcd.c
ramstage-$(CONFIG_DRAM_SUPPORT_DDR4) += ddr4.c

romstage-$(CONFIG_DRAM_SUPPORT_DDR3) += ddr3.c
ramstage-$(CONFIG_DRAM_SUPPORT_DDR3) += ddr3.c

romstage-$(CONFIG_DRAM_SUPPORT_DDR2) += ddr2.c
ramstage-$(CONFIG_DRAM_SUPPORT_DDR2) += ddr2.c
