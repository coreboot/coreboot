## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_FSP_CAR),y)
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU)+= car/cache_as_ram_fsp.S
ifeq ($(CONFIG_NO_FSP_TEMP_RAM_EXIT),y)
postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += car/exit_car.S
else
postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += car/exit_car_fsp.S
endif
else
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CAR) += car/cache_as_ram.S
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CAR) += ../../../../../cpu/x86/early_reset.S
postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CAR) += car/exit_car.S
endif

bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += cpulib.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += cpulib.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += cpulib.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT) += mp_init.c
ramstage-$(CONFIG_CPU_SUPPORTS_PM_TIMER_EMULATION) += pm_timer_emulation.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_SMMRELOCATE) += smmrelocate.c
