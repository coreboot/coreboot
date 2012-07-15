ROMCCFLAGS := -mcpu=p4 -fno-simplify-phi -O2
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mptable.c
