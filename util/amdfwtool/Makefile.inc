# SPDX-License-Identifier: BSD-3-Clause

amdfwtoolobj = amdfwtool.o data_parse.o
amdfwreadobj = amdfwread.o

AMDFWTOOLCFLAGS=-O2 -Wall -Wextra -Wshadow -Werror
AMDFWTOOLCFLAGS += -I $(top)/src/commonlib/bsd/include

HOSTPKGCONFIG ?= pkg-config

AMDFWTOOLCFLAGS += $(shell $(HOSTPKGCONFIG) --cflags libcrypto)
LDFLAGS += $(shell $(HOSTPKGCONFIG) --libs libcrypto)

$(objutil)/amdfwtool/%.o: $(top)/util/amdfwtool/%.c # $(HEADER)
	$(HOSTCC) $(AMDFWTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/amdfwtool/amdfwtool: $(addprefix $(objutil)/amdfwtool/,$(amdfwtoolobj))
	printf "   AMDFWTOOL\n"
	$(HOSTCC) $(addprefix $(objutil)/amdfwtool/,$(amdfwtoolobj)) $(LDFLAGS) -o $@

$(objutil)/amdfwtool/amdfwread: $(addprefix $(objutil)/amdfwtool/,$(amdfwreadobj))
	printf "   AMDFWREAD\n"
	$(HOSTCC) $(addprefix $(objutil)/amdfwtool/,$(amdfwreadobj)) $(LDFLAGS) -o $@
