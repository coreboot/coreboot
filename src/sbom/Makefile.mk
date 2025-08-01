## SPDX-License-Identifier: GPL-2.0-only

obj ?= build
src ?= src
build-dir = $(obj)/sbom
src-dir = $(src)/sbom

# Strip quotes from binary paths and SBOM file paths. Each binary path should have a
# corresponding SBOM file path, but not every SBOM file path needs a binary path. That
# is because binary files are only needed if they are used to extract information from
# them which in turn can be included in the SBOM files (like version or config stuff).
# But for some Software there are only SBOM files, which basically tell the most generic
# information about that piece of Software. Ideally one would not need the binary files
# at all, because extacting information out of mostly unknown binary blobs is a pain.
CONFIG_ME_BIN_PATH         := $(call strip_quotes, $(CONFIG_ME_BIN_PATH))
CONFIG_SBOM_ME_PATH        := $(call strip_quotes, $(CONFIG_SBOM_ME_PATH))
CONFIG_FSP_S_FILE          := $(call strip_quotes, $(CONFIG_FSP_S_FILE))
CONFIG_FSP_M_FILE          := $(call strip_quotes, $(CONFIG_FSP_M_FILE))
CONFIG_FSP_T_FILE          := $(call strip_quotes, $(CONFIG_FSP_T_FILE))
CONFIG_SBOM_FSP_PATH       := $(call strip_quotes, $(CONFIG_SBOM_FSP_PATH))
CONFIG_PAYLOAD_FILE        := $(call strip_quotes, $(CONFIG_PAYLOAD_FILE))
CONFIG_SBOM_PAYLOAD_PATH   := $(call strip_quotes, $(CONFIG_SBOM_PAYLOAD_PATH))
CONFIG_EC_PATH             := $(call strip_quotes, $(CONFIG_EC_PATH))
CONFIG_SBOM_EC_PATH        := $(call strip_quotes, $(CONFIG_SBOM_EC_PATH))
CONFIG_SBOM_BIOS_ACM_PATH  := $(call strip_quotes, $(CONFIG_SBOM_BIOS_ACM_PATH))
CONFIG_SBOM_SINIT_ACM_PATH := $(call strip_quotes, $(CONFIG_SBOM_SINIT_ACM_PATH))
CONFIG_SBOM_COMPILER_PATH  := $(call strip_quotes, $(CONFIG_SBOM_COMPILER_PATH))

# Select the correct payload directory for the used payload. Ideally we could just make this
# a one-liner, but since the payload is generated externally (with an extra make command), we
# have to hard code the paths here.
ifeq ($(CONFIG_SBOM_PAYLOAD_GENERATE), y)
payload-git-dir-$(CONFIG_PAYLOAD_BOOTBOOT)    = payloads/external/BOOTBOOT/bootboot
payload-git-dir-$(CONFIG_PAYLOAD_DEPTHCHARGE) = payloads/external/depthcharge/depthcharge
payload-git-dir-$(CONFIG_PAYLOAD_FILO)        = payloads/external/FILO/filo
payload-git-dir-$(CONFIG_PAYLOAD_GRUB2)       = payloads/external/GRUB2/grub2
payload-git-dir-$(CONFIG_PAYLOAD_LINUXBOOT)   = payloads/external/LinuxBoot/linuxboot
payload-git-dir-$(CONFIG_PAYLOAD_SEABIOS)     = payloads/external/SeaBIOS/seabios
payload-git-dir-$(CONFIG_PAYLOAD_SKIBOOT)     = payloads/external/skiboot/skiboot
#payload-git-dir-$(CONFIG_PAYLOAD_TIANOCORE)   = payloads/external/tianocore/
payload-git-dir-$(CONFIG_PAYLOAD_UBOOT)       = payloads/external/U-Boot/u-boot
payload-git-dir-$(CONFIG_PAYLOAD_IPXE)        = payloads/external/iPXE/ipxe
ifneq ($(payload-git-dir-y),)
# only proceed with payload sbom data, if one of the above payloads were selected (should be guarded by Kconfig as well)
# e.g. payload-git-dir-y=payloads/external/SeaBIOS/seabios -> payload-json-file=$(build-dir)/payload-SeaBIOS.json
payload-swid = $(build-dir)/payload-$(subst /,,$(dir $(patsubst payloads/external/%,%,$(payload-git-dir-y)))).json
payload-swid-template = $(patsubst $(build-dir)/%.json,$(src-dir)/%.json,$(payload-swid))
endif
endif

