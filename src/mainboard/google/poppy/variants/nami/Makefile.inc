
# IMPORTANT!! When a new memory SPD is added here, please ensure that ddp_bitmap
# is correctly updated in memory.c to indicate if memory id is using DDP.
SPD_SOURCES = empty     # 0b0000

bootblock-y += gpio.c

romstage-y += memory.c

ramstage-y += gpio.c
ramstage-y += pl2.c
