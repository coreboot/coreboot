bootblock-y += cbfs.c
bootblock-y += timer.c

romstage-y += cbfs.c
romstage-y += cbmem.c
romstage-y += timer.c

ramstage-y += cbfs.c
ramstage-y += cbmem.c
ramstage-y += timer.c

CPPFLAGS_common += -Isrc/soc/nvidia/tegra132/include/
