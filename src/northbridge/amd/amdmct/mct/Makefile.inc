ifeq ($(CONFIG_NORTHBRIDGE_AMD_AMDFAM10),y)

# DDR2
romstage-y += mct_d.c mct_d_gcc.c mctcsi_d.c mctmtr_d.c mctecc_d.c
romstage-y += mctpro_d.c mctdqs_d.c mctsrc.c mctsrc1p.c mcttmrl.c
romstage-y += mcthdi.c mctndi_d.c mctchi_d.c

ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x10)
romstage-y += mctardk3.c
endif

ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x11)
romstage-y += mctardk4.c
endif

endif
