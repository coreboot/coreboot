## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
romstage-y += memory.c
ramstage-y += gpio.c

all-$(CONFIG_NC_FPGA_POST_CODE) += post.c
