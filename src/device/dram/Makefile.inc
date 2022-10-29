
romstage-y += ddr_common.c
ramstage-y += ddr_common.c spd.c

romstage-$(CONFIG_USE_DDR5) += ddr5.c
ramstage-$(CONFIG_USE_DDR5) += ddr5.c

romstage-$(CONFIG_USE_LPDDR4) += lpddr4.c
ramstage-$(CONFIG_USE_LPDDR4) += lpddr4.c

romstage-$(CONFIG_USE_DDR4) += ddr4.c
ramstage-$(CONFIG_USE_DDR4) += ddr4.c

romstage-$(CONFIG_USE_DDR3) += ddr3.c
ramstage-$(CONFIG_USE_DDR3) += ddr3.c

romstage-$(CONFIG_USE_DDR2) += ddr2.c
ramstage-$(CONFIG_USE_DDR2) += ddr2.c
