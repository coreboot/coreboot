## 
## lar - LinuxBIOS archiver
## 
## Copright (C) 2006 coresystems GmbH
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

SOURCE = lar.c create.c extract.c list.c lib.c

CC=gcc
CFLAGS=-O2 -Wall -W

lar: $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $^
	strip -s $@

example: example.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf lar tree.lar tree tree2 example


tree:
	@printf "creating sample tree... "
	@rm -rf tree
	@mkdir tree
	@mkdir -p tree/compression
	@mkdir -p tree/normal
	@mkdir -p tree/fallback
	@dd if=/dev/urandom of=tree/compression/lzma bs=1k count=8 &>/dev/null
	@dd if=/dev/urandom of=tree/normal/linuxbios.elf.lzma bs=1k count=32 &>/dev/null
	@dd if=/dev/urandom of=tree/normal/initmem bs=1k count=16 &>/dev/null
	@dd if=/dev/urandom of=tree/fallback/linuxbios.elf.lzma bs=1k count=32 &>/dev/null
	@dd if=/dev/urandom of=tree/fallback/initmem bs=1k count=16 &>/dev/null
	@printf "done.\n"

tree.lar: lar tree
	cd tree; ../lar c ../tree.lar `find . -type f|cut -c3-`

test: lar example tree.lar
	mkdir tree2; cd tree2; ../lar x ../tree.lar
	@echo Comparing tree:
	diff -urN tree tree2
	@rm -rf tree tree2
	./example tree.lar
