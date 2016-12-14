# release-R50-7978.B
STABLE_COMMIT_ID=124af94fa5599a0698e59bf3d73675eb52fc6879

project_name=depthcharge
project_dir=$(CURDIR)/depthcharge
project_git_repo=https://chromium.googlesource.com/chromiumos/platform/depthcharge
project_config_file=$(project_dir)/.config
output_dir=$(project_dir)/build
libpayload_dir=$(abspath $(CURDIR)/../../libpayload)
libpayload_install_dir=$(output_dir)/lp_$(BOARD)

VBOOT_SOURCE ?= $(abspath $(CURDIR)/../../../3rdparty/vboot)

TAG-$(DEPTHCHARGE_MASTER)=origin/master
TAG-$(DEPTHCHARGE_STABLE)=$(STABLE_COMMIT_ID)

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES
unexport src srck obj objk

BOARD:=$(notdir $(CONFIG_MAINBOARD_DIR))

all: build

$(project_dir):
	echo "    Cloning $(project_name) from Git"
	git clone $(project_git_repo)

fetch: $(project_dir)
	cd $(project_dir); git show $(TAG-y) >/dev/null 2>&1 ;	if [ $$? -ne 0 ] || \
		[ "$(TAG-y)" = "origin/master" ]; then \
		echo "    Fetching new commits from the $(project_name) git repo"; \
		git fetch; fi

checkout: fetch
	echo "    Checking out $(project_name) revision $(TAG-y)"
	cd $(project_dir) ; git checkout master; git branch -D coreboot 2>/dev/null; git checkout -b coreboot $(TAG-y)

$(libpayload_install_dir): $(project_dir)
	test -f $(libpayload_dir)/configs/config.$(BOARD) || \
		(echo "Error: $(libpayload_dir)/configs/config.$(BOARD) is not present" && \
		false)
	cp $(libpayload_dir)/configs/config.$(BOARD) $(libpayload_dir)/.config
	$(MAKE) -C $(libpayload_dir) olddefconfig
	$(MAKE) -C $(libpayload_dir)
	$(MAKE) -C $(libpayload_dir) install DESTDIR=$(libpayload_install_dir)
#	rm -f $(libpayload_dir)/.config

config: $(libpayload_install_dir) checkout
	echo "    CONFIG     project_name $(TAG-y)"
	export VERSION=$$(cd depthcharge && \
		git describe --tags --long --dirty 2>/dev/null || \
		echo "unknown") ; \
	cd $(project_dir) && $(MAKE) BOARD=$(BOARD) LIBPAYLOAD_DIR=$(libpayload_install_dir)/libpayload \
		VB_SOURCE=$(VBOOT_SOURCE) defconfig

build: config
	echo "    MAKE       $(project_name) $(TAG-y)"
	cd $(project_dir) && $(MAKE) BOARD=$(BOARD) LIBPAYLOAD_DIR=$(libpayload_install_dir)/libpayload \
		VB_SOURCE=$(VBOOT_SOURCE) PATH="$(abspath ../../../build/util/cbfstool):$$PATH" depthcharge

clean:
	test -d $(output_dir) && rm -rf $(output_dir) || exit 0

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: checkout config build clean distclean clone fetch print-repo-info
