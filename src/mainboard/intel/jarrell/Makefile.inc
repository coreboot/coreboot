ROMCCFLAGS := -mcpu=p4 -O2
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mptable.c
