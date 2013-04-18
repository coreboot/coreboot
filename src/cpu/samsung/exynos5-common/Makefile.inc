romstage-y += spi.c
ramstage-y += spi.c
bootblock-y += spi.c

bootblock-y += gpio.c
bootblock-$(CONFIG_EARLY_CONSOLE) += pwm.c
bootblock-$(CONFIG_EARLY_CONSOLE) += timer.c

romstage-y += pwm.c	# needed by timer.c
romstage-y += gpio.c
romstage-y += timer.c
romstage-y += i2c.c

#romstage-y += wdt.c
#romstage-y += sromc.c

ramstage-y += cpu_info.c
ramstage-y += pwm.c	# needed by timer.c
ramstage-y += timer.c
ramstage-y += gpio.c
ramstage-y += i2c.c
ramstage-y += s5p-dp-reg.c
ramstage-y += exynos-fb.c
ramstage-y += exynos-tmu.c
