bootblock-y += gpio.c

ramstage-y += gpio.c
ramstage-y += mainboard.c

all-$(CONFIG_NC_FPGA_POST_CODE) += post.c
