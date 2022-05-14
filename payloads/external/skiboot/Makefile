## SPDX-License-Identifier: GPL-2.0-only

build_dir=$(CURDIR)/build
skiboot_dir=$(CURDIR)/skiboot
skiboot_git_repo=$(CONFIG_SKIBOOT_GIT_REPO)
skiboot_revision=$(CONFIG_SKIBOOT_REVISION)
skiboot_elf=$(build_dir)/skiboot.elf
skiboot_cross=$(or $(CROSS),powerpc64-linux-gnu-)

unexport $(COREBOOT_EXPORTS)

.PHONY: all clean distclean

all: $(skiboot_elf)

$(skiboot_elf): | $(skiboot_dir) $(build_dir)
	+$(MAKE) -C $(skiboot_dir) CROSS="$(skiboot_cross)"
	cp $(skiboot_dir)/skiboot.elf $@
	# skiboot is always built with debug information due to unconditional -ggdb
	$(skiboot_cross)strip $@

$(skiboot_dir):
	git clone $(skiboot_git_repo) $(skiboot_dir)
	git -C $(skiboot_dir) checkout $(skiboot_revision)

$(build_dir):
	mkdir -p $(build_dir)

distclean: clean
	rm -rf $(skiboot_dir)

clean:
	# Redefine RM because it's used like `$(RM) non-existent-file`
	# Also ignore useless messages about removing test files
	[ ! -d $(skiboot_dir) ] || \
	        $(MAKE) -C $(skiboot_dir) RM="rm -rf" CROSS="$(skiboot_cross)" clean > /dev/null
	rm -rf $(build_dir)
