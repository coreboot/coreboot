# This is an example from Exynos5250.
bootblock-y += pinmux.c mct.c power.c
# Clock is required for UART
bootblock-$(CONFIG_EARLY_CONSOLE) += clock_init.c
bootblock-$(CONFIG_EARLY_CONSOLE) += clock.c
bootblock-$(CONFIG_EARLY_CONSOLE) += monotonic_timer.c
bootblock-$(CONFIG_EARLY_CONSOLE) += soc.c
bootblock-$(CONFIG_EARLY_CONSOLE) += uart.c

#romstage-y += clock.c
#romstage-y += clock_init.c
#romstage-y += pinmux.c  # required by s3c24x0_i2c (exynos5-common) and uart.
#romstage-y += dmc_common.c
#romstage-y += dmc_init_ddr3.c
#romstage-y += power.c
#romstage-y += mct.c
#romstage-y += monotonic_timer.c
#romstage-$(CONFIG_EARLY_CONSOLE) += soc.c
#romstage-$(CONFIG_EARLY_CONSOLE) += uart.c
#
##ramstage-y += tzpc_init.c
#ramstage-y += clock.c
#ramstage-y += clock_init.c
#ramstage-y += pinmux.c
#ramstage-y += power.c
#ramstage-y += soc.c
#ramstage-$(CONFIG_CONSOLE_SERIAL_UART) += uart.c
#ramstage-y += cpu.c
#ramstage-y += monotonic_timer.c
