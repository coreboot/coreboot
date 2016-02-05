ifeq ($(CONFIG_SOC_INTEL_APOLLOLAKE),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cache_as_ram.S
bootblock-y += bootblock/bootblock.c
bootblock-y += car.c
bootblock-y += gpio.c
bootblock-y += mmap_boot.c
bootblock-y += placeholders.c
bootblock-y += tsc_freq.c
bootblock-$(CONFIG_SOC_UART_DEBUG) += uart_early.c

romstage-y += placeholders.c
romstage-y += car.c
romstage-$(CONFIG_PLATFORM_USES_FSP2_0) += romstage.c
romstage-y += gpio.c
romstage-$(CONFIG_SOC_UART_DEBUG) += uart_early.c
romstage-y += memmap.c
romstage-y += mmap_boot.c
romstage-y += tsc_freq.c
romstage-y += pmutil.c

smm-y += placeholders.c
smm-y += pmutil.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += cpu.c
ramstage-y += chip.c
ramstage-y += placeholders.c
ramstage-y += gpio.c
ramstage-$(CONFIG_SOC_UART_DEBUG) += uart_early.c
ramstage-y += lpc.c
ramstage-y += memmap.c
ramstage-y += mmap_boot.c
ramstage-y += uart.c
ramstage-y += northbridge.c
ramstage-y += spi.c
ramstage-y += tsc_freq.c
ramstage-y += pmutil.c

postcar-y += exit_car.S
postcar-y += memmap.c
postcar-y += mmap_boot.c
postcar-$(CONFIG_SOC_UART_DEBUG) += uart_early.c
postcar-y += tsc_freq.c

CPPFLAGS_common += -I$(src)/soc/intel/apollolake/include

endif
