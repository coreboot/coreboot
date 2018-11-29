cpu_incs-y += $(src)/cpu/intel/car/non-evict/cache_as_ram.S

romstage-y += cpu.c
romstage-y += pch.c
romstage-y += power_state.c
romstage-y += raminit.c
romstage-y += report_platform.c
romstage-y += romstage.c
romstage-y += smbus.c
romstage-y += spi.c
romstage-y += systemagent.c
romstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart.c
