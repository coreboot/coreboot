cpu_incs += $(src)/soc/intel/broadwell/romstage/cache_as_ram.inc

romstage-y += cpu.c
romstage-y += pch.c
romstage-y += power_state.c
romstage-y += raminit.c
romstage-y += report_platform.c
romstage-y += romstage.c
romstage-y += smbus.c
romstage-y += spi.c
romstage-y += stack.c
romstage-y += systemagent.c
romstage-$(CONFIG_CONSOLE_SERIAL8250MEM) += uart.c
