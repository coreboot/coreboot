# SPDX-License-Identifier: BSD-3-Clause

ec_usb_pd_fwtoolobj = main.o utils.o rom.o fmap.o kv_pair.o valstr.o
ec_usb_pd_fwheader = $(addprefix $(dir)/,utils.h rom.h)

WERROR ?= -Wno-error
EC_USB_PD_FWCFLAGS ?= $(CFLAGS)
EC_USB_PD_FWCFLAGS += -Wno-array-bounds -Wextra -MMD -MP -O3 -Wshadow $(WERROR)
EC_USB_PD_FWCFLAGS += -I $(top)/util/cbfstool/flashmap/
EC_USB_PD_FWCFLAGS += -I $(top)/util/amdtools/ec_usb_pd_fw
EC_USB_PD_FWCFLAGS += -I $(top)/src/commonlib/bsd/include

additional-dirs += $(objutil)/ec_usb_pd_fw

$(objutil)/ec_usb_pd_fw/%.o: $(top)/util/amdtools/ec_usb_pd_fw/%.c $(ec_usb_pd_fwheader) | $(objutil)/ec_usb_pd_fw
	printf "    PD_FWTOOL  $@\n"
	$(HOSTCC) $(EC_USB_PD_FWCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/ec_usb_pd_fw/%.o: $(top)/util/cbfstool/flashmap/%.c $(ec_usb_pd_fwheader) | $(objutil)/ec_usb_pd_fw
	$(HOSTCC) $(EC_USB_PD_FWCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/ec_usb_pd_fw/ec_usb_pd_fw: $(addprefix $(objutil)/ec_usb_pd_fw/,$(ec_usb_pd_fwtoolobj)) $(ec_usb_pd_fwheader) | $(objutil)/ec_usb_pd_fw
	printf "    PD_FWTOOL  $@\n"
	$(HOSTCC) $(addprefix $(objutil)/ec_usb_pd_fw/,$(ec_usb_pd_fwtoolobj)) $(LDFLAGS) -o $@
