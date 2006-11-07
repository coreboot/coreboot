#
# lar makefile. This file is GPL. All rights reserved.
#
# (c) 2006 coresystems GmbH
#

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
