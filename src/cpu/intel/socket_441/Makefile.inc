subdirs-y += ../model_106cx
subdirs-y += ../../x86/lapic
subdirs-y += ../microcode
subdirs-y += ../speedstep

bootblock-y += ../car/p4-netburst/cache_as_ram.S
bootblock-y += ../car/bootblock.c
postcar-y += ../car/p4-netburst/exit_car.S

romstage-y += ../car/romstage.c
