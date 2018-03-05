bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_CAR) += car/cache_as_ram.S
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_CAR) += car/ap_exit_car.S
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_CAR) += car/exit_car.S

postcar-$(CONFIG_SOC_AMD_COMMON_BLOCK_CAR) += car/exit_car.S

romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_CAR) += car/exit_car.S
