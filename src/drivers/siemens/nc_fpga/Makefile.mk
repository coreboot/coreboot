## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_DRIVER_SIEMENS_NC_FPGA) += nc_fpga.c

all-$(CONFIG_NC_FPGA_POST_CODE) += nc_fpga_early.c

ifeq ($(CONFIG_NC_FPGA_POST_CODE),y)
CPPFLAGS_common += -I$(src)/drivers/siemens/nc_fpga
endif
