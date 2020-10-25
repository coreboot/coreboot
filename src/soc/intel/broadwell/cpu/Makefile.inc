subdirs-y += ../../../../cpu/x86/lapic
subdirs-y += ../../../../cpu/x86/mtrr
subdirs-y += ../../../../cpu/x86/smm
subdirs-y += ../../../../cpu/x86/tsc
subdirs-y += ../../../../cpu/intel/microcode
subdirs-y += ../../../../cpu/intel/turbo
subdirs-y += ../../../../cpu/intel/common

bootblock-y += bootblock.c
bootblock-y += ../../../../cpu/intel/car/bootblock.c
bootblock-y += ../../../../cpu/intel/car/non-evict/cache_as_ram.S
bootblock-y += ../../../../cpu/x86/early_reset.S

romstage-y += romstage.c
romstage-y += ../../../../cpu/intel/car/romstage.c

postcar-y += ../../../../cpu/intel/car/non-evict/exit_car.S

ramstage-y += cpu.c
ramstage-y += smmrelocate.c

bootblock-y += tsc_freq.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-y      += tsc_freq.c
postcar-y  += tsc_freq.c
verstage-y += tsc_freq.c

cpu_microcode_bins += 3rdparty/blobs/soc/intel/broadwell/microcode.bin
