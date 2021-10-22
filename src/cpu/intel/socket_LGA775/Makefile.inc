subdirs-y += ../model_6fx
subdirs-y += ../model_f3x
subdirs-y += ../model_f4x
subdirs-y += ../model_1067x
subdirs-y += ../microcode
subdirs-y += ../speedstep

bootblock-y += ../car/p4-netburst/cache_as_ram.S
bootblock-y += ../car/bootblock.c
bootblock-y += ../../x86/early_reset.S

postcar-y += ../car/p4-netburst/exit_car.S

romstage-y += ../car/romstage.c
