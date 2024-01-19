## SPDX-License-Identifier: GPL-2.0-only
# copied from cbootimage/src/Makefile.am
CBOOTIMAGE_RAW_SRCS:= \
	cbootimage.c \
	data_layout.c \
	set.c \
	crypto.c \
	aes_ref.c \
	context.c \
	parse.c \
	t210/parse_t210.c \
	t132/parse_t132.c \
	t124/parse_t124.c \
	t114/parse_t114.c \
	t30/parse_t30.c \
	t20/parse_t20.c \
	t210/nvbctlib_t210.c \
	t132/nvbctlib_t132.c \
	t124/nvbctlib_t124.c \
	t114/nvbctlib_t114.c \
	t30/nvbctlib_t30.c \
	t20/nvbctlib_t20.c \
	cbootimage.h \
	context.h \
	crypto.h \
	data_layout.h \
	nvaes_ref.h \
	parse.h \
	set.h \
	t20/nvboot_bct_t20.h \
	t20/nvboot_sdram_param_t20.h \
	t30/nvboot_bct_t30.h \
	t30/nvboot_sdram_param_t30.h \
	t114/nvboot_bct_t114.h \
	t114/nvboot_sdram_param_t114.h \
	t124/nvboot_bct_t124.h \
	t124/nvboot_sdram_param_t124.h \
	t132/nvboot_bct_t132.h \
	t132/nvboot_sdram_param_t132.h \
	t210/nvboot_bct_t210.h \
	t210/nvboot_sdram_param_t210.h

CBOOTIMAGE_SRCS:=$(addprefix util/nvidia/cbootimage/src/,$(filter %.c,$(CBOOTIMAGE_RAW_SRCS)))
CBOOTIMAGE_OBJS:=$(addprefix $(objutil)/nvidia/cbootimage/,$(patsubst %.c,%.o,$(filter %.c,$(CBOOTIMAGE_RAW_SRCS))))
CBOOTIMAGE_FLAGS:=-Wall -std=c99 -O2

ifeq ($(OS_ARCH), FreeBSD)
CBOOTIMAGE_FLAGS:=-DENODATA=ENOATTR
endif

additional-dirs += $(sort $(dir $(CBOOTIMAGE_OBJS)))

$(objutil)/nvidia/cbootimage/%.o: util/nvidia/cbootimage/src/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(CBOOTIMAGE_FLAGS) -c -o $@ $<

$(CBOOTIMAGE): $(CBOOTIMAGE_OBJS)
	printf "    HOSTCC     $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCC) $(CBOOTIMAGE_FLAGS) -o $@.tmp $^ -lm
	mv $@.tmp $@
