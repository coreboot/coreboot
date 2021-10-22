subdirs-y += ../model_1067x
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../microcode
subdirs-y += ../speedstep

bootblock-y += ../car/core2/cache_as_ram.S
bootblock-y += ../car/bootblock.c
postcar-y += ../car/p4-netburst/exit_car.S

romstage-y += ../car/romstage.c
