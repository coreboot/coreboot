# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_QUARK),y)

subdirs-y += romstage

bootblock-y += bootblock/esram_init.S
bootblock-y += bootblock/bootblock.c
bootblock-y += i2c.c
bootblock-y += reg_access.c
bootblock-y += tsc_freq.c
bootblock-y += uart_common.c

verstage-y += i2c.c
verstage-y += reg_access.c
verstage-y += tsc_freq.c
verstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart_common.c

romstage-y += i2c.c
romstage-y += memmap.c
romstage-y += reg_access.c
romstage-$(CONFIG_STORAGE_TEST) += storage_test.c
romstage-y += tsc_freq.c
romstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart_common.c
romstage-y += reset.c

postcar-y += fsp_params.c
postcar-y += i2c.c
postcar-y += reg_access.c
postcar-y += tsc_freq.c
postcar-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart_common.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += chip.c
ramstage-y += ehci.c
ramstage-y += fsp_params.c
ramstage-y += gpio_i2c.c
ramstage-y += i2c.c
ramstage-y += lpc.c
ramstage-y += northcluster.c
ramstage-y += reg_access.c
ramstage-y += reset.c
ramstage-y += sd.c
ramstage-y += spi.c
ramstage-y += spi_debug.c
ramstage-$(CONFIG_STORAGE_TEST) += storage_test.c
ramstage-y += tsc_freq.c
ramstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart_common.c
ramstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart.c

CPPFLAGS_common += -I$(src)/soc/intel/quark
CPPFLAGS_common += -I$(src)/soc/intel/quark/include

# Chipset microcode path
CPPFLAGS_common += -I3rdparty/blobs/soc/intel/quark

# Since FSP-M runs in CAR we need to relocate it to a specific address
$(call strip_quotes,$(CONFIG_FSP_M_CBFS))-options := -b $(CONFIG_FSP_ESRAM_LOC)

# Add the FSP binary to the CBFS image
cbfs-files-$(CONFIG_ADD_FSP_RAW_BIN) += fsp.bin
fsp.bin-file := $(call strip_quotes,$(CONFIG_FSP_FILE))
fsp.bin-position := $(CONFIG_FSP_LOC)
fsp.bin-type := raw

# Add the chipset microcode file to the CBFS image
cbfs-files-$(CONFIG_ADD_RMU_FILE) += rmu.bin
rmu.bin-file := $(call strip_quotes,$(CONFIG_RMU_FILE))
rmu.bin-position := $(CONFIG_RMU_LOC)
rmu.bin-type := raw

endif # CONFIG_SOC_INTEL_QUARK
