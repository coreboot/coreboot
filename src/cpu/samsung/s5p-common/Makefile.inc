romstage-y += cpu_info.c
romstage-y += pwm.c	# needed by timer.c
romstage-y += s5p_gpio.c
romstage-y += timer.c

#romstage-y += sromc.c
#romstage-y += wdt.c

ramstage-y += cpu_info.c
ramstage-y += pwm.c	# needed by timer.c
ramstage-y += timer.c
ramstage-y += s5p_gpio.c
