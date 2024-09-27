## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_SNOWRIDGE),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT) += ../../../cpu/intel/turbo

all-$(CONFIG_HIGH_SPEED_UART) += common/uart8250mem.c
all-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += common/gpio.c
all-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PMC) += common/pmclib.c
all-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SPI) += common/spi.c

bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += bootblock/bootblock.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU) += bootblock/early_uart_init.c

romstage-y += ../../../cpu/intel/car/romstage.c
romstage-y += common/fsp_hob.c
romstage-y += common/kti_cache.c
romstage-y += romstage/gpio_snr.c
romstage-y += romstage/romstage.c
romstage-$(CONFIG_DISPLAY_HOBS) += common/hob_display.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += common/upd_display.c
romstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += common/reset.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += common/systemagent_early.c

ramstage-y += common/fsp_hob.c
ramstage-y += common/kti_cache.c
ramstage-y += chip.c
ramstage-y += finalize.c
ramstage-$(CONFIG_DISPLAY_HOBS) += common/hob_display.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += common/upd_display.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += common/reset.c
ramstage-$(CONFIG_PCI) += dlb.c
ramstage-$(CONFIG_PCI) += heci.c
ramstage-$(CONFIG_PCI) += lpc.c
ramstage-$(CONFIG_PCI) += nis.c
ramstage-$(CONFIG_PCI) += pcie_rp.c
ramstage-$(CONFIG_PCI) += qat.c
ramstage-$(CONFIG_PCI) += sata.c
ramstage-$(CONFIG_PCI) += sriov.c
ramstage-$(CONFIG_PCI) += systemagent.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_ACPI) += acpi.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT) += cpu.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_ITSS) += itss.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SA_SERVER) += common/systemagent_early.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_PCH_LOCKDOWN) += lockdown.c

smm-$(CONFIG_ARCH_RAMSTAGE_X86_32) += smihandler.c
smm-$(CONFIG_HIGH_SPEED_UART) += common/uart8250mem.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO) += common/gpio.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PMC) += common/pmclib.c
smm-$(CONFIG_SPI_FLASH_SMM) += common/spi.c

CPPFLAGS_common += -I$(src)/soc/intel/snowridge/include/

## Set FSP binary blobs memory location

$(call strip_quotes,$(CONFIG_FSP_T_CBFS))-position := $(CONFIG_FSP_T_LOCATION) --xip
$(call strip_quotes,$(CONFIG_FSP_M_CBFS))-position := $(CONFIG_FSP_M_ADDR) --xip
$(call strip_quotes,$(CONFIG_FSP_S_CBFS))-position := $(CONFIG_FSP_S_ADDR) --xip

endif ## CONFIG_SOC_INTEL_SNOWRIDGE
