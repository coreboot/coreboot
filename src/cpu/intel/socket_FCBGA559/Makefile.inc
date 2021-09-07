subdirs-y += ../model_106cx
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../microcode
subdirs-y += ../speedstep

bootblock-y += ../car/bootblock.c
bootblock-y += ../car/non-evict/cache_as_ram.S

postcar-y += ../car/non-evict/exit_car.S

romstage-y += ../car/romstage.c
