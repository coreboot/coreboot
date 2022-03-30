# Wed Apr 25, 2018
STABLE_COMMIT_ID=902681db13c170fc2d028f7391a9e9a5f69082a3

project_name=depthcharge
project_dir=$(CURDIR)/depthcharge
project_git_repo=https://chromium.googlesource.com/chromiumos/platform/depthcharge
project_config_file=$(project_dir)/.config
output_dir=$(project_dir)/build
libpayload_dir=$(abspath $(CURDIR)/../../libpayload)
libpayload_install_dir=$(output_dir)/lp_$(BOARD)
coreboot_build_dir=$(abspath $(CURDIR)/../../../build)

VBOOT_SOURCE ?= $(abspath $(CURDIR)/../../../3rdparty/vboot)
EC_HEADERS ?= $(abspath $(CURDIR)/../../../3rdparty/chromeec/include)

TAG-$(DEPTHCHARGE_MASTER)=origin/main
TAG-$(DEPTHCHARGE_STABLE)=$(STABLE_COMMIT_ID)
TAG-$(DEPTHCHARGE_REVISION)=$(DEPTHCHARGE_REVISION_ID)

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES
unexport src srck obj objk

BOARD:=$(notdir $(CONFIG_MAINBOARD_DIR))

ifeq ($(OVERRIDE_DEFCONFIG),y)
libpayload_config=$(libpayload_dir)/configs/defconfig
DEPTHCHARGE_LIBPAYLOAD_MSG="Depthcharge: Using default defconfig for libpayload"
else
libpayload_config=$(libpayload_dir)/configs/config.$(BOARD)
DEPTHCHARGE_LIBPAYLOAD_MSG="Depthcharge: Using $(libpayload_dir)/configs/config.$(BOARD)"
endif

all: build

$(project_dir):
	@echo "    Cloning $(project_name) from Git"
	@git clone $(project_git_repo) $(project_name)

fetch: $(project_dir)
	@cd $(project_dir); \
		git show $(TAG-y) >/dev/null 2>&1; \
		if [ $$? -ne 0 ] || \
		[ "$(TAG-y)" = "origin/main" ]; then \
		echo "    Fetching new commits from the $(project_name) git repo"; \
		git fetch; fi

# Check out the requested version of the tree
# Don't write a file for main branch so the latest remote version is always used
$(project_dir)/.version_$(TAG-y): fetch
	echo "    Checking out $(project_name) revision $(TAG-y)"
	rm -f $(project_dir)/.version_*
	cd $(project_dir); \
		git checkout main; \
		git branch -D coreboot 2>/dev/null; \
		git checkout -b coreboot $(TAG-y)
ifneq ($(DEPTHCHARGE_MASTER),y)
	touch $(project_dir)/.version_$(TAG-y)
endif

$(libpayload_install_dir): $(project_dir)
	test -f $(libpayload_config) || \
		(echo "Error: $(libpayload_config) is not present" && \
		false)
	echo $(DEPTHCHARGE_LIBPAYLOAD_MSG)
	cp $(libpayload_config) $(libpayload_dir)/.config
	(grep -q '^\s*CONFIG_LP_CHROMEOS=' $(libpayload_dir)/.config) || \
		(echo "CONFIG_LP_CHROMEOS=y" >> $(libpayload_dir)/.config)
	$(MAKE) -C $(libpayload_dir) olddefconfig
	$(MAKE) -C $(libpayload_dir)
	$(MAKE) -C $(libpayload_dir) install DESTDIR=$(libpayload_install_dir)

config: $(project_dir)/.version_$(TAG-y) $(libpayload_install_dir)
	echo "    CONFIG     project_name $(TAG-y)"
	export VERSION=$$(cd depthcharge && \
		git describe --tags --long --dirty 2>/dev/null || \
		echo "unknown") ; \
	cd $(project_dir) && \
		$(MAKE) BOARD=$(BOARD) \
		LIBPAYLOAD_DIR=$(libpayload_install_dir)/libpayload \
		VB_SOURCE=$(VBOOT_SOURCE) \
		EC_HEADERS=$(EC_HEADERS) defconfig

build: config
	echo "    MAKE       $(project_name) $(TAG-y)"
	cp $(coreboot_build_dir)/static_fw_config.h depthcharge/src/base/
	$(MAKE) -C $(project_dir) depthcharge BOARD=$(BOARD) \
		LIBPAYLOAD_DIR=$(libpayload_install_dir)/libpayload \
		VB_SOURCE=$(VBOOT_SOURCE) \
		EC_HEADERS=$(EC_HEADERS) \
		PATH="$(abspath ../../../build/util/cbfstool):$$PATH"

clean:
	rm -rf $(output_dir)

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: checkout config build clean distclean clone fetch print-repo-info