# Add all SBOM files into the swid-files-y target. This target contains all
# .json, .ini, .uswid, .xml, .pc SBOM files that are later merged into one uSWID SBOM file.
# Some of these have an option that this Makefile generates/extracts some information from
# binary files in order to give more complete/detailed information inside the SBOM file.
# These files are either in src/sbom/ or build/sbom (if they are generated).
swid-files-$(CONFIG_SBOM_ME) += $(if $(CONFIG_SBOM_ME_GENERATE), $(build-dir)/intel-me.json, $(CONFIG_SBOM_ME_PATH))
swid-files-$(CONFIG_SBOM_PAYLOAD) += $(if $(CONFIG_SBOM_PAYLOAD_GENERATE), $(payload-swid), $(CONFIG_SBOM_PAYLOAD_PATH))
# TODO think about just using one CoSWID tag for all intel-microcode instead of one for each. maybe put each microcode into files entity of CoSWID tag?
swid-files-$(CONFIG_SBOM_MICROCODE) += $(patsubst 3rdparty/intel-microcode/intel-ucode/%, $(build-dir)/intel-microcode-%.json, $(filter 3rdparty/intel-microcode/intel-ucode/%, $(cpu_microcode_bins)))
swid-files-$(CONFIG_SBOM_MICROCODE) += $(patsubst ${FIRMWARE_LOCATION}/UcodePatch_%.bin, $(build-dir)/amd-microcode-%.json, $(filter ${FIRMWARE_LOCATION}/UcodePatch_%.bin, $(cpu_microcode_bins)))
swid-files-$(CONFIG_SBOM_FSP) += $(CONFIG_SBOM_FSP_PATH)
swid-files-$(CONFIG_SBOM_EC) += $(CONFIG_SBOM_EC_PATH)
swid-files-$(CONFIG_SBOM_BIOS_ACM) += $(CONFIG_BIOS_ACM_PATH)
swid-files-$(CONFIG_SBOM_SINIT_ACM) += $(CONFIG_SINIT_ACM_PATH)

vboot-pkgconfig-files = $(obj)/external/vboot_reference-bootblock/vboot_host.pc $(obj)/external/vboot_reference-ramstage/vboot_host.pc $(obj)/external/vboot_reference-postcar/vboot_host.pc
ifeq ($(CONFIG_SEPARATE_ROMSTAGE),y)
vboot-pkgconfig-files += $(obj)/external/vboot_reference-romstage/vboot_host.pc
endif
swid-files-$(CONFIG_SBOM_VBOOT) += $(vboot-pkgconfig-files)
$(vboot-pkgconfig-files): $(VBOOT_LIB_bootblock) $(VBOOT_LIB_romstage) $(VBOOT_LIB_ramstage) $(VBOOT_LIB_postcar) # src/security/vboot/Makefile.mk

ifeq ($(CONFIG_SBOM_COMPILER),y)
compiler-toolchain = $(CC_bootblock) $(CC_romstage) $(CC_ramstage) $(CC_postcar) $(CC_verstage) $(LD_bootblock) $(LD_romstage) $(LD_ramstage) $(LD_postcar) $(LD_verstage) $(AS_bootblock) $(AS_romstage) $(AS_ramstage) $(AS_postcar) $(AS_verstage)
swid-files-compiler = $(CONFIG_SBOM_COMPILER_PATH)
endif

