# SPDX-License-Identifier: BSD-3-Clause

ifneq ($(BUILD_ALL_TOOLS)$(CONFIG_USE_AMDFWTOOL),)

amdfwtoolobj = amdfwtool.o data_parse.o signed_psp.o handle_file.o opts.o soc.o
amdfwreadobj = amdfwread.o psp_fw_parser.o psp_fw_bhd.o psp_fw_psp.o psp_fw_efs.o psp_fw_cmb.o psp_fw_ish.o psp_fw_util.o region.o
amdfwheader = amdfwtool.h

WERROR ?= -Werror
AMDFWTOOLCFLAGS :=-O2 -Wall -Wextra -Wshadow $(WERROR)
AMDFWTOOLCFLAGS += -I $(top)/src/commonlib/bsd/include -I $(top)/src/commonlib/include
AMDFWTOOLCFLAGS += -I $(top)/src/
AMDFWTOOLCFLAGS += -D_GNU_SOURCE # memmem() from string.h
AMDFWTOOLCFLAGS += -std=gnu23
AMDFWTOOLCFLAGS += -ffunction-sections -fdata-sections -Wno-sign-compare

ifneq ($(PKG_CONFIG),)
HOSTPKGCONFIG ?= $(PKG_CONFIG)
else
HOSTPKGCONFIG ?= pkg-config
endif

AMDFWTOOLCFLAGS += $(shell $(HOSTPKGCONFIG) --cflags libcrypto)
ifneq ($(.SHELLSTATUS),0)
$(error "Ensure that pkg-config is installed.")
endif
LDFLAGS += $(shell $(HOSTPKGCONFIG) --libs libcrypto) -Wl,-gc-sections

$(objutil)/amdfwtool/%.o: $(top)/util/amdfwtool/%.c $(dir)/$(amdfwheader)
	printf "    AMDFW      $@\n"
	$(HOSTCC) $(AMDFWTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/amdfwtool/%.o: $(top)/src/vendorcode/amd/psp/%.c
	printf "    AMDFW      $@\n"
	$(HOSTCC) $(AMDFWTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/amdfwtool/%.o: $(top)/src/commonlib/%.c
	printf "    AMDFW      $@\n"
	$(HOSTCC) $(AMDFWTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/amdfwtool/amdfwtool: $(addprefix $(objutil)/amdfwtool/,$(amdfwtoolobj)) $(dir)/$(amdfwheader)
	printf "    AMDFWTOOL  $@\n"
	$(HOSTCC) $(addprefix $(objutil)/amdfwtool/,$(amdfwtoolobj)) $(LDFLAGS) -o $@

$(objutil)/amdfwtool/amdfwread: $(addprefix $(objutil)/amdfwtool/,$(amdfwreadobj)) $(dir)/$(amdfwheader)
	printf "    AMDFWREAD  $@\n"
	$(HOSTCC) $(addprefix $(objutil)/amdfwtool/,$(amdfwreadobj)) $(LDFLAGS) -o $@

endif # ifneq ($(BUILD_ALL_TOOLS)$(CONFIG_USE_AMDFWTOOL),)
