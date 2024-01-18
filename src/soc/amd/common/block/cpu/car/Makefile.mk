## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_CAR),y)

bootblock-y += cache_as_ram.S
bootblock-y += ap_exit_car.S
bootblock-y += exit_car.S

postcar-y += exit_car.S

romstage-y += ap_exit_car.S
romstage-y += exit_car.S

endif # CONFIG_SOC_AMD_COMMON_BLOCK_CAR
