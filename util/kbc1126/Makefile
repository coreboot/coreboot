## SPDX-License-Identifier: GPL-2.0-only
obj = kbc1126_ec_dump kbc1126_ec_insert
HOSTCC := $(if $(shell type gcc 2>/dev/null),gcc,cc)

all: $(obj)

%: %.c
	$(HOSTCC) -Wall -o $@ $<

clean:
	rm -f kbc1126_ec_dump kbc1126_ec_insert

.PHONY: all clean
