subdirs-y += ../model_106cx
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../hyperthreading
subdirs-y += ../speedstep

bootblock-y += ../car/bootblock.c
bootblock-y += ../car/non-evict/cache_as_ram.S

postcar-y += ../car/non-evict/exit_car.S

romstage-y += ../car/romstage.c
