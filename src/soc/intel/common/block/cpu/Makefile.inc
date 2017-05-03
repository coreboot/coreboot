bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CAR) += car/cache_as_ram.S
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += cpulib.c

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CAR) += car/exit_car.S
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += cpulib.c

postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CAR) += car/exit_car.S

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += cpulib.c
