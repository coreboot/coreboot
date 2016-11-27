ifeq ($(CONFIG_NORTHBRIDGE_AMD_AMDFAM10),y)

# DDR3
romstage-$(CONFIG_HAVE_ACPI_RESUME) += s3utils.c
romstage-y += mct_d.c mctmtr_d.c mctcsi_d.c mctecc_d.c mctdqs_d.c mctsrc.c
romstage-y += mctsdi.c mctprod.c mctproc.c mctprob.c mcthwl.c mctwl.c
romstage-y += mport_d.c mutilc_d.c modtrdim.c mhwlc_d.c mctrci.c mctsrc1p.c
romstage-y += mcttmrl.c mcthdi.c mctndi_d.c mctchi_d.c modtrd.c mct_d_gcc.c

ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x11)
romstage-y += mctardk5.c
endif
ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x13)
romstage-y += mctardk5.c
endif
ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x14)
romstage-y += mctardk5.c
endif
ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x15)
romstage-y += mctardk5.c
endif
ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x16)
romstage-y += mctardk5.c
endif
ifeq ($(CONFIG_CPU_SOCKET_TYPE), 0x12)
romstage-y += mctardk6.c
endif

ramstage-$(CONFIG_HAVE_ACPI_RESUME) += s3utils.c

endif
