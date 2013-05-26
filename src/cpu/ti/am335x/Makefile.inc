bootblock-y	+= dmtimer.c
bootblock-y	+= nand.c
bootblock-$(CONFIG_EARLY_CONSOLE) += uart.c

romstage-y	+= nand.c
romstage-y	+= uart.c

ramstage-y	+= dmtimer.c
ramstage-y	+= monotonic_timer.c
ramstage-y	+= nand.c
ramstage-y	+= timer.c
ramstage-y	+= uart.c
