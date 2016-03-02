romstage-y += hand_off_block.c
romstage-y += util.c
romstage-y += memory_init.c

ramstage-y += graphics.c
ramstage-y += hand_off_block.c
ramstage-y += notify.c
ramstage-y += silicon_init.c
ramstage-y += util.c

CPPFLAGS_common += -I$(src)/drivers/intel/fsp2_0/include
