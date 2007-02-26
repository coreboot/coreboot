## 
## lar - LinuxBIOS archiver
## 
## Copyright (C) 2006 coresystems GmbH
## Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
## 

SOURCE := lar.c create.c extract.c list.c lib.c

$(obj)/util/lar/lar: $(patsubst %,$(src)/util/lar/%,$(SOURCE))
	$(Q)printf "Building LAR utility... "
	$(Q)mkdir -p $(obj)/util/lar
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ $^
	$(Q)printf "done\n"


## --------------------------------------------------------------------
## Stuff below this line is for debugging purposes only.

ifdef DEBUG_LAR
example: example.c
	$(Q)$(CC) $(CFLAGS) -o $@ $^

clean:
	$(Q)rm -rf lar tree.lar tree tree2 example

tree:
	$(Q)printf "creating sample tree... "
	$(Q)rm -rf tree
	$(Q)mkdir tree
	$(Q)mkdir -p tree/compression
	$(Q)mkdir -p tree/normal
	$(Q)mkdir -p tree/fallback
	$(Q)dd if=/dev/urandom of=tree/compression/lzma bs=1k count=8 &>/dev/null
	$(Q)dd if=/dev/urandom of=tree/normal/linuxbios.elf.lzma bs=1k count=32 &>/dev/null
	$(Q)dd if=/dev/urandom of=tree/normal/initmem bs=1k count=16 &>/dev/null
	$(Q)dd if=/dev/urandom of=tree/fallback/linuxbios.elf.lzma bs=1k count=32 &>/dev/null
	$(Q)dd if=/dev/urandom of=tree/fallback/initmem bs=1k count=16 &>/dev/null
	$(Q)printf "done.\n"

tree.lar: lar tree
	$(Q)cd tree; ../lar c ../tree.lar `find . -type f|cut -c3-`

test: lar example tree.lar
	$(Q)mkdir tree2; cd tree2; ../lar x ../tree.lar
	$(Q)echo Comparing tree:
	$(Q)diff -urN tree tree2
	$(Q)rm -rf tree tree2
	$(Q)./example tree.lar
endif