# include all licenses used in coreboot. Ideally we would only include the licenses,
# which are used in this build
coreboot-licenses = $(foreach license, $(patsubst %.txt, %, $(filter-out retained-copyrights.txt, $(patsubst LICENSES/%, %, $(wildcard LICENSES/*)))), https://spdx.org/licenses/$(license).html)

# only include CBFS SBOM section if there is any data for it
ifeq ($(CONFIG_SBOM),y)
cbfs-files-y += sbom
sbom-file = $(build-dir)/sbom.uswid
sbom-type = raw
endif

## Build final SBOM (Software Bill of Materials) file in uswid format

$(build-dir)/sbom.uswid: $(build-dir)/coreboot.json $$(swid-files-y) $(swid-files-compiler) | $(build-dir)/goswid $(build-dir)
	echo "    SBOM      " $^
	$(build-dir)/goswid convert -o $@ \
		--parent $(build-dir)/coreboot.json \
		$(if $(swid-files-y), --requires $$(echo $(swid-files-y) | tr ' ' ','),) \
		$(if $(swid-files-compiler), --compiler $(swid-files-compiler),)

# all build files depend on the $(build-dir) directory being created
$(build-dir):
	mkdir -p $(build-dir)

$(build-dir)/goswid: | $(build-dir)
	echo "    SBOM      building goswid tool"
	cd util/goswid; \
	GOPATH=$(abspath build/go) GO111MODULE=on go build -modcacherw -o $(abspath $@) ./cmd/goswid

## Generate all .json files

$(build-dir)/compiler-%.json: $(src-dir)/compiler-%.json | $(build-dir)/goswid
	cp $< $@
	for tool in $$(echo $(compiler-toolchain) | tr ' ' '\n' | sort | uniq); do \
		version=$$($$tool --version 2>&1 | head -n 1 | grep -Eo '([0-9]+\.[0-9]+\.*[0-9]*)'); \
		$(build-dir)/goswid add-payload-file -o $@ -i $@ --name $$(basename $$tool) --version $$version; \
	done

$(build-dir)/coreboot.json: $(src-dir)/coreboot.json $(obj)/build.h | $(build-dir)/goswid
	cp $< $@
	git_tree_hash=$$(grep 'COREBOOT_ORIGIN_TREE_REVISION' $(obj)/build.h | sed 's/.*"\(.*\)".*/\1/');\
	git_comm_hash=$$(grep 'COREBOOT_ORIGIN_GIT_REVISION' $(obj)/build.h | sed 's/.*"\(.*\)".*/\1/');\
	sed -i -e "s/<colloquial_version>/$$git_tree_hash/" -e "s/<software_version>/$$git_comm_hash/" $@;\
	$(build-dir)/goswid add-license -o $@ -i $@ $(coreboot-licenses)

$(build-dir)/intel-me.json: $(src-dir)/intel-me.json $(CONFIG_ME_BIN_PATH) | $(build-dir)
	cp $< $@
	#TODO put more Intel Management Engine metadata in sbom file


$(build-dir)/generic-fsp.json: $(src-dir)/generic-fsp.json $(CONFIG_FSP_S_FILE) $(CONFIG_FSP_T_FILE) $(CONFIG_FSP_M_FILE) | $(build-dir)/goswid
	cp $(src-dir)/generic-fsp.json $@
ifneq ($(CONFIG_FSP_S_FILE),)
	echo "    SBOM      Adding FSP-S"
	$(build-dir)/goswid add-payload-file -o $@ -i $@ --name "FSP-S"
endif
ifneq ($(CONFIG_FSP_T_FILE),)
	echo "    SBOM      Adding FSP-T"
	$(build-dir)/goswid add-payload-file -o $@ -i $@ --name "FSP-T"
endif
ifneq ($(CONFIG_FSP_M_FILE),)
	echo "    SBOM      Adding FSP-M"
	$(build-dir)/goswid add-payload-file -o $@ -i $@ --name "FSP-M"
endif

$(build-dir)/intel-microcode-%.json: $(src-dir)/intel-microcode.json 3rdparty/intel-microcode/intel-ucode/% | $(build-dir) $(build-dir)/goswid
	cp $< $@
	year=$$(hexdump --skip 8 --length 2 --format '"%04x"' $(word 2,$^));\
	day=$$(hexdump --skip 10 --length 1 --format '"%02x"' $(word 2,$^));\
	month=$$(hexdump --skip 11 --length 1 --format '"%02x"' $(word 2,$^));\
	sed -i "s/<software_version>/$$year-$$month-$$day/" $@
	#TODO add cpuid (processor family, model, stepping) as extra attribute

$(build-dir)/amd-microcode-%.json: $(src-dir)/amd-microcode.json ${FIRMWARE_LOCATION}/UcodePatch_%.bin | $(build-dir) $(build-dir)/goswid
	cp $< $@
	year=$$(hexdump --skip 0 --length 2 --format '"%04x"' $(word 2,$^));\
	day=$$(hexdump --skip 2 --length 1 --format '"%02x"' $(word 2,$^));\
	month=$$(hexdump --skip 3 --length 1 --format '"%02x"' $(word 2,$^));\
	sed -i "s/<software_version>/$$year-$$month-$$day/" $@

$(payload-swid): $(payload-swid-template) $(CONFIG_PAYLOAD_FILE) | $(build-dir)
	cp $< $@;\
	git_tree_hash=$$(git --git-dir $(payload-git-dir-y)/.git log -n 1 --format=%T);\
	git_comm_hash=$$(git --git-dir $(payload-git-dir-y)/.git log -n 1 --format=%H);\
	sed -i -e "s/<colloquial_version>/$$git_tree_hash/" -e "s/<software_version>/$$git_comm_hash/" $@;
