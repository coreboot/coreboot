verstage-y += power_state.c

romstage-$(CONFIG_PLATFORM_USES_FSP1_1) += car_stage.S
romstage-$(CONFIG_PLATFORM_USES_FSP2_0) += car_stage_fsp20.S
romstage-y += pmc.c
romstage-y += power_state.c
romstage-$(CONFIG_PLATFORM_USES_FSP1_1) += romstage.c
romstage-$(CONFIG_PLATFORM_USES_FSP2_0) += romstage_fsp20.c
romstage-y += spi.c
romstage-y += systemagent.c
